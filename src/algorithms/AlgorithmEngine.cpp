#include "AlgorithmEngine.h"

#include <QDateTime>

AlgorithmEngine::AlgorithmEngine(AlgorithmType type, QObject* parent)
    : QObject(parent)
    , m_strategy(AlgorithmFactory::create(type))
{
    m_pool.name = m_strategy->name();
}

AllocationResult AlgorithmEngine::allocate(int sizeKB, int processId, const QString& label)
{
    AllocationResult result;
    ++m_allocationAttempts;

    if (sizeKB <= 0 || processId < 0) {
        result.status = AllocationResult::FAILED_INVALID_REQUEST;
        result.failureReason = QStringLiteral("Invalid allocation request");
        appendEvent(SimEvent::ALLOC_FAIL, result.failureReason, -1, -1, processId);
        return result;
    }

    int index = m_strategy->findBlock(m_pool, sizeKB);
    if (index < 0) {
        ++m_failedAllocations;
        result.status = AllocationResult::FAILED_NO_BLOCK;
        result.failureReason = QStringLiteral("No free block found of sufficient size (requested %1 KB, largest free %2 KB)")
                                   .arg(sizeKB)
                                   .arg(m_pool.largestFreeBlockKB());
        appendEvent(SimEvent::ALLOC_FAIL, result.failureReason, -1, -1, processId);
        return result;
    }

    result.status = AllocationResult::SUCCESS;
    result.blockIndex = index;

    split(index, sizeKB, result);

    auto* block = m_pool.blockAt(index);
    if (block == nullptr) {
        ++m_failedAllocations;
        result.status = AllocationResult::FAILED_POOL_FULL;
        result.failureReason = QStringLiteral("Allocation failed due to invalid block state");
        appendEvent(SimEvent::ALLOC_FAIL, result.failureReason, -1, -1, processId);
        return result;
    }

    block->isFree = false;
    block->processId = processId;
    block->processLabel = label;

    result.blockId = block->blockId;
    ++m_allocationSuccesses;

    appendEvent(SimEvent::ALLOC_SUCCESS,
        QStringLiteral("ALLOC %1 -> BLK-%2 (%3 KB)").arg(label).arg(block->blockId).arg(sizeKB),
        block->blockId,
        -1,
        processId);

    return result;
}

AllocationResult AlgorithmEngine::deallocate(int blockId)
{
    AllocationResult result;
    result.blockId = blockId;

    const int index = indexByBlockId(blockId);
    if (index < 0) {
        result.status = AllocationResult::FAILED_NOT_FOUND;
        result.failureReason = QStringLiteral("Block BLK-%1 not found").arg(blockId);
        appendEvent(SimEvent::ALLOC_FAIL, result.failureReason, blockId);
        return result;
    }

    auto* block = m_pool.blockAt(index);
    if (block == nullptr || block->isFree) {
        result.status = AllocationResult::FAILED_INVALID_REQUEST;
        result.failureReason = QStringLiteral("Block BLK-%1 already free").arg(blockId);
        appendEvent(SimEvent::ALLOC_FAIL, result.failureReason, blockId);
        return result;
    }

    const int processId = block->processId;
    const QString label = block->processLabel;

    block->isFree = true;
    block->processId = -1;
    block->processLabel.clear();

    appendEvent(SimEvent::DEALLOC,
        QStringLiteral("DEALLOC %1 <- BLK-%2 freed").arg(label).arg(blockId),
        blockId,
        -1,
        processId);

    coalesce(indexByBlockId(blockId));

    result.status = AllocationResult::SUCCESS;
    return result;
}

void AlgorithmEngine::split(int blockIndex, int requestedSizeKB, AllocationResult& result)
{
    auto* block = m_pool.blockAt(blockIndex);
    if (block == nullptr) {
        return;
    }

    const int remainder = block->sizeKB - requestedSizeKB;
    if (remainder <= 20) {
        return;
    }

    MemoryBlock newBlock;
    newBlock.blockId = m_nextBlockId++;
    newBlock.startAddress = block->startAddress + requestedSizeKB;
    newBlock.sizeKB = remainder;
    newBlock.isFree = true;
    newBlock.processId = -1;
    newBlock.processLabel.clear();

    block->sizeKB = requestedSizeKB;
    m_pool.insertBlock(blockIndex + 1, newBlock);

    result.splitRemainder = remainder;
    result.newBlockId = newBlock.blockId;

    appendEvent(SimEvent::SPLIT,
        QStringLiteral("SPLIT BLK-%1 -> BLK-%1 (%2 KB) + BLK-%3 (%4 KB)")
            .arg(block->blockId)
            .arg(block->sizeKB)
            .arg(newBlock.blockId)
            .arg(newBlock.sizeKB),
        block->blockId,
        newBlock.blockId);

    emit splitOccurred(block->blockId, newBlock.blockId);
}

void AlgorithmEngine::coalesce(int blockIndex)
{
    if (blockIndex < 0 || blockIndex >= m_pool.blockCount()) {
        return;
    }

    bool changed = true;
    while (changed) {
        changed = false;

        if (blockIndex > 0) {
            auto* current = m_pool.blockAt(blockIndex);
            auto* left = m_pool.blockAt(blockIndex - 1);
            if (current != nullptr && left != nullptr && current->isFree && left->isFree) {
                const int survivorId = left->blockId;
                const int absorbedId = current->blockId;
                left->sizeKB += current->sizeKB;
                m_pool.removeBlock(blockIndex);
                blockIndex -= 1;
                appendEvent(SimEvent::MERGE,
                    QStringLiteral("MERGE BLK-%1 + BLK-%2 -> BLK-%1 (%3 KB)")
                        .arg(survivorId)
                        .arg(absorbedId)
                        .arg(left->sizeKB),
                    survivorId,
                    absorbedId);
                emit mergeOccurred(survivorId, absorbedId);
                changed = true;
            }
        }

        if (blockIndex < m_pool.blockCount() - 1) {
            auto* current = m_pool.blockAt(blockIndex);
            auto* right = m_pool.blockAt(blockIndex + 1);
            if (current != nullptr && right != nullptr && current->isFree && right->isFree) {
                const int survivorId = current->blockId;
                const int absorbedId = right->blockId;
                current->sizeKB += right->sizeKB;
                m_pool.removeBlock(blockIndex + 1);
                appendEvent(SimEvent::MERGE,
                    QStringLiteral("MERGE BLK-%1 + BLK-%2 -> BLK-%1 (%3 KB)")
                        .arg(survivorId)
                        .arg(absorbedId)
                        .arg(current->sizeKB),
                    survivorId,
                    absorbedId);
                emit mergeOccurred(survivorId, absorbedId);
                changed = true;
            }
        }
    }
}

FragmentationReport AlgorithmEngine::generateFragmentationReport() const
{
    FragmentationReport report;
    report.algorithmName = m_strategy->name();
    report.totalFreeKB = m_pool.totalFreeKB();
    report.freeBlockCount = m_pool.freeBlockCount();
    report.largestFreeBlockKB = m_pool.largestFreeBlockKB();
    report.externalFragmentationPresent = m_pool.hasExternalFragmentation();
    report.totalAllocatedKB = m_pool.totalAllocatedKB();
    report.failedAllocations = m_failedAllocations;

    const int totalBlocks = m_pool.blockCount();
    if (totalBlocks > 0) {
        report.fragmentationRatio = static_cast<double>(report.freeBlockCount) / static_cast<double>(totalBlocks);
    }

    for (const auto& block : m_pool.blocks) {
        if (block.isFree) {
            report.freeBlockSizes.push_back(block.sizeKB);
        }
    }

    return report;
}

const QVector<SimEvent>& AlgorithmEngine::eventLog() const
{
    return m_eventLog;
}

void AlgorithmEngine::resetPool(const QVector<MemoryBlock>& initial)
{
    m_pool.reset(initial);
    m_nextBlockId = initial.size();
    m_failedAllocations = 0;
    m_allocationAttempts = 0;
    m_allocationSuccesses = 0;
    m_eventLog.clear();
    appendEvent(SimEvent::INIT, QStringLiteral("Pool loaded with %1 blocks").arg(initial.size()));
}

QString AlgorithmEngine::name() const
{
    return m_strategy->name();
}

QString AlgorithmEngine::shortName() const
{
    return m_strategy->shortName();
}

int AlgorithmEngine::indexByBlockId(int blockId) const
{
    for (int index = 0; index < m_pool.blockCount(); ++index) {
        const auto* block = m_pool.blockAt(index);
        if (block != nullptr && block->blockId == blockId) {
            return index;
        }
    }
    return -1;
}

void AlgorithmEngine::appendEvent(SimEvent::Type type, const QString& detail, int blockA, int blockB, int processId)
{
    SimEvent event;
    event.timestamp = QDateTime::currentDateTime();
    event.type = type;
    event.algorithmName = m_strategy->name();
    event.detail = detail;
    event.blockIdA = blockA;
    event.blockIdB = blockB;
    event.processId = processId;
    m_eventLog.push_back(event);
}

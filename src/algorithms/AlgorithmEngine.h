#pragma once

#include <QObject>
#include <QVector>

#include <memory>

#include "AlgorithmFactory.h"
#include "IAllocationStrategy.h"
#include "core/AllocationResult.h"
#include "core/FragmentationReport.h"
#include "core/MemoryPool.h"
#include "core/SimEvent.h"

/**
 * @class AlgorithmEngine
 * @brief Owns one MemoryPool and one IAllocationStrategy.
 *        Implements allocate(), deallocate(), split(), and coalesce().
 *
 * @note Not thread-safe. All calls must be made from the owning thread.
 */
class AlgorithmEngine : public QObject {
    Q_OBJECT
public:
    explicit AlgorithmEngine(AlgorithmType type, QObject* parent = nullptr);

    AllocationResult allocate(int sizeKB, int processId, const QString& label);
    AllocationResult deallocate(int blockId);

    void split(int blockIndex, int requestedSizeKB, AllocationResult& result);
    void coalesce(int blockIndex);

    FragmentationReport generateFragmentationReport() const;
    const QVector<SimEvent>& eventLog() const;

    void resetPool(const QVector<MemoryBlock>& initial);

    const MemoryPool& pool() const { return m_pool; }
    MemoryPool& pool() { return m_pool; }
    QString name() const;
    QString shortName() const;
    int failedAllocations() const { return m_failedAllocations; }
    int allocationAttempts() const { return m_allocationAttempts; }
    int allocationSuccesses() const { return m_allocationSuccesses; }

signals:
    void splitOccurred(int originalBlockId, int newBlockId);
    void mergeOccurred(int survivorBlockId, int absorbedBlockId);

private:
    int indexByBlockId(int blockId) const;
    void appendEvent(SimEvent::Type type, const QString& detail, int blockA = -1, int blockB = -1, int processId = -1);

    MemoryPool m_pool;
    std::unique_ptr<IAllocationStrategy> m_strategy;
    QVector<SimEvent> m_eventLog;
    int m_nextBlockId = 0;
    int m_failedAllocations = 0;
    int m_allocationAttempts = 0;
    int m_allocationSuccesses = 0;
};

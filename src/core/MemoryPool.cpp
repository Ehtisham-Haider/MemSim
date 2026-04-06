#include "MemoryPool.h"

#include <algorithm>

void MemoryPool::reset(const QVector<MemoryBlock>& initial)
{
    blocks = initial;
    nextFitCursor = 0;
}

int MemoryPool::totalFreeKB() const
{
    int total = 0;
    for (const auto& block : blocks) {
        if (block.isFree) {
            total += block.sizeKB;
        }
    }
    return total;
}

int MemoryPool::totalAllocatedKB() const
{
    int total = 0;
    for (const auto& block : blocks) {
        if (!block.isFree) {
            total += block.sizeKB;
        }
    }
    return total;
}

int MemoryPool::freeBlockCount() const
{
    int count = 0;
    for (const auto& block : blocks) {
        if (block.isFree) {
            ++count;
        }
    }
    return count;
}

int MemoryPool::largestFreeBlockKB() const
{
    int largest = 0;
    for (const auto& block : blocks) {
        if (block.isFree) {
            largest = std::max(largest, block.sizeKB);
        }
    }
    return largest;
}

bool MemoryPool::hasExternalFragmentation() const
{
    int freeCount = 0;
    for (const auto& block : blocks) {
        if (block.isFree) {
            ++freeCount;
            if (freeCount > 1) {
                return true;
            }
        }
    }
    return false;
}

MemoryBlock* MemoryPool::blockAt(int index)
{
    if (index < 0 || index >= blocks.size()) {
        return nullptr;
    }
    return &blocks[index];
}

const MemoryBlock* MemoryPool::blockAt(int index) const
{
    if (index < 0 || index >= blocks.size()) {
        return nullptr;
    }
    return &blocks[index];
}

int MemoryPool::blockCount() const
{
    return blocks.size();
}

void MemoryPool::insertBlock(int index, const MemoryBlock& block)
{
    blocks.insert(index, block);
}

void MemoryPool::removeBlock(int index)
{
    blocks.removeAt(index);
}

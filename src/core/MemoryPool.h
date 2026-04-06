#pragma once

#include <QVector>

#include "MemoryBlock.h"

/**
 * @class MemoryPool
 * @brief Holds an ordered block list and pool-level metrics for one algorithm instance.
 */
class MemoryPool {
public:
    QVector<MemoryBlock> blocks;
    QString name;
    int nextFitCursor = 0;

    void reset(const QVector<MemoryBlock>& initial);

    int totalFreeKB() const;
    int totalAllocatedKB() const;
    int freeBlockCount() const;
    int largestFreeBlockKB() const;
    bool hasExternalFragmentation() const;

    MemoryBlock* blockAt(int index);
    const MemoryBlock* blockAt(int index) const;
    int blockCount() const;
    void insertBlock(int index, const MemoryBlock& block);
    void removeBlock(int index);
};

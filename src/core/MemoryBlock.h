#pragma once

#include <QString>

/**
 * @class MemoryBlock
 * @brief Represents one contiguous memory block in the simulated pool.
 */
class MemoryBlock {
public:
    int blockId = -1;
    int startAddress = 0;
    int sizeKB = 0;
    bool isFree = true;
    int processId = -1;
    QString processLabel;

    QString statusString() const;
    QString summary() const;
};

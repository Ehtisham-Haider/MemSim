#pragma once

#include <QString>

#include "core/MemoryPool.h"

class IAllocationStrategy {
public:
    virtual ~IAllocationStrategy() = default;
    virtual int findBlock(MemoryPool& pool, int sizeKB) = 0;
    virtual QString name() const = 0;
    virtual QString shortName() const = 0;
};

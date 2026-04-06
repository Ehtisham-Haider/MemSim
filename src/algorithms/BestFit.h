#pragma once

#include "IAllocationStrategy.h"

class BestFit final : public IAllocationStrategy {
public:
    int findBlock(MemoryPool& pool, int sizeKB) override;
    QString name() const override { return QStringLiteral("Best-Fit"); }
    QString shortName() const override { return QStringLiteral("bestFit"); }
};

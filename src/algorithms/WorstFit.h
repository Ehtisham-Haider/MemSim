#pragma once

#include "IAllocationStrategy.h"

class WorstFit final : public IAllocationStrategy {
public:
    int findBlock(MemoryPool& pool, int sizeKB) override;
    QString name() const override { return QStringLiteral("Worst-Fit"); }
    QString shortName() const override { return QStringLiteral("worstFit"); }
};

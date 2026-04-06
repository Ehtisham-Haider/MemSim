#pragma once

#include "IAllocationStrategy.h"

class NextFit final : public IAllocationStrategy {
public:
    int findBlock(MemoryPool& pool, int sizeKB) override;
    QString name() const override { return QStringLiteral("Next-Fit"); }
    QString shortName() const override { return QStringLiteral("nextFit"); }
};

#pragma once

#include "IAllocationStrategy.h"

class FirstFit final : public IAllocationStrategy {
public:
    int findBlock(MemoryPool& pool, int sizeKB) override;
    QString name() const override { return QStringLiteral("First-Fit"); }
    QString shortName() const override { return QStringLiteral("firstFit"); }
};

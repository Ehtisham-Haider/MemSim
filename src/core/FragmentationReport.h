#pragma once

#include <QString>
#include <QVector>

struct FragmentationReport {
    QString algorithmName;
    int totalFreeKB = 0;
    int freeBlockCount = 0;
    int largestFreeBlockKB = 0;
    bool externalFragmentationPresent = false;
    double fragmentationRatio = 0.0;
    int totalAllocatedKB = 0;
    int failedAllocations = 0;
    QVector<int> freeBlockSizes;
};

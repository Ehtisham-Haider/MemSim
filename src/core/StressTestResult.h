#pragma once

#include <QString>

struct StressTestResult {
    int requestIndex = 0;
    int sizeKB = 0;
    bool success = false;
    int blockId = -1;
    QString algorithmName;
};

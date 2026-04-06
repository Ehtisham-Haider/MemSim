#pragma once

#include <QString>

struct AllocationRequest {
    int sizeKB = 0;
    int processId = -1;
    QString processLabel;
    bool applyToAll = false;
};

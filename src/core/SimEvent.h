#pragma once

#include <QDateTime>
#include <QString>

struct SimEvent {
    enum Type {
        INIT,
        ALLOC_SUCCESS,
        ALLOC_FAIL,
        DEALLOC,
        SPLIT,
        MERGE,
        STRESS,
        RESET
    };

    QDateTime timestamp;
    Type type = INIT;
    QString algorithmName;
    QString detail;
    int blockIdA = -1;
    int blockIdB = -1;
    int processId = -1;
};

#pragma once

#include <QString>

struct AllocationResult {
    enum Status {
        SUCCESS,
        FAILED_NO_BLOCK,
        FAILED_POOL_FULL,
        FAILED_INVALID_REQUEST,
        FAILED_NOT_FOUND
    };

    Status status = FAILED_INVALID_REQUEST;
    int blockIndex = -1;
    int blockId = -1;
    int splitRemainder = 0;
    int newBlockId = -1;
    QString failureReason;

    bool isSuccess() const { return status == SUCCESS; }
};

#include "MemoryBlock.h"

QString MemoryBlock::statusString() const
{
    return isFree ? QStringLiteral("FREE") : QStringLiteral("ALLOCATED");
}

QString MemoryBlock::summary() const
{
    const QString owner = isFree ? QStringLiteral("-") : processLabel;
    return QStringLiteral("BLK-%1 | Start=%2 KB | Size=%3 KB | %4 | PID=%5")
        .arg(blockId)
        .arg(startAddress)
        .arg(sizeKB)
        .arg(statusString())
        .arg(owner);
}

#pragma once

#include <QVector>

#include "algorithms/AlgorithmEngine.h"

inline QVector<MemoryBlock> defaultTestPool()
{
    return {
        MemoryBlock { 0, 0, 60, true, -1, QString() },
        MemoryBlock { 1, 60, 100, true, -1, QString() },
        MemoryBlock { 2, 160, 30, true, -1, QString() },
        MemoryBlock { 3, 190, 80, true, -1, QString() },
        MemoryBlock { 4, 270, 50, true, -1, QString() },
        MemoryBlock { 5, 320, 200, true, -1, QString() },
        MemoryBlock { 6, 520, 40, true, -1, QString() },
        MemoryBlock { 7, 560, 150, true, -1, QString() },
        MemoryBlock { 8, 710, 90, true, -1, QString() },
        MemoryBlock { 9, 800, 224, true, -1, QString() },
    };
}

#include "BestFit.h"

#include <limits>

int BestFit::findBlock(MemoryPool& pool, int sizeKB)
{
    int bestIndex = -1;
    int bestSize = std::numeric_limits<int>::max();

    for (int index = 0; index < pool.blockCount(); ++index) {
        const auto* block = pool.blockAt(index);
        if (block == nullptr || !block->isFree || block->sizeKB < sizeKB) {
            continue;
        }

        if (block->sizeKB < bestSize) {
            bestSize = block->sizeKB;
            bestIndex = index;
        }
    }

    return bestIndex;
}

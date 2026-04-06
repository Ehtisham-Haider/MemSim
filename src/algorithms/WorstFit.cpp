#include "WorstFit.h"

int WorstFit::findBlock(MemoryPool& pool, int sizeKB)
{
    int worstIndex = -1;
    int worstSize = -1;

    for (int index = 0; index < pool.blockCount(); ++index) {
        const auto* block = pool.blockAt(index);
        if (block == nullptr || !block->isFree || block->sizeKB < sizeKB) {
            continue;
        }

        if (block->sizeKB > worstSize) {
            worstSize = block->sizeKB;
            worstIndex = index;
        }
    }

    return worstIndex;
}

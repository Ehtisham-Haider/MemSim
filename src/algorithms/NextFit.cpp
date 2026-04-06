#include "NextFit.h"

int NextFit::findBlock(MemoryPool& pool, int sizeKB)
{
    if (pool.blockCount() == 0) {
        return -1;
    }

    int start = pool.nextFitCursor;
    if (start < 0 || start >= pool.blockCount()) {
        start = 0;
    }

    int index = start;
    do {
        const auto* block = pool.blockAt(index);
        if (block != nullptr && block->isFree && block->sizeKB >= sizeKB) {
            pool.nextFitCursor = index;
            return index;
        }

        index = (index + 1) % pool.blockCount();
    } while (index != start);

    return -1;
}

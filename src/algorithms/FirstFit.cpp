#include "FirstFit.h"

int FirstFit::findBlock(MemoryPool& pool, int sizeKB)
{
    for (int index = 0; index < pool.blockCount(); ++index) {
        const auto* block = pool.blockAt(index);
        if (block != nullptr && block->isFree && block->sizeKB >= sizeKB) {
            return index;
        }
    }
    return -1;
}

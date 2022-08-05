#include "mon_memory.h"

namespace Mon
{
    void* PushSize_(MemoryArena* arena, size_t size)
    {
        //Assert((arena->used + size) <= arena->size);
        void* result = arena->base + arena->used;
        arena->used += size;
        return result;
    }
}
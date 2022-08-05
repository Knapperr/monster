#ifndef MON_MEMORY_H
#define MON_MEMORY_H

#include <cstdint>

namespace Mon
{
    struct MemoryArena
    {
        size_t size;
        uint8_t* base;
        size_t used;
    };

    static void InitializeArena(MemoryArena* arena, size_t size, uint8_t* base)
    {
        arena->size = size;
        arena->base = base;
        arena->used = 0;
    }

#define PushStruct(arena, type) (type *)PushSize_(arena, sizeof(type))
#define PushArray(arena, count, type) (type *)PushSize_(arena, (count)*sizeof(type))
    void* PushSize_(MemoryArena* arena, size_t size);
}

#endif
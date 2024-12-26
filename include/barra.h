#ifndef BARRA_H
#define BARRA_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#define MIN_BLOCK_SIZE 16
#define MAX_BLOCK_SIZE (1 << 22)   // 4MB
#define MAX_ORDERS 19              // Log2(MAX_BLOCK_SIZE/MIN_BLOCK_SIZE) + 1
#define ALIGNMENT 16               // Memory alignment requirement
#define FENCE_SIZE 16              // Size of memory fences

_Static_assert(FENCE_SIZE > sizeof(uint32_t), "FENCE_SIZE must be larger than sizeof(uint32_t)");

typedef enum {
    MAGIC_UNINITIALIZED = 0x00000000,
    MAGIC_ALLOCATED     = 0xDEADBEEF,
    MAGIC_FREED        = 0xDEADDEAD,
    MAGIC_FENCE        = 0xFEECFEEC
} BlockMagic;

typedef struct {
    size_t total_allocated;        // All the allocation sizes summed
    size_t total_freed;
    size_t peak_memory;
    size_t current_memory_usage;   // Currently allocated blocks sizes summed
    size_t total_allocations;
    size_t total_frees;
    size_t fragmentation;
    size_t failed_allocations;
    size_t invalid_frees;
} AllocatorStats;

typedef struct {
    uint32_t pattern[FENCE_SIZE / sizeof(uint32_t)];
} MemoryFence;

typedef struct BarraAllocator BarraAllocator;

BarraAllocator* barra_init(size_t total_size);
void* barra_alloc(BarraAllocator* allocator, size_t size);
void barra_free(BarraAllocator* allocator, void* ptr);
void barra_cleanup(BarraAllocator* allocator);
void barra_compact(BarraAllocator* allocator);
AllocatorStats barra_get_stats(BarraAllocator* allocator);

// Debug functions
void barra_dump_memory_info(BarraAllocator* allocator, void* ptr);
void barra_validate_all_blocks(BarraAllocator* allocator);
const char* barra_get_error_string(void);

#ifdef DEBUG
    #define BARRA_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                fprintf(stderr, "Barra: Assertion failed: %s\n", message); \
                abort(); \
            } \
        } while(0)
#else
    #define BARRA_ASSERT(condition, message) ((void)0)
#endif

#endif // BUDDY_ALLOCATOR_H

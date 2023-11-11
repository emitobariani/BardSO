#ifndef MY_MALLOC_MANAGER_H
#define MY_MALLOC_MANAGER_H

// Declaraciones de funciones
void *my_malloc(size_t nbytes);
void my_free(void *ptr);

typedef unsigned char *Bitmap;

// Declaraciones de estructuras
typedef struct MemoryChunkHeader {
    uint16_t id;                    // id of the chunk, useful for debugging
    uint16_t is_large_allocation;   // Flag to indicate if this is for a single large allocation
    uint16_t chunk_total_units;     // Size of the memory block in units
    uint16_t chunk_available_units; // How many units are available
    Bitmap bitmap;                  // Pointer to the bitmap for this chunk, NULL if is_large_allocation
    uint16_t bitmap_size;           // Size of bitmap in bytes, 0 if is_large_allocation
    struct MemoryChunkHeader *next; // Pointer to the next MemoryChunkHeader, NULL if last one
} MemoryChunkHeader;

typedef struct AllocationHeader {
    uint16_t nunits;    // number of units
    uint16_t bit_index; // offset from the MemoryChunkHeader struct
} AllocationHeader;

// Declaraciones de constantes
#define BITMAP_SIZE (uint16_t)16 // in bytes
#define UNIT_SIZE (uint16_t)16   // minimum unit to assign, in bytes
#define UNITS_PER_CHUNK (uint16_t)(BITMAP_SIZE * 8)
#define STRUCT_SIZE (uint16_t)((sizeof(MemoryChunkHeader) + UNIT_SIZE - 1)/UNIT_SIZE)
#define BITMAP_UNITS (uint16_t)((BITMAP_SIZE + UNIT_SIZE - 1)/UNIT_SIZE)
#define IS_LARGE_ALLOCATION (units) (units >= (UNITS_PER_CHUNK - STRUCT_SIZE - BITMAP_UNITS))
#define MAX_MALLOC_SIZE (size_t)16 * 1024 * 1024 // 16 MB


#endif

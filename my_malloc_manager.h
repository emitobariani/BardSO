// Standard includes
#include <errno.h>  // for errno
// #include <error.h>  // for error handling of system calls: man 3 error
#include <stddef.h> // for size_t
#include <stdio.h>  // for I/O functions
#include <stdlib.h> // for EXIT_FAILURE
#include <string.h> // for string manipulation functions
#include <stdint.h>
#include <sys/mman.h>

#ifndef MY_MALLOC_MANAGER_H
#define MY_MALLOC_MANAGER_H

// Declaraciones de funciones
void *my_malloc(size_t nbytes);
void my_free(void *ptr);

typedef unsigned char *Bitmap;

// Declaraciones de estructuras
typedef struct MemoryChunkHeader {
    uint16_t id;                    // id of the chunk, useful for debugging
    void *addr;                  // Address of the chunk
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
#define BITMAP_SIZE 16 // in bytes
#define UNIT_SIZE (uint16_t)16   // minimum unit to assign, in bytes
#define UNITS_PER_CHUNK (BITMAP_SIZE * 8)
#define STRUCT_SIZE (uint16_t)((sizeof(MemoryChunkHeader) + UNIT_SIZE - 1)/UNIT_SIZE)
#define BITMAP_UNITS (uint16_t)((BITMAP_SIZE + UNIT_SIZE - 1)/UNIT_SIZE)
#define IS_LARGE_ALLOCATION (units) (units >= (UNITS_PER_CHUNK - STRUCT_SIZE - BITMAP_UNITS))
#define MAX_MALLOC_SIZE (size_t)16 * 1024 * 1024 // 16 MB

int first_fit(unsigned char *bitmap, size_t bitmap_size, size_t units_needed);
void print_bitmap(unsigned char *bitmap, size_t bitmap_size);
void set_or_clear_bits(int set, Bitmap bitmap, uint16_t start_byte_index, uint16_t start_bit_index, uint16_t qty);
void* create_new_chunk(uint16_t units_needed, int is_large_allocation, MemoryChunkHeader *next);

#endif

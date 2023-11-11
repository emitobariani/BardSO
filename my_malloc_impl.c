#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>

#include "my_malloc_manager.h"

// Declaraciones de variables globales
extern void *first_chunk;
extern uint16_t chunk_current_id;

// Implementación de la función my_malloc
void *my_malloc(size_t nbytes) {
    if (nbytes == 0) {
        return NULL; // No se permite asignar memoria de tamaño 0
    }

    size_t units_needed = (nbytes + sizeof(AllocationHeader) - 1) / sizeof(AllocationHeader) + 1;

    MemoryChunkHeader *chunk = find_or_create_chunk(units_needed);

    if (chunk) {
        for (int i = 0; i < chunk->chunk_total_units; i++) {
            if ((i + units_needed) > chunk->chunk_total_units) {
                continue;
            }

            if (bitmap_find_free_space(chunk->bitmap, i, units_needed)) {
                AllocationHeader *header = (AllocationHeader *)((char *)chunk + sizeof(MemoryChunkHeader) + (i * sizeof(AllocationHeader)));
                header->nunits = units_needed;
                header->bit_index = i;

                chunk->chunk_available_units -= units_needed;
                return (void *)(header + 1);
            }
        }
    }

    return NULL;
}


// Implementación de la función my_free
void my_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    AllocationHeader *header = (AllocationHeader *)ptr - 1;

    MemoryChunkHeader *chunk = (MemoryChunkHeader *)((char *)header - sizeof(MemoryChunkHeader) - (header->bit_index * sizeof(AllocationHeader)));

    bitmap_mark_space_free(chunk->bitmap, header->bit_index, header->nunits);
    chunk->chunk_available_units += header->nunits;

    if (chunk->chunk_available_units == chunk->chunk_total_units) {
        MemoryChunkHeader *prev_chunk = chunk->prev;

        if (prev_chunk == NULL) {
            first_chunk = NULL;
        } else {
            prev_chunk->next = chunk->next;
        }

        munmap(chunk, chunk->chunk_total_units * UNIT_SIZE);
    }
}


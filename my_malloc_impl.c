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

    uint16_t units_needed = (nbytes + sizeof(AllocationHeader) + UNIT_SIZE - 1) / UNIT_SIZE;
    
    MemoryChunkHeader *header = (MemoryChunkHeader*) create_new_chunk(UNITS_PER_CHUNK, 0,NULL);

    if(first_chunk == NULL) {
        my_malloc_init();
    }
    //large location, create new chunk
    if (is_large_allocation) {
        chunk = create_new_chunk(units_needed, 1, first_chunk->next);
        bit_index = STRUCT_SIZE;
    } else {
        //small location, find chunk
        chunk = first_chunk;
        while (chunk != NULL) {
            bit_index = bitmap_find_space(chunk->bitmap, units_needed);
            if (bit_index != -1) {
                break;
            }
            chunk = chunk->next;
        }
        //no chunk found, create new chunk
        if (chunk == NULL) {
            chunk = create_new_chunk(units_needed, 0, first_chunk);
            bit_index = 0;
        }
    }
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


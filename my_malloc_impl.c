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

    // Calcula el número de unidades necesarias
    size_t units_needed = (nbytes + sizeof(AllocationHeader) - 1) / sizeof(AllocationHeader) + 1;

    // Busca un chunk disponible
    MemoryChunkHeader *chunk = find_or_create_chunk(units_needed);

    if (chunk) {
        // Encuentra un espacio en el chunk y marca los bits en el bitmap
        for (int i = 0; i < chunk->chunk_total_units; i++) {
            if ((i + units_needed) > chunk->chunk_total_units) {
                continue; // No hay suficiente espacio en este chunk
            }

            if (bitmap_find_free_space(chunk->bitmap, i, units_needed)) {
                // Asigna memoria
                AllocationHeader *header = (AllocationHeader *)(chunk + 1);
                header->nunits = units_needed;
                header->bit_index = i;

                chunk->chunk_available_units -= units_needed;
                return (void *)(header + 1);
            }
        }
    }

    return NULL; // No se pudo asignar memoria
}

// Implementación de la función my_free
void my_free(void *ptr) {
    if (ptr == NULL) {
        return; // No es necesario liberar memoria nula
    }

    // Obtiene el encabezado de la asignación
    AllocationHeader *header = (AllocationHeader *)ptr - 1;

    // Encuentra el chunk correspondiente
    MemoryChunkHeader *chunk = (MemoryChunkHeader *)((char *)header - (header->bit_index * UNIT_SIZE));

    // Marca los bits correspondientes en el bitmap como disponibles
    bitmap_mark_space_free(chunk->bitmap, header->bit_index, header->nunits);
    chunk->chunk_available_units += header->nunits;

    // Si el chunk está vacío, se puede liberar
    if (chunk->chunk_available_units == chunk->chunk_total_units) {
        // Busca el chunk anterior
        MemoryChunkHeader *prev_chunk = chunk->prev;

        // Si no hay chunk anterior, el chunk actual es el primero
        if (prev_chunk == NULL) {
            first_chunk = NULL;
        } else {
            // Elimina el chunk actual de la lista
            prev_chunk->next = chunk->next;
        }

        // Libera la memoria del chunk
        munmap(chunk, chunk->chunk_total_units * UNIT_SIZE);
    }
}

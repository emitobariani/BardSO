#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>

#include "my_malloc_manager.h"

// Declaraciones de variables globales
extern void *first_chunk;
extern uint16_t chunk_current_id;

// Implementación de la función set_or_clear_bits
void set_or_clear_bits(int set, Bitmap bitmap, uint16_t start_byte_index, uint16_t start_bit_index, uint16_t qty) {
    if (set) {
        // Escribe unos en el bitmap
        for (int i = 0; i < qty; i++) {
            bitmap[start_byte_index] |= (1 << (7 - (start_bit_index + i)));
        }
    } else {
        // Escribe ceros en el bitmap
        for (int i = 0; i < qty; i++) {
            bitmap[start_byte_index] &= ~(1 << (7 - (start_bit_index + i)));
        }
    }
}

// Implementación de la función create_new_chunk
void *create_new_chunk(uint16_t units_needed, int is_large_allocation, MemoryChunkHeader *next) {
    // Calcula el total de unidades para mmap
    uint16_t total_units_to_mmap = units_needed + sizeof(MemoryChunkHeader) / UNIT_SIZE;

    // Reserva memoria con mmap
    void *mem = mmap(NULL, total_units_to_mmap * UNIT_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (mem == MAP_FAILED) {
        error(EXIT_FAILURE, errno, "Memory mapping failed");
    }

    // Inicializa el header del chunk
    MemoryChunkHeader *new_chunk = (MemoryChunkHeader *)mem;
    new_chunk->id = chunk_current_id++;
    new_chunk->is_large_allocation = is_large_allocation;
    new_chunk->chunk_total_units = units_needed;
    new_chunk->chunk_available_units = units_needed;
    new_chunk->bitmap = NULL; // Establece en NULL por ahora
    new_chunk->bitmap_size = 0; // Establece en 0 por ahora
    new_chunk->next = next;

    // Devuelve la dirección del chunk
    return mem;
}


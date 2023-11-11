#include "my_malloc_manager.h"

// Declaraciones de variables globales
extern void *first_chunk;
extern uint16_t chunk_current_id;

int first_fit(unsigned char *bitmap, size_t bitmap_size, size_t units_needed) {
    size_t consecutive_free_bits = 0;
    size_t first_free_bit_index = -1;

    for (int i = 0; i < bitmap_size * 8; i++) {
        size_t byte = i / 8;
        size_t bit = i % 8;

        if (!(bitmap[byte] & (1 << (7-bit)))) {
            if (consecutive_free_bits == 0) {
                first_free_bit_index = i;
            }
            consecutive_free_bits++;
            if (consecutive_free_bits == units_needed) {
                for (int j = 0; j < units_needed; j++) {
                    int byte_to_set = (first_free_bit_index + j) / 8;
                    int bit_to_set = (first_free_bit_index + j) % 8;
                    bitmap[byte_to_set] |= (1 << (7 - bit_to_set));
                }
                return first_free_bit_index;
            }
        } else {
            consecutive_free_bits = 0;
        }
    }

    return -1; // No se encontró espacio adecuado.
}

void print_bitmap(unsigned char *bitmap, size_t bitmap_size) {
    for (size_t byte = 0; byte < bitmap_size; byte++) {
        
        unsigned char current_byte = bitmap[byte];

        for (int bit = 7; bit >= 0; bit--) {
            int bit_value = (current_byte >> bit) & 1;
            // Imprimimos cada bit
            printf("%d", bit_value);
        }

        // Si no es el último byte se imprime un espacio
        if (byte < bitmap_size - 1) {
            printf(" ");
        }
    }

    printf("\n");
}

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
    MemoryChunkHeader *new_chunk = NULL;
    uint16_t total_units_to_mmap = is_large_allocation ? units_needed + STRUCT_UNITS : UNITS_PER_CHUNK; 
    uint16_t used_units = is_large_allocation ? STRUCT_UNITS : STRUCT_UNITS + BITMAP_UNITS;
    // Reserva memoria con mmap
    void *mem = mmap(NULL, total_units_to_mmap * UNIT_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (mem == MAP_FAILED) {
        print("Error")
        //error(EXIT_FAILURE, errno, "Memory mapping failed");
    }


    // Inicializa el header del chunk
    new_chunk = (MemoryChunkHeader *)mem;
    new_chunk->address = mem;
    new_chunk->id = 0;
    new_chunk->is_large_allocation = is_large_allocation;
    new_chunk->chunk_total_units = total_units_to_mmap;
    new_chunk->chunk_available_units = total_units_to_mmap - used_units;
    new_chunk->bitmap = is_large_allocation ? NULL: (unsigned char *)mem + STRUCT_UNITS * UNIT_SIZE; // Establece en NULL por ahora
    new_chunk->bitmap_size = is_large_allocation ? 8 : BITMAP_SIZE; // Establece en 0 por ahora
    new_chunk->next = next;
    
    if(!is_large_allocation){
        // Inicializa el bitmap
        for (u_int16_t i = 0; i < used_units; i++) {
            u_int16_t byte_index = i / 8;
            u_int16_t bit_index = i % 8;
            new_chunk->bitmap[byte_index]  |= (1 << (7 - bit_index));
        }
    }

    return new_chunk;
}


#include "my_malloc_manager.h"


int main() {
    // Creamos e inicializamos el mapa de bits a cero
    // unsigned char bitmap[BITMAP_SIZE] = {0};
    //crear un chunk de 128 unidades
    MemoryChunkHeader* header = (MemoryChunkHeader*) create_new_chunk(UNITS_PER_CHUNK, 0, NULL);
    for (int i = 0; i < BITMAP_SIZE; i++){
        header->bitmap[i] = 0;
    }

    printf("Bitmap Size: %d bytes\n", BITMAP_SIZE);

    while (1) {
        printf("Enter the number of bits to occupy (0 to exit, -1 to set/clear): ");
        int action;
        scanf("%d", &action);
        if (action == 0) {
            break;
        } else if (action == -1) {
            int start_byte_index, start_bit_index, qty;
            printf("Enter start byte index: ");
            scanf("%d", &start_byte_index);
            printf("Enter start bit index: ");
            scanf("%d", &start_bit_index);
            printf("Enter quantity: ");
            scanf("%d", &qty);
            int set;
            printf("Enter 1 to set or 0 to clear: ");
            scanf("%d", &set);
            set_or_clear_bits(set, header->bitmap, start_byte_index * 8 + start_bit_index, qty);
        } else {
            int index = first_fit(header->bitmap, BITMAP_SIZE, action);
            if (index != -1) {
                printf("Allocated %d units starting at index %d.\n", action, index);
                printf("Updated Bitmap: ");
                print_bitmap(header->bitmap, BITMAP_SIZE);
            } else {
                printf("Failed to allocate %d units.\n", action);
            }
        }
    }
    return 0;
}
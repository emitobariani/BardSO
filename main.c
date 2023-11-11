#include "my_malloc_manager.h"

// Funcion para obtener la entrada del usuario
int get_user_input(char *prompt, int *input) {
    printf("%s", prompt);
    if(scanf("%d", input) != 1) {
        printf("Invalid input.\n");
        return 0;
    }
    return 1;
}

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
        int choice;
        if (!get_user_input("Enter your choice (1 to set, 2 to clear or 0 to exit) ", &choice)) {
            continue;
        }
        // Si el usuario introduce 0, salimos del bucle y terminamos el programa
        if (choice == 0) {
            break;
        }
        int start_byte_index;
        int start_bit_index;
        int qty;

        if (choice == 1){
            if (!get_user_input("Enter the number of bits to occupy: ", &qty)) {
                continue;
            }
            first_fit(header->bitmap, header->bitmap_size, qty);
            // set_or_clear_bits(1, header->bitmap, start_byte_index, start_bit_index, qty);
        } else if (choice == 2){
            if (!get_user_input("Enter the number of bits to clear: ", &qty) || 
                !get_user_input("Enter in which byte start to clear: ", &start_byte_index) || 
                !get_user_input("Enter the position of the bit to start clear: ", &start_bit_index)) {
                continue;
            }
            set_or_clear_bits(0, header->bitmap, start_byte_index, start_bit_index, qty);
        }
        // Imprimimos el estado actual del mapa de bits después de cada operación de asignación
        printf("Updated Bitmap: ");
        print_bitmap(header->bitmap, BITMAP_SIZE);
    }
    return 0;
}
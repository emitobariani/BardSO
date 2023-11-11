CC = gcc
CFLAGS = -Wall -Werror -g

all: my_malloc

my_malloc: my_malloc_manager.o my_malloc_impl.o
    $(CC) $(CFLAGS) -o my_malloc my_malloc_manager.o my_malloc_impl.o

my_malloc_manager.o: my_malloc_manager.c my_malloc_manager.h
    $(CC) $(CFLAGS) -c my_malloc_manager.c

my_malloc_impl.o: my_malloc_impl.c my_malloc_manager.h
    $(CC) $(CFLAGS) -c my_malloc_impl.c

clean:
    rm -f my_malloc my_malloc_manager.o my_malloc_impl.o

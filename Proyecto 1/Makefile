CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LDFLAGS = -lrt
OBJS = shared_memory.o
SRCS = creador.c cliente.c
EXECUTABLES = creador cliente

all: $(EXECUTABLES)

creador: creador.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

cliente: cliente.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)


clean:
	rm -rf $(EXECUTABLES)

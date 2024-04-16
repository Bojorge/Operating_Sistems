CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LDFLAGS = -lrt
OBJS = shared_memory.o
SRCS = creador.c cliente.c
EXECUTABLES = creador cliente

all: $(EXECUTABLES)

creador: creador.c $(OBJS)   # Agregado shared_memory.o como una dependencia
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

cliente: cliente.c $(OBJS)   # Agregado shared_memory.o como una dependencia
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(EXECUTABLES)

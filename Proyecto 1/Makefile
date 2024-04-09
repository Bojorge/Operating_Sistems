CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LDFLAGS = -lrt

all: creador cliente reconstructor estadisticas

creador: creador.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

cliente: cliente.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

reconstructor: reconstructor.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

estadisticas: estadisticas.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f creador cliente reconstructor estadisticas

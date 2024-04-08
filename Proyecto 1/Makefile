CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LDFLAGS = -lrt

all: sm_create sm_read sm_write

sm_create: sm_create.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

sm_read: sm_read.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

sm_write: sm_write.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f sm_create sm_read sm_write

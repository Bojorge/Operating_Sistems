// shared_memory.h


#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#define MEMORY_OBJECT_NAME "/sharedProcessMemory"
#define MAX_CHARS 100

typedef struct {
    //char *buffer;
    char buffer[MAX_CHARS];
    int bufferSize;
    int writeIndex; // Head
    int readIndex;  // Tail
    int memUsed;    // Count
} SharedMemory;


void initializeCircularBuffer(int numChars, size_t sharedSize);
     
void write_buf(char buf[], size_t size);

int getBufferSize();


#endif



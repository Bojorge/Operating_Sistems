// shared_memory.h


#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#define MEMORY_OBJECT_NAME "/sharedProcessMemory"

typedef struct {
    char *buffer;
    //char buffer[100];
    size_t bufferSize;
    int writeIndex; // Head
    int readIndex;  // Tail
    int memUsed;    // Count
} SharedMemory;


void initializeCircularBuffer(int numChars, size_t sharedSize);
     
void write_buf(char buf[], size_t size);

size_t getBufferSize();


#endif



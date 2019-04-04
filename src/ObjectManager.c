#define MEMORY_SIZE 1024*512
#define NULL_REF 0
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef unsigned long Ref;
typedef struct MEMBLOCK
{
    int numBytes;
    int startAddr;
    Ref ref;
    int count;
    MemBlock * next;
} MemBlock;


bool isInitiated = false;
int activePool = 0;


void initPool()
{
    if (!isInitiated)
    {
        
    }
    else
    {
        printf("destroy the pool and try again.\n");
    }
    
}

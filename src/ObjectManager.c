#define MEMORY_SIZE 1024*512
#define NULL_REF 0
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

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
bool currentPool = false;

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

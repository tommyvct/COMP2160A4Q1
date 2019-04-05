#define MEMORY_SIZE 1024 * 512
#define NULL_REF 0
#define INIT_CAPACITY 512
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

typedef unsigned long Ref;
typedef struct MEMBLOCK
{
    int blockSize;
    int offset;
    Ref id;
    int count;
    struct MEMBLOCK *next;
} MemBlock;


bool isInitiated = false;
Ref nextID;
int nextFreeAddr;
int currentPool;
int numMemBlock;
unsigned char pool[2][MEMORY_SIZE];
MemBlock * memBlockStart;
MemBlock * memBlockEnd;


void initPool()
{
    if (!isInitiated)
    {
        currentPool = 0;
        nextID = 1;
        nextFreeAddr = 0;
        numMemBlock = 0;
        memBlockStart = NULL;
        memBlockEnd = NULL;
        isInitiated = true;
    }
    else
    {
        printf("destroy the pool and try again.\n");
    }
}

Ref insertObject(const int size)
{
    if (!isInitiated)  // not initialised
    {
        return NULL_REF;
    }

    if (isFit(size))  // need GC
    {
        compact();
        if (!isFit) // full
        {
            return NULL_REF;
        }
    }

    MemBlock * newMemBlock = malloc(sizeof(MemBlock));
    
    assert(newMemBlock);

    if (memBlockStart == NULL)
    {
        memBlockStart = newMemBlock;
        
    }
    else
    {
        MemBlock * insertAfter = memBlockStart;
        
        for (int i = 0; i < numMemBlock; i++)
        {
            insertAfter = insertAfter -> next;
        }

        assert(!insertAfter -> next);

        insertAfter -> next = newMemBlock;
    }

    newMemBlock -> blockSize = size;
    newMemBlock -> offset = nextFreeAddr;
    newMemBlock -> id = nextID++;
    newMemBlock -> count = 1;
    newMemBlock -> next = NULL;

    memBlockEnd = newMemBlock;
    nextFreeAddr += size;
    numMemBlock++;

    return newMemBlock -> id;
}

void * retrieveObject( const Ref id )
{
    if (!isInitiated)  // not initialised
    {
        return NULL;
    }

    MemBlock * toRetrieve = memBlockStart;

    for (int i = 0; i < numMemBlock; i++)
    {
        if (toRetrieve -> id == id)
        {
            return pool[currentPool] + toRetrieve -> offset;
        }

        toRetrieve -> next = toRetrieve;
    }
    
    return NULL;
}

void addReference( const Ref id )
{
    if (!isInitiated)  // not initialised
    {
        return;
    }

    MemBlock * toAdd = memBlockStart;

    for (int i = 0; i < numMemBlock; i++)
    {
        if (toAdd -> id == id)
        {
            toAdd -> count++;
            return;
        }

        toAdd -> next = toAdd;
    }
}

void dropReference( const Ref id )
{
    if (!isInitiated)  // not initialised
    {
        return;
    }

    MemBlock * toDrop = memBlockStart;

    for (int i = 0; i < numMemBlock; i++)
    {
        if (toDrop -> id == id)
        {
            toDrop -> count--;
            return;
        }

        toDrop -> next = toDrop;
    }
    if (toDrop -> count <= 0)
    {
        cleanRef(toDrop -> id);
    }
}

void destroyPool()
{
    MemBlock * nextToDestroy = memBlockStart;
    MemBlock * toDestroy;

    while (nextToDestroy != NULL)
    {
        toDestroy = nextToDestroy;
        nextToDestroy = nextToDestroy -> next;
        free(toDestroy);
    }

    isInitiated = false;
    nextID = -1;
    nextFreeAddr = -1;
    currentPool = -1;
    numMemBlock = -1;
    memBlockStart = NULL;
    memBlockEnd = NULL;    
}

void dumpPool()
{
    MemBlock * toDump = memBlockStart;

    for(int i = 0; i < numMemBlock; i++)
    {
        printf
        (
            "Reference ID: %lu, starting addr: %p, size: %d\n", 
            toDump -> id, 
            pool[currentPool] + toDump -> offset,
            toDump -> blockSize
        );

        toDump = toDump -> next;
    }
}

static void cleanRef(const Ref id)
{
    MemBlock * before = memBlockStart;  // backtrack
    MemBlock * toDelete;  // actually check for invalid count

    if (memBlockStart -> id == id)
    {
        memBlockStart = memBlockStart -> next;
        free(before);
        numMemBlock--;
        return;
    }

    for (int i = 0; i < numMemBlock - 1; i++)
    {
        if (before -> next -> id == id)  // found it
        {
            toDelete = before -> next;
            before -> next = toDelete -> next;
            if (toDelete == memBlockEnd)  // if deleting last one
            {
                memBlockEnd = before;
            }
            free(toDelete);
            numMemBlock--;
            return;
        }
        else 
        {
            before = before -> next;
        }
    }
}

static void compact()
{
    nextFreeAddr = 0;
    MemBlock * toCompact = memBlockStart;
    int switchTo;
    if (!currentPool)  // 1
    {
        switchTo = 0;
    }
    else               // 0
    {
        switchTo = 1;
    }
    
    for(int i = 0; i < numMemBlock; i++)
    {
        assert(toCompact);

        void * dest = pool[switchTo] + nextFreeAddr;
        void * src = pool[currentPool] + toCompact -> offset;
        int cpsize = toCompact -> blockSize;
        memcpy(dest, src, cpsize);
        toCompact -> offset = nextFreeAddr;
        nextFreeAddr += toCompact -> blockSize;

        toCompact = toCompact -> next;
    }
    
    currentPool = switchTo;
}

static bool isFit(int size)
{
    if (MEMORY_SIZE - nextFreeAddr < size)
    {
        return false;
    }
    else
    {
        return true;
    }
}
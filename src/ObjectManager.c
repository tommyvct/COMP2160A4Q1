//--------------------------------------------------------------
// Name: Shang Wu
// Student Number: 7852291
// Course: COMP2160, Section A01
// Instructor: Mehdi Niknam
// Assignment 4
// 
// Implementation of ObjectManager.h
// Implemented a simple object manager and garbage collector
//--------------------------------------------------------------

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



// initiation indicator
static bool isInitiated = false;
// next available reference
static Ref nextID;
// next available address
static int nextFreeAddr;
// Pool indicator, possible values are 0 or 1
static int currentPool;
// counter for MemBlocks
static int numMemBlock;
// Pool
static unsigned char pool[2][MEMORY_SIZE];
// MemBlock LinkedList start
static MemBlock * memBlockStart;
// MemBlock LinkedList end
static MemBlock * memBlockEnd;



// completely remove a reference
// from MemBlock LinkedList
static void cleanRef(const Ref id)
{
    MemBlock * before = memBlockStart;  // backtrack
    MemBlock * toDelete;   // the actual MemBlock to be checked and deleted

    // Drawback: it skips the first item
    // check the first item(index 0) first
    if (memBlockStart -> id == id)
    {
        memBlockStart = memBlockStart -> next;
        free(before);
        numMemBlock--;
        return;
    }

    // for every MemBlock with index 1 to last
    for (int i = 0; i < numMemBlock - 1; i++)
    {
        if (before -> next -> id == id)  // found it
        {
            toDelete = before -> next;
            before -> next = toDelete -> next;  // skip it
            if (toDelete == memBlockEnd)  // if deleting last one
            {
                memBlockEnd = before;
            }
            free(toDelete);
            numMemBlock--;
            return;
        }
        else          // not found, skip it
        {
            before = before -> next;
        }
    }
}

// compact the memory, swap buffer
static void compact()
{
    nextFreeAddr = 0;
    MemBlock * toCompact = memBlockStart;
    int switchTo;
    // swap pool
    if (currentPool == 1)  // 1
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
        // parameters of memcpy()
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

// true if remaining memory at the end is 
// greater or equal than size
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

    if (!isFit(size))  // need GC
    {
        compact();
        if (!isFit(size)) // full
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
        
        for (int i = 0; i < numMemBlock - 1; i++)
        {
            insertAfter = insertAfter -> next;
        }

        // assert(!insertAfter -> next);

        insertAfter -> next = newMemBlock;  //ok
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

        toRetrieve = toRetrieve -> next;
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

        toAdd = toAdd -> next;
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
            break;
        }

        toDrop = toDrop -> next;
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

    // reset everything
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
    MemBlock * toDump;
    toDump = memBlockStart;

    assert(toDump);

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
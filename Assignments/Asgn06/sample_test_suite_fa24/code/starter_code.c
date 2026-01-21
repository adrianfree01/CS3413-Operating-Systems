#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FIRST_FIT 1
#define WORST_FIT 2
#define BEST_FIT 3

int algorithm;
int memSize;
int totalAllocated = 0;
int totalMemAllocated = 0;
int totalFailed = 0;
int totalTerminated = 0;
int totalFreedMemory = 0;

struct memoryChunk 
{
    int start;
    int size;
};

struct process 
{
    int processId;
    int start;
    int size;
};

struct process *processTable = NULL;
int processTableSize = 0;
int processTableCapacity = 0;

struct memoryChunk *freeList = NULL;
int freeListSize = 0;
int freeListCapacity = 0;

int compareChunks(const void *a, const void *b) 
{
    struct memoryChunk *chunkA = (struct memoryChunk *)a;
    struct memoryChunk *chunkB = (struct memoryChunk *)b;
    return chunkA->start - chunkB->start;
}

void mergeChunks() 
{
    if (freeListSize < 2) return;  

    qsort(freeList, freeListSize, sizeof(struct memoryChunk), compareChunks);
    int i = 0;

    while (i < freeListSize - 1) {
        if (freeList[i].start + freeList[i].size == freeList[i + 1].start) 
        {
            freeList[i].size += freeList[i + 1].size;
            for (int j = i + 1; j < freeListSize - 1; j++) 
                freeList[j] = freeList[j + 1];
            freeListSize--;
        } 
        else 
            i++;
    }
}

int doFree(int processId) 
{
    int processIndex = -1;
    for (int i = 0; i < processTableSize; i++) 
    {
        if (processTable[i].processId == processId) 
        {
            processIndex = i;
            break;
        }
    }

    if (processIndex == -1)
    {
        printf("Process %d failed to free memory\n", processId);
        totalFailed++;
        return 0;
    }

    int startAddress = processTable[processIndex].start;
    int chunkSize = processTable[processIndex].size;

    for (int i = processIndex; i < processTableSize - 1; i++)
        processTable[i] = processTable[i + 1];
    processTableSize--;

    if (freeListSize == freeListCapacity) {
        freeListCapacity = (freeListCapacity == 0) ? 1 : freeListCapacity * 2;
        freeList = realloc(freeList, freeListCapacity * sizeof(struct memoryChunk));
        if (!freeList) 
        {
            printf("Failed to allocate memory.\n");
            exit(1);
        }
    }

    freeList[freeListSize].start = startAddress;
    freeList[freeListSize].size = chunkSize;
    freeListSize++;

    totalTerminated++;
    totalFreedMemory += chunkSize;

    mergeChunks();

    return 1;
}

int doAllocate(int howMuchToAllocate, int processId) 
{
    int selectedIndex = -1;

    switch (algorithm) {
        case FIRST_FIT:
            for (int i = 0; i < freeListSize; i++) 
            {
                if (freeList[i].size >= howMuchToAllocate) 
                {
                    selectedIndex = i;
                    break;
                }
            }
            break;

        case WORST_FIT:
            for (int i = 0; i < freeListSize; i++) 
            {
                if (freeList[i].size >= howMuchToAllocate) 
                {
                    if (selectedIndex == -1 || freeList[i].size > freeList[selectedIndex].size)
                        selectedIndex = i;
                }
            }
            break;

        case BEST_FIT:
            for (int i = 0; i < freeListSize; i++) 
            {
                if (freeList[i].size >= howMuchToAllocate) 
                {
                    if (selectedIndex == -1 || freeList[i].size < freeList[selectedIndex].size)
                        selectedIndex = i;
                }
            }
            break;

        default:
            printf("There was an error, the algorithm is uninitialized");
            exit(0);
    }

    if (selectedIndex == -1) 
    {
        printf("Process %d failed to allocate %d memory\n", processId, howMuchToAllocate);
        totalFailed++;
        return 0;
    }

    struct memoryChunk *selectedChunk = &freeList[selectedIndex];
    int allocatedStart = selectedChunk->start;

    if (howMuchToAllocate < selectedChunk->size) 
    {
        selectedChunk->start += howMuchToAllocate;
        selectedChunk->size -= howMuchToAllocate;
    } 
    else if (howMuchToAllocate == selectedChunk->size) 
    {
        for (int i = selectedIndex; i < freeListSize - 1; i++) 
            freeList[i] = freeList[i + 1];
        freeListSize--;
    }

    if (processTableSize == processTableCapacity) 
    {
        processTableCapacity = (processTableCapacity == 0) ? 1 : processTableCapacity * 2;
        processTable = realloc(processTable, processTableCapacity * sizeof(struct process));
        if (!processTable) 
        {
            printf("Failed to allocate memory for process table.\n");
            exit(1);
        }
    }

    processTable[processTableSize].processId = processId;
    processTable[processTableSize].start = allocatedStart;
    processTable[processTableSize].size = howMuchToAllocate;
    processTableSize++;

    totalAllocated++;
    totalMemAllocated += howMuchToAllocate;

    return 1;
}

int calcFinalMemory() 
{
    int totalFreeMemory = 0;
    for (int i = 0; i < freeListSize; i++)
        totalFreeMemory += freeList[i].size;
    return totalFreeMemory;
}

int getNumberOfChunks() 
{
    return freeListSize;
}

int getSmallest() 
{
    if (freeListSize == 0) return 0;

    int smallest = freeList[0].size;
    for (int i = 1; i < freeListSize; i++) 
    {
        if (freeList[i].size < smallest) 
            smallest = freeList[i].size;
    }
    return smallest;
}

int getBiggest() 
{
    if (freeListSize == 0) return 0;

    int largest = freeList[0].size;
    for (int i = 1; i < freeListSize; i++) 
    {
        if (freeList[i].size > largest)
            largest = freeList[i].size;
    }
    return largest;
}

int main(int argc, char **argv) 
{
    for (int i = 0; i < argc; i++) 
    {
        if (strcmp(argv[i], "-b") == 0)
            algorithm = BEST_FIT;
        else if (strcmp(argv[i], "-w") == 0)
            algorithm = WORST_FIT;
        else if (strcmp(argv[i], "-s") == 0)
            memSize = atoi(argv[i + 1]);
        else if (strcmp(argv[i], "-f") == 0)
            algorithm = FIRST_FIT;
    }

    if (memSize <= 0) 
    {
        printf("The program requires size\n");
        exit(0);
    }

    freeListCapacity = 1;
    freeListSize = 1;
    freeList = malloc(freeListCapacity * sizeof(struct memoryChunk));
    freeList[0].start = 0;
    freeList[0].size = memSize;

    char operation;
    int id = 1337;
    int size;
    while (EOF != scanf("%c", &operation)) 
    {
        switch (operation) 
        {
            case 'N':
                scanf(" %d %d\n", &id, &size);
                doAllocate(size, id);
                break;

            case 'T':
                scanf(" %d\n", &id);
                doFree(id);
                break;

            case 'S':
                printf("Total Processes created %d, Total allocated memory %d, Total Processes\nterminated %d, Total freed memory %d, Final memory available %d, Final\nsmallest and largest fragmented memory sizes %d and %d, total failed requests:%d, number of memory chunks: %d\n",
                    totalAllocated, totalMemAllocated, totalTerminated, totalFreedMemory, calcFinalMemory(), getSmallest(), getBiggest(), totalFailed, getNumberOfChunks());
                break;
        }
    }
    return 0;
}

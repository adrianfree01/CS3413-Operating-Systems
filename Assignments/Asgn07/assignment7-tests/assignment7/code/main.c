#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#define PAGE_DIR_SIZE 1024       // 2^10 entries
#define PAGE_TABLE_SIZE 2048     // 2^11 entries
#define FRAME_SIZE 2048          // 2 KB
#define OFFSET_BITS 11
#define PT_INDEX_BITS 11
#define PD_INDEX_BITS 10
#define ADDRESS_SIZE 32

#define PAGETABLEENTRYSIZE 4     // Assuming 4 bytes per page table entry

typedef struct PageTableEntry {
    int frameNumber;
    int valid;
    int dirty;
    unsigned long timestamp;
} PageTableEntry;

typedef struct PageTable {
    PageTableEntry entries[PAGE_TABLE_SIZE];
} PageTable;

typedef struct PageDirectoryEntry {
    PageTable *pageTable;
    int valid;
} PageDirectoryEntry;

typedef struct Frame {
    uint32_t pageDirIndex;
    uint32_t pageTableIndex;
    int occupied;
} Frame;

PageDirectoryEntry pageDirectory[PAGE_DIR_SIZE];
Frame *frameTable;
int totalFrames;
unsigned long globalTimestamp = 0;
int pageFaults = 0;
int pageHits = 0;
int pagesSwappedOut = 0;
int framesUsed = 0;
unsigned long pagingStructureMemory = 0;
unsigned long swappedPagesMemory = 0;

void initializePageDirectory();
PageTable *initializePageTable();
int getFrame();
void updateLRU(uint32_t pdIndex, uint32_t ptIndex);
void handlePageFault(uint32_t pdIndex, uint32_t ptIndex, int isWrite);
void accessMemory(char operation, uint32_t logicalAddress);
uint32_t translateAddress(int frameNumber, uint32_t offset);
void printStatistics();

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage:%s <number_of_frames>\n", argv[0]);
        return 1;
    }

    totalFrames = atoi(argv[1]);
    frameTable = (Frame *)malloc(sizeof(Frame) * totalFrames);
    if (frameTable == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for frame table.\n");
        return 1;
    }
    memset(frameTable, 0, sizeof(Frame) * totalFrames);

    initializePageDirectory();

    char operation;
    uint32_t logicalAddress;

    printf("Logical addresses -> Physical addresses:\n");

    while (scanf(" %c %u", &operation, &logicalAddress) != EOF)
    {
        accessMemory(operation, logicalAddress);
    }

    printStatistics();

    for (int i = 0; i < PAGE_DIR_SIZE; i++)
    {
        if (pageDirectory[i].valid)
        {
            free(pageDirectory[i].pageTable);
        }
    }
    free(frameTable);

    return 0;
}

void initializePageDirectory()
{
    memset(pageDirectory, 0, sizeof(pageDirectory));
    pagingStructureMemory += PAGE_DIR_SIZE * PAGETABLEENTRYSIZE;
}

PageTable *initializePageTable()
{
    PageTable *pageTable = (PageTable *)malloc(sizeof(PageTable));
    if (pageTable == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for page table.\n");
        exit(1);
    }
    memset(pageTable, 0, sizeof(PageTable));
    pagingStructureMemory += PAGE_TABLE_SIZE * PAGETABLEENTRYSIZE;
    return pageTable;
}

int getFrame()
{
    if (framesUsed < totalFrames)
    {
        return framesUsed++;
    }
    else
    {
        unsigned long minTimestamp = ULONG_MAX;
        int victimFrame = -1;
        for (int i = 0; i < totalFrames; i++)
        {
            uint32_t pdIndex = frameTable[i].pageDirIndex;
            uint32_t ptIndex = frameTable[i].pageTableIndex;
            PageTableEntry *pte = &pageDirectory[pdIndex].pageTable->entries[ptIndex];
            if (pte->timestamp < minTimestamp)
            {
                minTimestamp = pte->timestamp;
                victimFrame = i;
            }
        }
        return victimFrame;
    }
}

void updateLRU(uint32_t pdIndex, uint32_t ptIndex)
{
    globalTimestamp++;
    pageDirectory[pdIndex].pageTable->entries[ptIndex].timestamp = globalTimestamp;
}

void handlePageFault(uint32_t pdIndex, uint32_t ptIndex, int isWrite)
{
    pageFaults++;
    int frameNumber = getFrame();

    if (frameTable[frameNumber].occupied)
    {
        uint32_t oldPdIndex = frameTable[frameNumber].pageDirIndex;
        uint32_t oldPtIndex = frameTable[frameNumber].pageTableIndex;
        PageTableEntry *oldPte = &pageDirectory[oldPdIndex].pageTable->entries[oldPtIndex];

        if (oldPte->dirty)
        {
            pagesSwappedOut++;
            swappedPagesMemory += FRAME_SIZE;
        }

        oldPte->valid = 0;
    }

    frameTable[frameNumber].pageDirIndex = pdIndex;
    frameTable[frameNumber].pageTableIndex = ptIndex;
    frameTable[frameNumber].occupied = 1;

    PageTableEntry *pte = &pageDirectory[pdIndex].pageTable->entries[ptIndex];
    pte->frameNumber = frameNumber;
    pte->valid = 1;
    pte->dirty = isWrite;
    updateLRU(pdIndex, ptIndex);
}

void accessMemory(char operation, uint32_t logicalAddress)
{
    uint32_t pdIndex = (logicalAddress >> (PT_INDEX_BITS + OFFSET_BITS)) & 0x3FF; // 10 bits
    uint32_t ptIndex = (logicalAddress >> OFFSET_BITS) & 0x7FF;                   // 11 bits
    uint32_t offset = logicalAddress & 0x7FF;                                     // 11 bits

    if (!pageDirectory[pdIndex].valid)
    {
        pageDirectory[pdIndex].pageTable = initializePageTable();
        pageDirectory[pdIndex].valid = 1;
    }

    PageTable *pageTable = pageDirectory[pdIndex].pageTable;
    PageTableEntry *pte = &pageTable->entries[ptIndex];

    if (pte->valid)
    {
        pageHits++;
        if (operation == 'w')
        {
            pte->dirty = 1;
        }
        updateLRU(pdIndex, ptIndex);
    }
    else
    {
        int isWrite = (operation == 'w') ? 1 : 0;
        handlePageFault(pdIndex, ptIndex, isWrite);
    }

    int frameNumber = pageTable->entries[ptIndex].frameNumber;
    uint32_t physicalAddress = translateAddress(frameNumber, offset);

    printf("%u -> %u\n", logicalAddress, physicalAddress);
}

uint32_t translateAddress(int frameNumber, uint32_t offset)
{
    return (frameNumber * FRAME_SIZE) + offset;
}

void printStatistics()
{
    unsigned long processMemory = framesUsed * FRAME_SIZE;
    unsigned long totalMemory = pagingStructureMemory + processMemory + swappedPagesMemory;
    double usefulMemoryPercentage = ((double)(processMemory + swappedPagesMemory) / totalMemory) * 100.0;

    printf("\nStats:\n");
    printf("major page faults = %d\n", pageFaults);
    printf("page hits = %d\n", pageHits);
    printf("pages swapped out = %d\n", pagesSwappedOut);
    printf("Memory used for paging structure %lu, memory used by the process pages %lu, percentage of useful memory w.r.t the total memory occupied by paging structures and data %.2f\n",
           pagingStructureMemory, processMemory + swappedPagesMemory, usefulMemoryPercentage);
}

#include <stdio.h>
#include <stdlib.h>
#define OFFSETBITS 12
// fancy way of computing 2^OFFSETBITS 
#define PAGESIZE (1 << OFFSETBITS)
#define WORDSIZE 32
int getPageNumber(int logicalAddress){
    return logicalAddress / PAGESIZE ; 
}

int getPageOffset(int logicalAddress){
    return logicalAddress % PAGESIZE ;
}
int getStartingAddressOfTheFrameByIndex(int indexInFramesArray){
    return indexInFramesArray*PAGESIZE ;
}

int main(int argc, char** argv){
    /* int* frames =  malloc the recommended array here*/
    int numFrames = 1 << (WORDSIZE - OFFSETBITS);
    int* frames = (int*)malloc(numFrames * sizeof(int)); 
    for (int i = 0; i < numFrames; i++)
        frames[i] = -1;

    int maxPreviouslyUsedFrame = 0;
    int logicalAddress;
    int i;
    int hasAFrameAssignedBefore = 0;
    int tempFrameIndex=0;
    while(EOF != scanf("%d\n",&logicalAddress) )
    {
        hasAFrameAssignedBefore = 0;
        int pageNumber = getPageNumber(logicalAddress);
        int pageOffset = getPageOffset(logicalAddress);

        for (i = 0; i < maxPreviouslyUsedFrame; i++) 
        {
            if (frames[i] == pageNumber) 
            {
                hasAFrameAssignedBefore = 1;
                tempFrameIndex = i;
                break;
            }
        }

        if (!hasAFrameAssignedBefore) 
        {
            if (maxPreviouslyUsedFrame < numFrames) 
            {
                frames[maxPreviouslyUsedFrame] = pageNumber;
                tempFrameIndex = maxPreviouslyUsedFrame;
                maxPreviouslyUsedFrame++;
            } 
            else 
            {
                fprintf(stderr, "Error: Out of frames!\n");
                free(frames);
                return 1;
            }
        }

        int frameStartAddress = getStartingAddressOfTheFrameByIndex(tempFrameIndex);
        int physicalAddress = frameStartAddress + pageOffset;

        printf("Physical address is %i, logical address is %i\n", physicalAddress, logicalAddress);
        printf("Read %i\n\n",logicalAddress);
    }
    free(frames);
    return 0;
}

/* 
* PART ONE
*   1. How many values can one 2-base digit take? List all of them.
*       One 2-base digit can take 2 values, 0 and 1
*
*   2. How many values can one 10-base digit take? List all of them.
*       One 10-base digit can take 10 values, 0, 1, 2, 3, 4, 5, 6, 7, 8, and 9
*
*   3. How many values can 7 bits represent? DO NOT list all of them.
*       7 bits can represent 128 different values
*
*   4. Recall, look up or come up with a formula for a generic case: how many values can n m-valued
*       bits represent? 
*           n m-valued bits can represent m^n values
*       
*   5. If our machine is byte-addressable (i.e., each byte must have its own address), how many
*       addresses do we need to have a table with pointers to every byte in 1 Megabyte?
*          There are 2^20 bytes in a megabyte, so 2^20 addresses are needed to have one address for each byte.
*
*   6. What if our machine was bit-addressable?
*           There are 2^20 bytes in a megabyte, 8 bits in a byte, so 2^3 * 2^20 or 2^23 bits total
*               Therefore we need 2^23 addresses to have one address per bit.
*   7. Consider you are using an array of pointers, each entry of which is sixty-four bits (or 8 bytes) to
*       have a pointer to each individual byte from the question 5, how much memory would this array
*       occupy?
*           This would occupy 2^23 bytes of memory, or 8MB.
*
*   8. Consider you are using an array of pointers, each entry of which is sixty-four bits (or 8 bytes) to
*       have a pointer to each individual bit from the question 6, how much memory would this array
*       occupy?
*           This would occupy 2^23 * 2^3 bytes (2^26 bytes), or 64MB of memory.
*
*
* PART TWO
*   1. Write a short C program that would translate logical addresses (in a 32-bit machine) 
*       into physical addresses with 12-bit page offset and 20-bit page number. If the
*       page was not loaded into the physical memory, you are to use the next free frame, however you
*       must reuse the pages that already have frames assigned to them before. For this lab, consider
*       you have all 2^(WORDSIZE-OFFSETBITS) frames available. Refer to the starter code provided
*       with the lab.
*
*   2. How many megabytes are required to store 2^20 integers?
*       An int is 4 bytes, so 2^20 * 2^2 = 2^22 bytes, or 4 MB.
*
*
*/
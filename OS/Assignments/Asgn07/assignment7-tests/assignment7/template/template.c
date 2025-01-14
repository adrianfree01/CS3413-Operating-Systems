#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define ARCH 32
#define PAGETABLEENTRYSIZE 8
#define PAGESIZE (1 << OFFSETBITS)
#define OFFSETBITS 11
#define PAGEDIRBITS 10
#define PAGETABLEBITS (ARCH-OFFSETBITS-PAGEDIRBITS)
#define PAGEDIRSIZE (1 << (PAGETABLEBITS+OFFSETBITS))
int framesSize;
unsigned int pageFault=0;
unsigned int pageHits = 0;
unsigned int pagesSwapped = 0;
unsigned long page_struct_memory=0;
unsigned long physical_memory_occupied_by_data=0;

void printPhysicalAddress(int frameID, unsigned long logicalAddress){
 //   printf("%lu -> %i\n",logicalAddress,frameID*PAGESIZE+getPageOffsetAddress(logicalAddress));
}

unsigned int getPageDirAddress(unsigned long logicalAddress){
    return logicalAddress / (1 << (PAGETABLEBITS+OFFSETBITS)); 
}

unsigned int getPageTableAddress(unsigned long logicalAddress){
    int temp = logicalAddress - PAGEDIRSIZE*getPageDirAddress(logicalAddress);
    return temp/PAGESIZE; 
}

unsigned int getPageOffsetAddress(unsigned long logicalAddress){
    return logicalAddress % PAGESIZE;
}


int main(int argc, char** argv){
    framesSize = atoi(argv[1]);
    unsigned long logicalAddress;
    char operation;
    printf("Logical addresses -> Physical addresses:\n");
    while(EOF != scanf("%c %lu\n",&operation,&logicalAddress) )
    {
        if (operation == 'r')
        {
            // readFromAddress(logicalAddress);
        }
        else
        {
            // writeToAddress(logicalAddress);
        }
    }

    printf("\nStats:\nmajor page faults = %u\npage hits = %u\npages swapped out = %u\n",pageFault,pageHits,pagesSwapped);
    printf("Memory used for paging structure %lu, memory used by the process pages %lu, percentage of useful memory w.r.t the total memory occupied by paging structures and data %.2f\n",page_struct_memory,physical_memory_occupied_by_data,1.0f*physical_memory_occupied_by_data/(page_struct_memory+physical_memory_occupied_by_data)*100);
    return 0;
}
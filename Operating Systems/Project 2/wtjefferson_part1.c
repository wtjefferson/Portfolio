/*
NAME: William Jefferson
CWID: 11930076
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


const int TLB_SIZE = 16;
const int PT_SIZE = 256;
const int BUF_SIZE = 256;
const int PM_SIZE = 256;

struct TLB
{
    unsigned char TLBpage[16];
    unsigned char TLBframe[16];
    int ind;
};

int readFromDisk(int PN, char *PM, int *OF)
{
    char buffer[BUF_SIZE]; //Initialize a buffer char
    memset(buffer, 0, sizeof(buffer)); //Clear buffer

    //Open our backing store file
    FILE *BS;
    BS = fopen("BACKING_STORE.bin", "rb");
    //File fails to open
    if (BS == NULL)
    {
        printf("File failed to open\n");
        exit(0);
    }
    //Failure to search file
    if (fseek(BS, PN * PM_SIZE, SEEK_SET) != 0)
        printf("error in fseek\n");
    //Failure in reading file
    if (fread(buffer, sizeof(char), PM_SIZE, BS) == 0)
        printf("error in fread\n");
    /*Loop through and write to physical memory
    from buffer*/
    int i = 0;
    while (i < PM_SIZE) {
        *((PM + (*OF) * PM_SIZE) + i) = buffer[i];
        i++;
    }
    (*OF)++;
    return (*OF) - 1;
}

int findPage(int LA, char *PT, struct TLB *tlb, char *PM, int *OF, int *PF, int *TLBhits){
    unsigned char mask = 0xFF; //Mask for the rightmost 16 bits
    unsigned char offset; //Initialize offset
    unsigned char pageNum; //Initialize page number
    bool TLBhit = false; //Default TLBhit to false until triggered
    int frame = 0; //Default frame starts at zero
    int value;
    int newFrame = 0;

    //Print virtual address
    printf("Virtual adress: %d\t", LA);

    //Solve for page number using bit shift and mask
    pageNum = (LA >> 8) & mask;
    //Solve for offset using pointer to memory location of mask
    offset = LA & mask;

    // Check if in TLB
    int i = 0;
    while (i < TLB_SIZE && !TLBhit) {
        if (tlb->TLBpage[i] == pageNum) {
            frame = tlb->TLBframe[i];
            TLBhit = true;
            (*TLBhits)++;
        }
        i++;
    }

    // Check if in PageTable
    if (TLBhit == false)
    {
        if (PT[pageNum] != -1)
        {
        }

        // if not in either read from disk
        else
        {
            newFrame = readFromDisk(pageNum, PM, OF);
            PT[pageNum] = newFrame;
            (*PF)++;
        }
        //Update TLB with new entry
        frame = PT[pageNum];
        tlb->TLBpage[tlb->ind] = pageNum;
        tlb->TLBframe[tlb->ind] = PT[pageNum];
        tlb->ind = (tlb->ind + 1) % TLB_SIZE;
    }

    int index = ((unsigned char)frame * PM_SIZE) + offset;
    value = *(PM + index);
    printf("Physical address: %d\t Value: %d\n", index, value);

    return 0;
}

int main(int argc, char *argv[]){
    //Initializing variables
    int val; //Integer for reading file using address
    FILE *fd; //User's input file
    int openFrame = 0;
    float pageFaultRate; //Float for storing page fault rate
    float TLBHitRate; //Float for storing hit rate
    int pageFaults = 0; //Int for storing faults
    int TLBhits = 0; //Int for storing hits
    int inputCount = 0; //Int for counting logical addresses
    unsigned char PageTable[PT_SIZE]; //Initialize page table as unsigned char
    memset(PageTable, -1, sizeof(PageTable)); //Clear page table
    //Building the TLB in main using the struct
    struct TLB tlb; //Call struct
    memset(tlb.TLBpage, -1, sizeof(tlb.TLBpage)); //Clear pages
    memset(tlb.TLBframe, -1, sizeof(tlb.TLBframe)); //Clear frames
    tlb.ind = 0; //Zero out TLB index for input
    char PhyMem[PM_SIZE][PM_SIZE]; //Tracking physical memory with 2D char

    //Exit if user enters too few command line args
    if (argc < 2)
    {
        printf("Not enough arguments\nProgram Exiting\n");
        exit(0);
    }
    //Open user file for reading
    fd = fopen(argv[1], "r");
    if (fd == NULL)
    {
        printf("File failed to open\n");
        exit(0);
    }
    //Calculate the number of addresses input
    for (; fscanf(fd, "%d", &val) == 1; )
    {
    findPage(val, PageTable, &tlb, (char *)PhyMem, &openFrame, &pageFaults, &TLBhits);
    inputCount++;
    }

    //Report statistics and exit
    pageFaultRate = (float)pageFaults / (float)inputCount;
    TLBHitRate = (float)TLBhits / (float)inputCount;
    printf("Page Faults: %d\n", pageFaults);
    printf("Page Fault Rate = %.3f\nTLB Hits = %d\nTLB hit rate= %.3f\n", pageFaultRate, TLBhits, TLBHitRate);
    pclose(fd);
    return 0;
}


/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: b_io_helper.c
*
* Description: Helper functions for b_io.c
*
**************************************************************/

#include "b_io.h"
#include "parsepath.h"

// A helper funciton to search the next allocated block in file
int getNextblockOfFile(struct ExtentNode extents[NUM_OF_EXTENTS],int currentBlock){
    int nextBlock = -1;
    for(int i =0;i<NUM_OF_EXTENTS;i++){
        if(extents[i].blockNum != 0){
            // check if the current block is within the extent
            if((currentBlock >= extents[i].blockNum) && (currentBlock <= (extents[i].blockNum + extents[i].count -1)) ){
                // If yes, check there is next block in the extent
                if((currentBlock + 1) <= (extents[i].blockNum + extents[i].count -1)){
                    nextBlock = currentBlock + 1;
                    break;
                }
                // If not, check if there is block in next entent
                if(i < NUM_OF_EXTENTS){
                    if(extents[i+1].blockNum != 0){
                        nextBlock = extents[i+1].blockNum;
                    }
                }
            }
        }
    }
    return nextBlock;
}

// A helper function to merge two extents
int mergeExtents(struct ExtentNode originExtents[NUM_OF_EXTENTS],struct ExtentNode tempExtents[NUM_OF_EXTENTS]){
    // Will check and merge the contigous blocks from tempExtents into original extents where two
    // extentNode are adjacent
    for(int i =0;i< NUM_OF_EXTENTS;i++){
        int fileStartBlock = originExtents[i].blockNum;
        int fileCountBlock = originExtents[i].count;
        if(fileStartBlock != 0){
            for(int j =0;j< NUM_OF_EXTENTS;j++){
                if((fileStartBlock + fileCountBlock) == tempExtents[j].blockNum){
                    originExtents[i].count += tempExtents[j].count;
                    // Mark the extent has been merged to file's extent array
                    tempExtents[j].blockNum = -1;
                }
            }
        }
    }
    // Will copy the left over extents from tempExtents to the empty slot of file's extents
    for(int j =0;j< NUM_OF_EXTENTS;j++){
        if(tempExtents[j].blockNum > 0){
            for(int i =0;i< NUM_OF_EXTENTS;i++){
                if(originExtents[i].blockNum == 0){
                    originExtents[i].blockNum = tempExtents[j].blockNum;
                    originExtents[i].count = tempExtents[j].count;
                    break;
                }
            }
        }
    }
    return 0;
}

// A debug function to print the content in blocks of text file
int printExtent(struct ExtentNode extents[NUM_OF_EXTENTS]){

    for (size_t i = 0; i < NUM_OF_EXTENTS; i++)
    {
        if(extents[i].blockNum != 0){
            for (size_t j = extents[i].blockNum; j < (extents[i].blockNum + extents[i].count); j++)
            {
            char * temp = malloc(volumeControlBlock->sizeOfBlocks);
            LBAread(temp,1,j);
            printf("*****In block: %ld******\n[%s]\n******\n\n",j,temp);
            free(temp);
            }
        }
    }
}
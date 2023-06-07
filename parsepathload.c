/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: parsepathload.c
*
* Description: This is the .c file for the parse path function containing
*              the helper function to load the target directory.
**************************************************************/




#include "parsepath.h"



struct directoryEntry *loadDirectory(int index, struct directoryEntry * directory){


    // To malloc space for a directory.
    // Mallocing space for a directory will always be the same size
    struct directoryEntry *directoryEntryBuffer = malloc(directory[index].size);	

    /* A variable that is an offset from the beginning of the allocated space to allow
       for a proper read of the directory from disk to memory, as we don't want to read
       into the same position in the buffer */
    int offset = 0;



    // To loop through the extents table
    for(int i = 0; i < NUM_OF_EXTENTS; i++){

        /* If the extent has a startingBlockNumber of 0 then break.
        * A startingBlockNumber of 0 means you have reached the end
        * of extents. 
        */	
        if(directory[index].extents[i].blockNum == 0){
	
           break;
    
        }	
    
        // To read the total number of blocks given by extents table
        LBAread(directoryEntryBuffer + offset, directory[index].extents[i].count, directory[index].extents[i].blockNum);
    
        /* To update the offset to an appropriate number for the next LBAread as we
           don't want to read into the same position in the buffer */
        offset = offset + ((directory[index].extents[i].count) * volumeControlBlock->sizeOfBlocks);

    }
    
    return directoryEntryBuffer;

}
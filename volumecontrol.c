/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: volumecontrol.c
*
* Description: This is the .c file for volume control which will allow us 
*              to have control and access to the various parts of the volume and
*              where we initialize the volume control block.
*
**************************************************************/




#include "volumecontrol.h"


// Our volume control block (instance of VCB) to reference throughout the file system 
struct VCB *volumeControlBlock;







/* A function to allocate space for the volume control block and to initialize the 
   fields of the VCB */
void volumeControlInit(uint64_t numberOfBlocks, uint64_t blockSize){
    
    // The next 4 lines initialize our values in the VCB
	volumeControlBlock->sizeOfBlocks = blockSize;
	volumeControlBlock->numOfTotalBlocks = numberOfBlocks;
	volumeControlBlock->numOfFreeBlocks = volumeControlBlock->numOfTotalBlocks;
	volumeControlBlock->magicNumber = MAGIC_NUMBER;
	
}





/* A function to handle writing to disk (utilizing extents) a passed in buffer which 
   has the sole purpose of preventing code from being reused again and again*/
void extentsWrite(struct directoryEntry *buffer, int index){


	/* A variable that is an offset from the beginning of the buffer to allow
       for a proper write of the directory from memory to disk, as we don't want to write
       from the same position in the buffer */
    int offset = 0;



    // To loop through the extents table
    for(int i = 0; i < NUM_OF_EXTENTS; i++){

        /* If the extent has a startingBlockNumber of 0 then break.
        * A startingBlockNumber of 0 means you have reached the end
        * of extents. 
        */	
        if(buffer[index].extents[i].blockNum == 0){
	
           break;
    
        }	
    
        // To write the blocks given by the extents table
        LBAwrite(buffer + offset, buffer[index].extents[i].count, buffer[index].extents[i].blockNum);
    
        /* To update the number of bytes that have been previously written to an 
		   appropriate number for the next LBAwrite as we don't want to write from
		   the same position in the buffer */
         offset = offset + (buffer[index].extents[i].count * volumeControlBlock->sizeOfBlocks);

    }

}
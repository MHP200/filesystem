/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: fsInit.c
*
* Description: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
*
**************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>






#include "parsepath.h"



int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	
	

	
	
	/* To allocate a block of memory for our volume control block to bring it into
	   memory */
	volumeControlBlock = malloc(blockSize);


	// To bring the VCB into memory to check if we formatted the volume already
	LBAread(volumeControlBlock, 1, 0);

	/* To check if the magic numbers match. If not, format our volume */
	if(!(volumeControlBlock->magicNumber == MAGIC_NUMBER))
	{
		
	
		// To initialize the volume control block
		volumeControlInit(numberOfBlocks, blockSize);
		
		
		// To initialize the free space map
		freeSpaceInit();
		

		/* To set the parent directory pointer of the root directory to NULL since
		   the root directory does not have a parent directory */
		struct directoryEntry *parent = NULL;
		// To initialize the root directory
		rootDirectory = initializeDir(NUM_ENTRIES, parent);

		
		// To write to disk our initialized volume control block 
		LBAwrite(volumeControlBlock, 1, 0);
		
		
		/* To set the current directory to be the root directory so that we can
		   use parsepath() and perform our directory functions */
		currentDirectory = rootDirectory;
	
			
	} else {
		
		/* To allocate a block of memory for our free space map to bring it into
	   	memory */
		freeSpaceMap = malloc(volumeControlBlock->lengthOfFreeSpaceMap * blockSize);

		// Buffer to load the root directory
		rootDirectory = malloc(volumeControlBlock->lengthOfRootDirectory*blockSize);	
		
		
		
		// To bring the root directory into memory
		LBAread(rootDirectory, volumeControlBlock->lengthOfRootDirectory, 
		        volumeControlBlock->startingBlockOfRootDirectory);
		
		// To bring the free space map into memory
		LBAread(freeSpaceMap, volumeControlBlock->lengthOfFreeSpaceMap, 
		        volumeControlBlock->startingBlockOfFreeSpace);

		
		/* To set the current directory to be the root directory so that we can
		   use parsepath() and perform our directory functions */
		currentDirectory = rootDirectory;
		

	}
	return 0;
	}
	



void exitFileSystem ()
	{

	/* The next 3 lines free the memory allocated for the volume control
	   bock, the root directory, and the free space map, respectively  */
	free(volumeControlBlock);
	free(rootDirectory);
	free(freeSpaceMap);
	

	/*Free the memory allocated for helper structure of the fs_setcwd and 
	  fs_getcwd  */
	  
	//free(currentDirectory);
	free(absPathOfCurrentDir);


	printf ("System exiting\n");
	}
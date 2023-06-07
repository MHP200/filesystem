/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: directories.c
*
* Description: This is the .c file for our directory system in which we implement
*              and define the functions necessary to create and initialize the rrot
*              directory.
*
**************************************************************/





#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "directories.h"



struct directoryEntry * rootDirectory;
struct directoryEntryCalculations * directoryEntryCalcs;



/* Used for debugging purposes to print each element in array of directory entries
 * @param needs the total number of entries in root dir
 */
void debugPrint(int total)
{
	
	printf("\nElement = %d\n",0);
	printf("Filename: %s\n",rootDirectory[0].fileName);
	printf("Data Created: %ld\n",rootDirectory[0].dateCreated);
	printf("Date Modified: %ld\n",rootDirectory[0].dateModified);
	printf("Date Created in string: %s\n\n",ctime(&rootDirectory[0].dateCreated));

	printf("\nElement = %d\n",1);
	printf("Filename: %s\n",rootDirectory[1].fileName);
	printf("Data Created: %ld\n",rootDirectory[1].dateCreated);
	printf("Date Modified: %ld\n",rootDirectory[1].dateModified);
	printf("Date Created in string: %s\n\n",ctime(&rootDirectory[0].dateCreated));
	for(int i = 2; i < total; i++)
    {
		printf("\nElement = %d\n",i);
		printf("Filename: %s\n",rootDirectory[i].fileName);
		printf("Data Created: %ld\n",rootDirectory[i].dateCreated);
		printf("Date Modified: %ld\n",rootDirectory[i].dateModified);
		
		
    }
}




/* Calculates the total number of bytes needed for a directory
* @param totalEntries, the number of bytes needed for a dir
* @param directoryCalcs, a struct which stores the total bytes needed for a dir
*        along with other information for calculations  
*/
int numBytesForEntries(int numEntries, struct directoryEntryCalculations* directoryCalcs)
{
	// Total bytes needed for a dir is the number of entries * the size of each directory entry
	int numOfBytesForEntries = numEntries * sizeof(struct directoryEntry);
	
	// To store the number of bytes needed for the directory for future calculations
	directoryCalcs->totalBytesForEntries = numOfBytesForEntries;

    return numOfBytesForEntries;
}




/* Calculates the total number of blocks needed
* @param directoryCalcs, a struct which stores the total blocks needed for a directory
*/
int numBlocksToAskFreeSpace(struct directoryEntryCalculations* directoryCalcs)
{
	/* A variable to store the needed number of bytes for our desired number of entries 
	   so that we can eventually find the number of blocks to ask the free space */
	int m = directoryCalcs->totalBytesForEntries;

	/* A variable to store the size of each block in the partition so that we can
	   eventually find the number of blocks to ask the free space */
	int n = volumeControlBlock->sizeOfBlocks;

	/* A variable to store the number of blocks that we'll need to ask the free 
	   space for */
	int lengthOfDirectory = (m + (n - 1)) / n;

	/* To store the length of our directory in our directoryEntryCalculations struct
	   for future calculations */
    directoryCalcs->lengthOfDirectory = lengthOfDirectory;
	
	/* A variable that will return the number of blocks to ask the free space */
	int numBlocks = lengthOfDirectory;
    
	return numBlocks;
}




/* Calcuates how many extra entries can be added to the new directory
 * @param directoryCalcs, a struct which stores the total number of extra entries 
 */
int numExtraEntries(struct directoryEntryCalculations* directoryCalcs)
{
	/* To get the number of bytes left in the space allocated for the new 
	   directory so we could store more directory entries */
	int bytesRemaining = directoryCalcs->totalBytesForEntries % volumeControlBlock->sizeOfBlocks;
    
	/* To get the number of directory entries that we could fit in to the allocated 
	   space */
	int extraRoomForEntries = bytesRemaining / sizeof(struct directoryEntry);

	// To store the above variable in the struct containing data used for calculations
	directoryCalcs->totalExtraEntries = extraRoomForEntries;

    return extraRoomForEntries;
}




/* Sets each data field in the new directory to its proper value
 * @param directoryCalcs, a struct which stores the necessary variables
 */
void initializeDirFields(struct directoryEntryCalculations *calcs, struct directoryEntry *directory, struct directoryEntry *parentDirectory)
{


	// To initialize the directory entries after the "." and ".." entries
	for (int i = 2; i < calcs->totalNumOfEntries; i++) {

    	// To initialize the name of the file to an empty string
    	directory[i].fileName[0] = '\0';
    	// To set the size of the file to 0
		directory[i].size = 0;
		/* To set the number of directory entries to 0. If it becomes a directory,
		   the number of entries will be at least 52 eventually. Though for now,
		   since it's unused, it will be 0. If it becomes a file, the number of 
		   entries will always be 0 */
		directory[i].numOfEntries = 0;
    	// To set the date created and modified to 0
    	directory[i].dateCreated = (time_t)0l;
   		directory[i].dateModified = (time_t)0l;
		/* To set file type to directory. File type 0 = directory, 1 = file. All entries
		   are 0 by default, but the value may change once it's in a used state and
		   becomes a file */
		directory[i].fileType = 0;
		// To set the state of the entry as unused, or false.
		directory[i].used = false;
    	// To loop through the eight extent tables and set each to be in a free state
    	for (int j = 0; j < NUM_OF_EXTENTS; j++) {
        	directory[i].extents[j].blockNum = 0;
        	directory[i].extents[j].count = 0;
   		}

	}





	// To initialize first entry to '.'
    strcpy(directory[0].fileName,".");
	// To set the size of the directory to the amount of bytes in use
	directory[0].size = calcs->totalNumOfEntries * sizeof(directoryEntry);
	/* To set the number of entries in the directory (in our case, it's always 
	   going to be initialized to 52) */
	directory[0].numOfEntries = calcs->totalNumOfEntries;
	// To set dateCreated and dateModified to number of secs since January 1, 1979
	directory[0].dateCreated = time(&directory->dateCreated);
	directory[0].dateModified = time(&directory->dateModified);
	// To set file type to directory. File type 0 = directory, 1 = file 
	directory[0].fileType = 0;
	// To set the state of the entry as used, or true.
	directory[0].used = true;
	/* To first initialize all of the extent values to values of 0, since default
	   values are not always 0s but can be random int values */
	for (int j = 0; j < NUM_OF_EXTENTS; j++) {
        directory[0].extents[j].blockNum = 0;
        directory[0].extents[j].count = 0;
    }
	// To initialize the extents of the '.' entry to their appropriate values
	blockAllocator(calcs->lengthOfDirectory, directory[0].extents);




	// To initialize second entry to '..'
    strcpy(directory[1].fileName,"..");
	/* To check whether or not this directory being created is the root directory. 
	   It is the root directory if the parent directory is NULL */
	if (parentDirectory == NULL){

		/* To store the size of the "parent" directory, really this root directory, 
		   to the amount of bytes in use by this root directory */
		directory[1].size = directory[0].size;
		// To store the number of entries in this current (root) directory
		directory[1].numOfEntries = directory[0].numOfEntries;
		/* To store the dateCreated and dateModified of the root directory to 
		   number of secs since January 1, 1979 */
		directory[1].dateCreated = directory[0].dateCreated;
		directory[1].dateModified = directory[0].dateModified;
		// To set file type to directory. File type 0 = directory, 1 = file 
		directory[1].fileType = 0;
		// To set the state of the entry as used, or true.
		directory[1].used = true;
		/* To copy the extents of the '.' entry of this directory since the root
		   dir does not have a parent. Essentially, we are storing the LBA information
		   of this current (root) directory in here */
		for (int j = 0; j < NUM_OF_EXTENTS; j++) {
        	directory[1].extents[j].blockNum = directory[0].extents[j].blockNum;
        	directory[1].extents[j].count = directory[0].extents[j].count;
    	}

		/* The next two lines store into the VCB the starting block of the root 
		   directory as well as the length of the root directory, respectively */
		volumeControlBlock->startingBlockOfRootDirectory = directory[0].extents->blockNum;
		volumeControlBlock->lengthOfRootDirectory = directory[0].extents->count;
		
		

	// If the parent is not NULL, in other words, the current dir DOES have a parent
	} else {

		/* To store the size of, or, the number of bytes being used, by the parent
		   directory */
		directory[1].size = parentDirectory[0].size;
		// To store the number of entries contained in the parent directory
		directory[1].numOfEntries = parentDirectory[0].numOfEntries;
		// To store dateCreated and dateModified of the parent to number of secs since January 1,1979
		directory[1].dateCreated = parentDirectory[0].dateCreated;
		directory[1].dateModified = parentDirectory[0].dateModified;
		// To set file type to directory. File type 0 = directory, 1 = file 
		directory[1].fileType = 0;
		// To set the state of the entry as used, or true.
		directory[1].used = true;
		/* To copy the extents of the "." entry of the parent directory. Essentially, 
		   we are storing the LBA information of the parent directory here */
		for (int j = 0; j < NUM_OF_EXTENTS; j++) {
        	directory[1].extents[j].blockNum = parentDirectory[0].extents[j].blockNum;
        	directory[1].extents[j].count = parentDirectory[0].extents[j].count;
    	}

	}

}




/* Calls all functions outlined above to initialize the new directory
 * @param numEntries, number of entries requested for the new dir
 */
struct directoryEntry *initializeDir(int numOfEntries, struct directoryEntry * parentDirectory)
{

	/* To allocate space for the struct that we'll use to store data to use in
	   our calculations */
	directoryEntryCalcs = (struct directoryEntryCalculations *)malloc(sizeof(directoryEntryCalcs));
	
	// To get the total number of bytes needed for the new directory
	numBytesForEntries(numOfEntries, directoryEntryCalcs);

	// To calculate the number of blocks to ask the free space for
	numBlocksToAskFreeSpace(directoryEntryCalcs);

	// To calculate how many extra entries can be added to the new directory
	numExtraEntries(directoryEntryCalcs);

	// Total number of entries = extra entries + the number of entries requested
	int total = directoryEntryCalcs->totalExtraEntries + numOfEntries;

	/* To store the total number of entries we have in the blocks we want to 
	   allocate */
	directoryEntryCalcs->totalNumOfEntries = total;
	
	// To allocate the memory needed for a new directory
	struct directoryEntry *directory = malloc(directoryEntryCalcs->totalNumOfEntries * sizeof(directoryEntry));

	// To set each data field in the new directory to its proper value
	initializeDirFields(directoryEntryCalcs, directory, parentDirectory);

	// To write the new directory to disk (by accessing its extents) 
	extentsWrite(directory, 0);

	// To free the space allocated for the directoryEntryCalcs struct
	free(directoryEntryCalcs);

	return directory;
	
}


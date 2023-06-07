/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: volumecontrol.h
*
* Description: This is the header file for volume control which will allow us 
*              to have control and access to the various parts of the volume.
*
**************************************************************/





#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "fsLow.h"
#include "mfs.h"



#ifndef VOLUME_CONTROL_H
#define VOLUME_CONTROL_H


// The signature of the volume
#define MAGIC_NUMBER 39106557589782

// The initial number of entries requested for all directories
#define NUM_ENTRIES 50

// The number of extents we want for a directory entry
#define NUM_OF_EXTENTS 8

/* The minimum number of blocks to ask the free space for our extents */
#define MINIMUM_NUMBER_OF_EXTENT_BLOCKS 3




// Our volume control block (instance of VCB) to reference throughout the file system 
extern struct VCB *volumeControlBlock;



// The struct for the extent node
struct ExtentNode{
	
	// The address of the first block in the extent
	int blockNum;
	//The number of blocks counted after the first block
    int count;

} ExtentNode;




// The struct for our directory entry
struct directoryEntry {

	// The name of the file
	char fileName[30];
	// The size of the file
	int  size;
	// The number of entries in the directory entry
	int numOfEntries;
	// The time the file was created
	time_t  dateCreated;
	// The time the file was modified
	time_t  dateModified;
	// The type of file. 0 = directory, 1 = file
	int fileType;
	// The state of a directory entry. Used = true, unused = false.
	bool used;
	// The eight extent tables
	struct ExtentNode extents[NUM_OF_EXTENTS];

} directoryEntry; 



// The struct for our volume control block
struct VCB {

	// The size of each block in the partition
	int sizeOfBlocks;
	// The number of total blocks in the partition
	int numOfTotalBlocks;
	// The number of free blocks in the partition
	int numOfFreeBlocks;
	// The type/signature of the volume
	u_int64_t magicNumber;
	// The starting block number of VCB
	int startingBlockOfVCB;
	// The starting block number of the root directory. 
	int startingBlockOfRootDirectory;
	// The length of the root directory
	int lengthOfRootDirectory;
	// The starting block # of our free space map
	int startingBlockOfFreeSpace;
	// The length of the free space map
	int lengthOfFreeSpaceMap;

} VCB; 



/* A function to allocate space for the volume control block and initialize the 
   fields of the VCB */
void volumeControlInit(uint64_t numberOfBlocks, uint64_t blockSize);

/* A function to handle writing to disk (utilizing extents) a passed in buffer which 
   has the sole purpose of preventing code from being reused again and again */
void extentsWrite(struct directoryEntry *buffer, int index);


#endif 
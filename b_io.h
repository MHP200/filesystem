/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: b_io.h
*
* Description: Interface of basic I/O functions
*
**************************************************************/

#ifndef _B_IO_H
#define _B_IO_H
#include <fcntl.h>
#include "volumecontrol.h"

typedef int b_io_fd;

b_io_fd b_open (char * filename, int flags);
int b_read (b_io_fd fd, char * buffer, int count);
int b_write (b_io_fd fd, char * buffer, int count);
int b_seek (b_io_fd fd, off_t offset, int whence);
int b_close (b_io_fd fd);

// A helper funciton to search the next allocated block in file
int getNextblockOfFile(struct ExtentNode extents[NUM_OF_EXTENTS],int currentBlock);
// A helper function to merge two extents
int mergeExtents(struct ExtentNode originExtents[NUM_OF_EXTENTS],struct ExtentNode tempExtents[NUM_OF_EXTENTS]);

int printExtent(struct ExtentNode extents[NUM_OF_EXTENTS]);

#define MAXFCBS 20
#define B_CHUNK_SIZE 512
#define EPos (EBuf-EAvail)
#define UPos (UBuf-UEmpty)

typedef struct b_fcb
	{
	/** TODO add al the information you need in the file control block **/
	char * openFileBuffer;		//holds the open file buffer
	int bufferPosition;		//holds the current position in the buffer
	int buflen;		//holds how many valid bytes are in the buffer             
	
	
	int filePosition;   // To track where in the file the caller is in


	// DO WE NEED?????
	/* A variable to track which block of the LBA we're currently in (to use in 
	   calls to LBAread) */
	int currentBlock;

	struct directoryEntry * parentDir; //To hold the directory which contains the file
	struct directoryEntry * file;//get the information about the file
	int canRead;	//this file is opened for read
	int canWrite;	//this file is opened for write
	int appendModeOn;//this file is opened for append

	} b_fcb;
	
b_fcb fcbArray[MAXFCBS];

#endif


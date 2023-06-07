/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: b_io.c
*
* Description: Basic File System - Key File I/O Operations
*
**************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>			// for malloc
#include <string.h>			// for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "b_io.h"
#include "parsepath.h"

int startup = 0;	//Indicates that this has not been initialized

//Method to initialize our file system
void b_init ()
	{
	//init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
		{
		fcbArray[i].openFileBuffer = NULL; //indicates a free fcbArray
		}
		
	startup = 1;
	}

//Method to get a free FCB element
b_io_fd b_getFCB ()
	{
	for (int i = 0; i < MAXFCBS; i++)
		{
		if (fcbArray[i].openFileBuffer == NULL)
			{
			return i;		//Not thread safe (But do not worry about it for this assignment)
			}
		}
	return (-1);  //all in use
	}
	
// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR
b_io_fd b_open (char * filename, int flags)
	{
	b_io_fd returnFd;
	if (startup == 0) b_init();  //Initialize our system
	
	returnFd = b_getFCB();				// get our own file descriptor
										// check for error - all used FCB's
	// no free fcb ,return -1
	if(returnFd	== -1){
		return returnFd;
	}
	
	int fileExists = false;		// Will store the state if the file exists or not
	int fileNameIsUsed = false;	// Will store the state if the file name is used or not
	int indexOfFreeSlot = -1;		// Will store the index of the free slot in ending directory
	time_t seconds;				// To store the current time
	struct directoryDetails * parseResult;// To hold the returned directory details of parsePath()
	parseResult = malloc(sizeof(struct directoryDetails));
	parseResult = parsePath(filename,currentDirectory,parseResult);
	
	// To check if the middle path before the last element is valid
    // The return value is NULL, the middle path is invalid(either directory entry is not found 
    //  or is not a directory), return -1
    if(parseResult == NULL){
        return -1; 
    }
	// Get the directory of where the last element will be checked
	struct directoryEntry * dir = parseResult->ptrToDirectory;
	// Get the name of last element from directory details
	char * nameOfLastElement = parseResult->finalElementName;

	// To analyze the last element
	for(int i = 0;i<dir[0].numOfEntries; i++){
		// Check if the last element exist in the directory
		if(strcmp(dir[i].fileName, nameOfLastElement) == 0){
			// printf("File '%s''s name already used.\n",nameOfLastElement);
			// If exist, the we can not create another file with the same name
			fileNameIsUsed = true;
			// Check if it's a file
			if(dir[i].fileType == 1){
				// printf("File '%s' already existed.\n",nameOfLastElement);
				// Obtain the directory entry
				fcbArray[returnFd].file = &dir[i];
				// It's a file, so we are allow to access to it
				fileExists = true;
			}
			break;	// The element is found in directory, break the loop
		}
	}

	// Flag "O_CREAT"
	// If caller tries to create a file, make sure the name is not used
	// ***The name is not used, naturally means the file does not exist***
	if((flags & O_CREAT) && fileNameIsUsed == false){
		// // create a new file, malloc space to hold the directory entry
		// fcbArray[returnFd].file = malloc(sizeof(struct directoryEntry));
		// Get the access the b_fcb's file
		for(int i = 0;i<dir[0].numOfEntries; i++){
			if(dir[i].used == false){
				indexOfFreeSlot = i;
				break;
			}
		}

		// If no more free slot is in the directory, can not create a file
		// b_open will return -1
		if(indexOfFreeSlot == -1){
			free(parseResult); // Free unused resource
			return -1;
		}
		// b_fcb's file will get the address of the free slot in directory
		fcbArray[returnFd].file = &dir[indexOfFreeSlot];
		// Create a tempFile to point to the b_fcb's file
		struct directoryEntry * tempFile = fcbArray[returnFd].file;
		// Below will initialize the file directory entry
		strcpy(tempFile->fileName,nameOfLastElement);
		tempFile->size = 0;	// The file initially has no content, the size is 0
		seconds = time(NULL);
		tempFile->dateCreated = seconds;
		tempFile->dateModified = seconds;
		tempFile->fileType = 1;	// file type 1 means it's a file
		tempFile->used = true;
		for(int i =0;i<NUM_OF_EXTENTS;i++){	// Initially, no block are allocated to file
			tempFile->extents[i].blockNum = 0;
			tempFile->extents[i].count = 0;
		}
		// Need to specify the file is now exist in b_fcb
		fileExists = true;
	}

	// Either the file is not found or no new file is created, b_open fails, return -1
	if(!fileExists){
		free(parseResult); // Free unused resource
		return -1;
	}

	// From here now, we know the file exist in the directory.
	// So we now can populate the b_fcb.

	// The openFileBuffer will temporary hold the data in a size of a block
	fcbArray[returnFd].openFileBuffer = malloc(volumeControlBlock->sizeOfBlocks);

	// The parentDir will hold the memory address of the directory which contains the file entry.
	// We will use this to write the change of file entry back to disk.
	fcbArray[returnFd].parentDir = parseResult->ptrToDirectory;
	fcbArray[returnFd].bufferPosition = 0;
	fcbArray[returnFd].buflen = 0;
	fcbArray[returnFd].filePosition = 0;// By default, should read from index 0 of the file
	fcbArray[returnFd].canRead = false;	// By default, we don't know if the file is openned for read
	fcbArray[returnFd].canWrite = false;// By default, we don't know if the file is openned for write
	fcbArray[returnFd].appendModeOn = false;// By default, the append mode should be off
	// set the current block in b_fcb
	fcbArray[returnFd].currentBlock = fcbArray[returnFd].file->extents[0].blockNum;


	// Only when the file exist can it be read or written
	// Flag "O_RDONLY"
	if((flags & O_RDONLY)==0){
		fcbArray[returnFd].canRead = true;
		fcbArray[returnFd].canWrite = false;
	}
	// Flag "O_WRONLY"
	if(flags & O_WRONLY){
		fcbArray[returnFd].canRead = false;
		fcbArray[returnFd].canWrite = true;
	}
	// Flag "O_RDWR"
	if(flags & O_RDWR){
		fcbArray[returnFd].canRead = true;
		fcbArray[returnFd].canWrite = true;
	}

	// Flag "O_TRUNC"
	// If the file exists and the file can be written, it will be truncated to length 0
	if((flags & O_TRUNC) && ((flags & O_RDWR) || (flags & O_WRONLY)) && fileExists){
		struct directoryEntry * tempFile = fcbArray[returnFd].file;
		// Set the file size to 0
		tempFile->size = 0;
		fcbArray[returnFd].filePosition = 0;
		// Create an empty buffer as helper to erase all the data on the file
		char * emptyBuffer = malloc(volumeControlBlock->sizeOfBlocks);
		memset(emptyBuffer,0,volumeControlBlock->sizeOfBlocks);
		// Erase all the content in the blocks of file
		for(int i = 0;i < NUM_OF_EXTENTS;i++){
			if(tempFile->extents[i].blockNum != 0){
				LBAwrite(emptyBuffer,tempFile->extents[i].count,tempFile->extents[i].blockNum);
			}
		}
		free(emptyBuffer);
		
		freeBlocks(tempFile->extents);	// Free the bits in free space map
		// initialize the extents
		for(int i = 0;i < NUM_OF_EXTENTS;i++){
			tempFile->extents[i].blockNum = 0;
			tempFile->extents[i].count = 0;
		}
	}

	// Flag "O_APPEND"
	// If the file is openned in append mode, every write will append to the end of file
	if((flags & O_APPEND) && fileExists){
		fcbArray[returnFd].appendModeOn = true;
		// The file offset set to be the end of the file
		fcbArray[returnFd].filePosition = fcbArray[returnFd].file->size;

		// To store the index of the last block of the directory entry's extents
		int indexOfLastBlocksInExtents = NUM_OF_EXTENTS - 1 ; // Assume the extents is full
		for(int i =0;i<NUM_OF_EXTENTS;i++){
			// If extents is not full, return the index of last used slot
			if(fcbArray[returnFd].file->extents[i].blockNum == 0){
				indexOfLastBlocksInExtents = i-1; 
				break;
			}
		}
		//If no block are allocated to the file, the current block set to be 0
		if(indexOfLastBlocksInExtents == -1){
			indexOfLastBlocksInExtents = 0;
		}
		// The current block is the last block allocated for the file
		fcbArray[returnFd].currentBlock = 
		fcbArray[returnFd].file->extents[indexOfLastBlocksInExtents].blockNum +
		fcbArray[returnFd].file->extents[indexOfLastBlocksInExtents].count - 1;
	}
	else{
		fcbArray[returnFd].appendModeOn = false;
	}
	
	free(parseResult); // Free unused resource
	return (returnFd);						// all set
	}


int b_seek (b_io_fd fd, off_t offset, int whence)
	{
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
	// Create a variable to store the new offset
	off_t new_offset; 
	// Modify the file position based on the "whence" parameter
	switch (whence) {
		// If "whence" is SEEK_SET, set the file position to the "offset"
		case SEEK_SET:  
			fcbArray[fd].filePosition = offset;
			break;
			// If "whence" is SEEK_CUR, move the file position by the "offset"
		case SEEK_CUR:  
			 fcbArray[fd].filePosition += offset;
			break;
			// If "whence" is SEEK_END, set the file position to the end of the file plus the "offset"
		case SEEK_END:  
			 fcbArray[fd].filePosition =  fcbArray[fd].file->size + offset;
			break;
			// If "whence" is not any of the above, return an error
		default:  
			return (-1);
	}
	new_offset  = fcbArray[fd].filePosition;
	// return value is the final offset value  
	return (int)new_offset;
}




// Interface to write function
/* Steps:
	1) Go the blocks where the file offset belongs to.
	2) If file offset is within the file's allocated block, which means the block is avaliable.
	   write to block from the offset.
	3) Check if file has more blocks and the remain caller's buffer can fill in some whole blocks.
	   Write the caller's remain buffer in size of blocks directly to the allocated blocks of file.
	4) If the caller's buffer is not totally written, check if file has avaliable block.
	   If yes, write the remain buffer to the file's block.
	   If not, create a temporary extentNode array and allocated more blocks base on the size of 
	   the remain buffer.
	   Then, write the remain buffer onto new allocated blocks
	   Lastly, merge the temporary extentNode array to the file's extents
*/
int b_write (b_io_fd fd, char * buffer, int count)
	{
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
	// The file can not be written, return -1
	if(fcbArray[fd].canWrite == false){
		return -1;
	}

	int hasWritten = 0;	// Store the number of bytes that is successfully written onto disk
	struct directoryEntry * file = fcbArray[fd].file; // Get the memory address of file entry
	int fileSize = file->size;	// Store the file size
	int startingBlockOfFile = file->extents[0].blockNum; // Store the starting block number of file
	int blockSize = volumeControlBlock->sizeOfBlocks; // Store the size of block
	int orginFileOffset = fcbArray[fd].filePosition; // To store the original file offset
	// temporary buffer to hold the data read from file
	char * tempBuffer = malloc(blockSize);

	// printf("File size before write: %d\n",file->size);

	// Keep track of next avaliable block of the file
	int currentAvaliableBlock = startingBlockOfFile;
	// To get the sequential logical block number where the file offset is in
	int offsetToFileOffsetBlock = fcbArray[fd].filePosition / blockSize; 

	// If the file has been allocated blocks, go the block where the file offset is in
	if(currentAvaliableBlock > 0){
		while(offsetToFileOffsetBlock > 0){
			currentAvaliableBlock = getNextblockOfFile(file->extents,currentAvaliableBlock);
			offsetToFileOffsetBlock --;
		}
	}

	// If the block where the file offset belongs to is available, start writting to the block
	if(currentAvaliableBlock > 0){
		
		int offsetInCurrentBlock = fcbArray[fd].filePosition % blockSize;
		// Read the data from the current offset block in file
		LBAread(tempBuffer,1,currentAvaliableBlock);
		// Get the number of bytes from the offset in the current block
		int bytesLeftFromBufferOffset = blockSize - offsetInCurrentBlock;
		int actualWrite = count > bytesLeftFromBufferOffset? bytesLeftFromBufferOffset : count;
		// Copy the first several bytes from caller's buffer to temporary buffer
		memcpy(tempBuffer + offsetInCurrentBlock,buffer,actualWrite);
		// Write the temporary buffer back to disk
		LBAwrite(tempBuffer,1,currentAvaliableBlock);
		hasWritten += actualWrite;
		count -= actualWrite;
		fcbArray[fd].filePosition += actualWrite;
		// Check if the current offset is beyond the current block
		if(hasWritten == bytesLeftFromBufferOffset){
			// If yes, read the next block of file
			currentAvaliableBlock = getNextblockOfFile(file->extents,currentAvaliableBlock);
		}
	}
	
	if(count > 0){
		/*If count is not zero, first write to the available whole block of file*/
		int wholeBlocksToWrite = count/blockSize; // To store the number of whole block to write
		
		// If the number of whole block to write greater than 0, we will check if the file still has avaliable blocks.
		while((wholeBlocksToWrite > 0) && (currentAvaliableBlock > 0)){
			// Only if the next block of file is avaliable will write the buffer to block
			// Write to a block of the file
			LBAwrite(buffer + hasWritten,1,currentAvaliableBlock);
			hasWritten += blockSize; // Update hasWritten
			count -= blockSize;	// Update remain count
			fcbArray[fd].filePosition += blockSize; // Update offset of file
			wholeBlocksToWrite --; // Update the remain requested whole block, if reach zero, end 
									// writting to whole block
			currentAvaliableBlock = getNextblockOfFile(file->extents,currentAvaliableBlock);
		}
		// Either the remain buffer is not enough to write to a whole block or the file has no more avaliable blocks
		if(count > 0){
			// If true, means the file can contain the remain buffer
			if(currentAvaliableBlock > 0){
				// Read the data from the current offset block in file
				LBAread(tempBuffer,1,currentAvaliableBlock);
				// Copy the leftover bytes from caller's buffer to temporary buffer
				memcpy(tempBuffer,buffer + hasWritten,count);
				// Write the temporary buffer back to disk
				LBAwrite(tempBuffer,1,currentAvaliableBlock);
				hasWritten += count;
				fcbArray[fd].filePosition += count;
				count = 0;
			}
			// The file offset is beyond the file's allocated blocks, ask blockAllocator for more blocks
			else{
				struct ExtentNode tempExtents[NUM_OF_EXTENTS];
				// initialize the tempExtents
				for(int i =0;i< NUM_OF_EXTENTS;i++){
					tempExtents[i].blockNum = 0;
					tempExtents[i].count = 0;
				}
				blockAllocator(wholeBlocksToWrite + 1,tempExtents);
				// First write to the whole block
				currentAvaliableBlock = tempExtents[0].blockNum;
				while(wholeBlocksToWrite > 0 && currentAvaliableBlock > 0){
					LBAwrite(buffer + hasWritten,1,currentAvaliableBlock);
					hasWritten += blockSize;
					count -= blockSize;
					fcbArray[fd].filePosition += blockSize;
					wholeBlocksToWrite --;
					currentAvaliableBlock = getNextblockOfFile(tempExtents,currentAvaliableBlock);
				}
				// Second write the remain buffer to avaliable block
				if(currentAvaliableBlock > 0){
					LBAwrite(buffer + hasWritten,1,currentAvaliableBlock);
					hasWritten += count;
					fcbArray[fd].filePosition += count;
					count = 0;
				}
				// Last thing is to merge the tempExtents to the file's original extents
				mergeExtents(file->extents,tempExtents);
			}
		}
	}

	free(tempBuffer); // Free unused resource

	// Check if the file need to resize
	if((hasWritten + orginFileOffset) > fileSize ){
		file->size = (hasWritten + orginFileOffset);
	}
	// printf("File size after write: %d\n",file->size);
	// printExtent(file->extents);
	return hasWritten; 
	}

//Calculates the min of two given values
int min(int a,int b)
{
	if(a<b)
	{
		return a;
	}
	else{
		return b;
	}
}

// Interface to read a buffer

// Filling the callers request is broken into three parts
// Part 1 is what can be filled from the current buffer, which may or may not be enough
// Part 2 is after using what was left in our buffer there is still 1 or more block
//        size chunks needed to fill the callers request.  This represents the number of
//        bytes in multiples of the blocksize.
// Part 3 is a value less than blocksize which is what remains to copy to the callers buffer
//        after fulfilling part 1 and part 2.  This would always be filled from a refill 
//        of our buffer.
//  +-------------+------------------------------------------------+--------+
//  |             |                                                |        |
//  | filled from |  filled direct in multiples of the block size  | filled |
//  | existing    |                                                | from   |
//  | buffer      |                                                |refilled|
//  |             |                                                | buffer |
//  |             |                                                |        |
//  | Part1       |  Part 2                                        | Part3  |
//  +-------------+------------------------------------------------+--------+

// A method used for debugging purposes, tests b_read using linux write
struct directoryEntry * debugTest()
{
	struct directoryEntry * file = (struct directoryEntry *)malloc(sizeof(directoryEntry));
	strcpy(file->fileName,"Test");
	file->fileType=1;
	file->extents->blockNum=40;
	file->extents->count=5;
	file->size=2000;
	fdDir *fDirp=(fdDir *)malloc(sizeof(fdDir));
	strcpy(fDirp->dirEntryInfo.d_name,file->fileName);
	fDirp->dirEntryInfo.fileType=file->fileType;
	FILE * fp;
	fp=fopen("/Users/amitbal/Desktop/File1.txt","r");
	char * buffer = (char *)malloc(20000);
	size_t sz = fread(buffer, sizeof(char), 20000, fp);
	LBAwrite(buffer,file->extents->count,file->extents->blockNum);
	return file;
	
}

//debug function to check values of variables in b_read()
void debugPrintRead(int fd, const char *msg, int fileSize, int EAvail, int UEmpty, int EBuf, int UBuf, int bytesToCopy  ) {
	printf("-----------\n");
	printf("filesize=%d fileposition=%d\n",fcbArray[fd].file->size, fcbArray[fd].filePosition );
	printf("%30s bToCpy=%d EPos=%d UPos=%d EAvail=%d UEmpty=%d  fileSize=%d  EBuf=%d  UBuf=%d \n", msg, bytesToCopy, EPos, UPos, EAvail, UEmpty, fileSize, EBuf, UBuf);
	printf("-----------\n");
}


//reads a file given a fd, a buffer to read into, and a count of bytes to read
int b_read (b_io_fd fd, char * buffer, int count)
	{

	int fileSize = fcbArray[fd].file->size;	// Store the file size
	int startingBlockOfFile =  fcbArray[fd].file->extents[0].blockNum;// Store the starting block number of file
	
	//if file position exceeds the file size then return error. 
	if( fcbArray[fd].filePosition >= fileSize ) {
		return -1;
	}
	
	//if user does not have access to file then return error. 
	if(fcbArray[fd].canRead==false)
	{
		return -1;
	}

	//index to iterate through extents
	int index=0;

	//extentOffset is the distance in bytes between filePosition and the closest extent. 
	int extentOffset = fcbArray[fd].filePosition;

	//calculate extentOffset by subracting the number of bytes allocated for extent from file position. 
	while(fcbArray[fd].filePosition > fcbArray[fd].file->extents[index].count *B_CHUNK_SIZE && index < NUM_OF_EXTENTS ) {
		if( fcbArray[fd].file->extents[index].blockNum != 0 && fcbArray[fd].file->extents[index].count != 0 ) {
			extentOffset-= fcbArray[fd].file->extents[index].count;
			
		}
	index ++;	
	}


	 //UBuf is the size of the user buffer. UBuf cannot exceed the number of bytes that have yet to be read. 
	int UBuf=min(count,fileSize-fcbArray[fd].filePosition);   
	 //EBuf is the size of the intermediate buffer used.
	int EBuf= fcbArray[fd].file->extents[index].count * B_CHUNK_SIZE;
	//UEmpty is the number of free space in UBuf, initialize set to size of UBuf. 
	int UEmpty=UBuf; 
	//pUBuf pointer to user buffer.  
	char *pUBuf = buffer;
	//pEBuf is pointer to intermediate buffer. 
	char *pEBuf = malloc( EBuf );
	int bytesSent=0;//Keep track of the number of bytes sent to the user
	//Read the first extent 
	LBAread(pEBuf, fcbArray[fd].file->extents[index].count, fcbArray[fd].file->extents[index].blockNum);
	index++; //Index used to iterate throught the extents table

	//The number of bytes available in the intermediate buffer that are ready to be copied into the user buffer.
	int EAvail = EBuf - extentOffset; 

	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
	
	/*The conditions for this while loop:
	*UEmpty>0: We still haven't satisfied the user's request for bytes
	*EAvail>0: There are still bytes in the file system buffer that need to be copied
	*index<NUM_OF_EXTENTS : Iterate through the entire extent table without going out of bounds
	*/
	while( UEmpty > 0 && ( EAvail > 0 || index < NUM_OF_EXTENTS )) {
	
		// filesystem buffer is equivalent to the intermediate buffer as described in class
		//Part 1.  Do we have more bytes available in the file system buffer than there is empty space in the user buffer.
		if( UEmpty <= EAvail ) {
			//Copy the available bytes in the intermediate buffer into the user buffer.  
			memcpy( pUBuf + UPos, pEBuf+EPos, UEmpty );
			bytesSent+=UEmpty;
			fcbArray[fd].filePosition+=bytesSent;
			//Since we have filled the user buffer with the requested number of bytes we return. 
			return bytesSent;
		}

		//Part 2 The file system buffer already contains bytes that  the user can read
		if(UEmpty > EAvail && EAvail > 0  ) {
			//Copy the availiable bytes in the intermediate buffer into the user buffer
			memcpy( pUBuf + UPos, pEBuf + EPos, EAvail );
			bytesSent+=EAvail;
			UEmpty -= EAvail;
			//There are no more bytes to read from the file system buffer
			EAvail = 0;
		}

		//Part 3 The extents buffer is empty but we have not filled user buffer with the requested amout of bytes
		if( UEmpty > 0 && EAvail == 0 ) {

			
			if( UEmpty > fcbArray[fd].file->extents[index].count * B_CHUNK_SIZE ) {
				//User buffer has enough capacity to copy entire extents into user buffer
				LBAread(pUBuf+UPos, fcbArray[fd].file->extents[index].count, fcbArray[fd].file->extents[index].blockNum);
				bytesSent+=fcbArray[fd].file->extents[index].count * B_CHUNK_SIZE;
				UEmpty -= fcbArray[fd].file->extents[index].count * B_CHUNK_SIZE;
				index ++; //Iterate throught the extents table
			}else{
				//User buffer does not have enough capacity to copy entire extents entry into user buffer. 
				//Let's copy data into intermediate buffer called EBuf and then start the copy loop again.
				free(pEBuf);
				EBuf = fcbArray[fd].file->extents[index].count * B_CHUNK_SIZE;
				pEBuf = malloc(EBuf);
				LBAread(pEBuf, fcbArray[fd].file->extents[index].count, fcbArray[fd].file->extents[index].blockNum);
				EAvail = fcbArray[fd].file->extents[index].count * B_CHUNK_SIZE;
				index ++; //Iterate throught the extents table

				

			}
		}
	
	}
	//Increment the position within the file by bytesSent
	fcbArray[fd].filePosition+=bytesSent;
	return bytesSent;
	
	}
	
// Interface to Close the file	
int b_close (b_io_fd fd)
	{
		// On close, we write the directory which containing the file entry back to disk
		// if the file is not open for read only
		if(fcbArray[fd].canWrite == true){
			extentsWrite(fcbArray[fd].parentDir,0);
		}
		fcbArray[fd].file = NULL;	// Indicate the fcb is free
		fcbArray[fd].parentDir = NULL;	// Indicate the fcb is free
		free(fcbArray[fd].openFileBuffer);// Free allocated memory to save resource
		fcbArray[fd].openFileBuffer = NULL;// Point the openFileBuffer to NULL to set
											// b_fcb into free state
		return 0;
	}
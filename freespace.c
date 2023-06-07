/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: freespace.c
*
* Description: This is the .c file for the free space management system
*              in which we will implement the functions necessary to
*              manage free space.
*
**************************************************************/



#include "freespace.h"


// Our free space map to reference throughout the file system 
unsigned char *freeSpaceMap;



/* A function to initialize the free space map, where we allocate space for the 
   free space map, initialize the map with bit values of 1 (1 = free, 0 = used),
   mark the blocks holding this map as well as the volume control block as used, 
   and where we initialize certain fields of the volume control block such as
   the lenght of the map and its starting block number */
void freeSpaceInit(){
    
    // To store the number of bits in our bitmap
    int m = volumeControlBlock->numOfTotalBlocks;

    // To get the number of bytes that the bitmap will take up
    m = m / 8;

    // To store the number of bytes per block
    int n = volumeControlBlock->sizeOfBlocks;

    /* To get the number of blocks that we'll need to allocate for our free 
       space map */
    int lengthOfFreeSpaceMap = (m + (n-1)) / n;

    // To initialize the size of the free space map in the volume control block
    volumeControlBlock->lengthOfFreeSpaceMap = lengthOfFreeSpaceMap;
	

    // To allocate the memory needed to store our free space map
	freeSpaceMap = malloc(volumeControlBlock->lengthOfFreeSpaceMap * 
	                      volumeControlBlock->sizeOfBlocks);
    
    // To initialize our free space map
    initializeMap();

	// To get the starting block of the VCB
	volumeControlBlock->startingBlockOfVCB = getBlocks(1);

	// To reduce number of free blocks by 1
    volumeControlBlock->numOfFreeBlocks = volumeControlBlock->numOfFreeBlocks - 1;

	// To set the bit/block associated with the VCB to a value of 0
	clearMultipleBits(volumeControlBlock->startingBlockOfVCB, 1);
    
    /* To mark the blocks holding the VCB and free space map as used and to store
       the first block of the map to use in the following statement */
	volumeControlBlock->startingBlockOfFreeSpace = 
	                       getBlocks(volumeControlBlock->lengthOfFreeSpaceMap);
	
	// To reduce number of free blocks by the length of the free space map (5 blocks)
    volumeControlBlock->numOfFreeBlocks = volumeControlBlock->numOfFreeBlocks - volumeControlBlock->lengthOfFreeSpaceMap;

	// To set the bits/blocks associated with the free space map to values of 0
	clearMultipleBits(volumeControlBlock->startingBlockOfFreeSpace, 
	                  volumeControlBlock->lengthOfFreeSpaceMap);

	LBAwrite(freeSpaceMap, volumeControlBlock->lengthOfFreeSpaceMap, 
	         volumeControlBlock->startingBlockOfFreeSpace);


}




/* A function to initialize the free space map with bit values of 1, where 1 = 
   free and 0 = used. Helper function for freeSpaceInit(). */
void initializeMap(){

	/* To set every bit to a value of 1, indicating that a block is free, since
	   we are initializing the map */
	for (int i = 0; i < volumeControlBlock->numOfTotalBlocks; i++){
		setBit(i);
	}

}




/* A function that will be used to alter a bit, specifically setting a bit to a 
   value of 1, indicating a free block, in the free space map. Can act as a
   helper function for initializeMap(), seen above */
void setBit(int position){

	/* To store the byte number, or arrayNumber, of the byte the caller wants to
	   modify */
	int arrayNumber = position / 8;
	/* To store the bit number, or arrayIndex, of the bit the caller wants to
	   alter */ 
	int arrayIndex = position % 8;
	// A variable to store the target byte so that the caller can alter a bit 
	unsigned char byte = freeSpaceMap[arrayNumber];
	// To set the bit at the desired position a value of 1, or free
	byte |= (1 << 7 - arrayIndex);
	// To reflect the desired changes in the free space map
	freeSpaceMap[arrayNumber] = byte;
	
}




/* A function that will be used to alter a bit, specifically setting a bit to a 
   value of 0, indicating a used block, in the free space map. Can act as a
   helper function for blockAllocator(), seen below  */ 
void clearBit(int position){

	/* To store the byte number, or arrayNumber, of the byte the caller wants to
	   modify */
	int arrayNumber = position / 8;
	/* To store the bit number, or arrayIndex, of the bit the caller wants to
	   alter */
	int arrayIndex = position % 8;
	/* A variable to store the target byte in its inverted state, meaning, where 
	   all of the original bits are flipped, so that the caller can eventually 
	   alter a bit */
	unsigned char byte = ~freeSpaceMap[arrayNumber];
	/* To set the bit at the desired position a value of 1 so that when all of the
	   bits of "byte" are flipped, we can use the & operator to clear that bit */
	byte |= (1 << 7 - arrayIndex);
	// To flip all of the bits of "byte"
	byte = ~byte;
	// To fully clear the bit at the desired position using the & operator
	freeSpaceMap[arrayNumber] = freeSpaceMap[arrayNumber] & byte;

}





/* A function that will be used to alter multiple bits, specifically setting a 
   consecutive number of bits to values of 0, where 0 indicates a used block, in
   the free space map. Can act as a helper function for blockAllocator() and 
   freeSpaceInit(). */ 
void clearMultipleBits(int position, int numOfBlocksToClear){
	
	// A variable to store the position
	int newPosition = position;

	// To mark multiple bits as used
    for (int i = 0; i < numOfBlocksToClear; i++){
        
		// To mark an individual bit as used
        clearBit(newPosition);

		// To update the position
		newPosition = newPosition + 1;       
	}

}








/* A function to free the blocks associated with a directory entry. It sets every
   bit for those extents values of 1, which indicates free (0 = used) */
void freeBlocks(struct ExtentNode arrayOfExtents[NUM_OF_EXTENTS]){


	/* A pointer to point to the beginning of the array of extents to begin freeing
	   the extents */
	struct ExtentNode *arrayOfExtentsPtr = arrayOfExtents;

	/* A variable to get a starting position in the free space map to begin setting
	   bits to values of 1. The position always begins at "blockNum" since we have
	   extents */
	int position = 0;

	/* To store how many blocks have been freed. Will be used to modify the volume
	   control block's "numOfFreeBlocks" variable */
	int additionalFreeBlocks = 0;


	// To iterate through the array of extents to begin freeing blocks
	for (int i = 0; i < NUM_OF_EXTENTS; i++){
		

		/* To set the position to "blockNum", the starting block number in the 
		   extent */
		position = arrayOfExtentsPtr->blockNum;

		/* To check if "blockNum" is 0, which indicates that we're done freeing
		   blocks since "blockNum" of 0 and "count" of 0 are not valid extent
		   values */
		if (position == 0){
			
			break;
		
		}

		/* To iterate across the extent, setting each bit in the contiguous set of
		   allocated blocks values of 1 */
		for (int j = 0; j < arrayOfExtentsPtr->count; j++){
			
			// To set a bit to a value of 1
			setBit(position);
			// To get the next bit in the contiguous set
			position++;
			// To increment the number of newly freed blocks
			additionalFreeBlocks = additionalFreeBlocks + 1;

		}

		// To point to another extent in the array of extents
		arrayOfExtentsPtr++;
		
	}

	// To modify the number of free blocks in the volume to reflect the new changes
	volumeControlBlock->numOfFreeBlocks = volumeControlBlock->numOfFreeBlocks + additionalFreeBlocks;
	
	// To write the volume control block to disk
	LBAwrite(volumeControlBlock, 1, volumeControlBlock->startingBlockOfVCB);

	// To write the free space map back to disk
	LBAwrite(freeSpaceMap, volumeControlBlock->lengthOfFreeSpaceMap, 
	         volumeControlBlock->startingBlockOfFreeSpace);

}










/* A function to give the caller the requested amount of free blocks. Will mark 
   blocks as used and returns an array of extents to fulfill our chosen extent-based
   allocation method for directory entries */
struct ExtentNode *blockAllocator(int numOfBlocks, struct ExtentNode arrayOfExtents[NUM_OF_EXTENTS]){


	/* A variable that will get the starting block number of a set of contiguous
	   blocks in the extent */
	int startingBlockNumber = 0;

	// A variable to get the number of blocks in the extent
	int count = 0;

	// A variable to keep track of how many blocks are left to allocate
	int numOfBlocksLeft = numOfBlocks;

	// To point to the beginning of the array of extents we want to fill
	struct ExtentNode *arrayOfExtentsPtr = arrayOfExtents;

	
	/* To check if the number of blocks is a valid value, i.e., above 0. The function
	   getBlocks() returns -2 if "numOfBlocks" is less than 1 */
	if (getBlocks(numOfBlocks) == -2){
		
		return NULL;
	
	}

	/* To check if we can just find "numOfBlocks" number of consecutive bits with 
	   values of 1 (1 = free block, 0 = used). If not, and we can't even find 1, 
	   2, or "MINIMUM_NUMBER_OF_EXTENT_BLOCKS" (which we've set to 3) consecutive 
	   1s, we return NULL since we're running out of space in our file system. The
	   function getBlocks() returns -1 if we can't find "numOfBlocks" number of consecutive 1s. */
	if (!(getBlocks(numOfBlocks) == -1)){

		
		// To store the starting block number of the consecutive 1s
		startingBlockNumber = getBlocks(numOfBlocks);

		/* The next two lines are written to store into the first extent of the 
		   array of extents the starting block number of consecutive 1s and
		   the "count" number of blocks in the extent */
		arrayOfExtentsPtr->blockNum = startingBlockNumber;
		arrayOfExtentsPtr->count = numOfBlocks;

		// To clear mark the bits that have been allocated as "used", or 0
		clearMultipleBits(startingBlockNumber, numOfBlocks);

		// To reduce number of free blocks by "numOfBlocks"
        volumeControlBlock->numOfFreeBlocks = volumeControlBlock->numOfFreeBlocks - numOfBlocks;
		
		// To write the volume control block to disk
		LBAwrite(volumeControlBlock, 1, volumeControlBlock->startingBlockOfVCB);

		// To write the free space map back to disk
		LBAwrite(freeSpaceMap, volumeControlBlock->lengthOfFreeSpaceMap, 
	             volumeControlBlock->startingBlockOfFreeSpace);

		return arrayOfExtentsPtr;


	} else if ((getBlocks(numOfBlocks) == -1) && numOfBlocks <= MINIMUM_NUMBER_OF_EXTENT_BLOCKS){
		
		// To return NULL if we have run out of space in our file system
		return NULL;

	}
	




	/* To iterate through the array of extents and assign each "blockNum" and "count"
	   a value */
	for (int i = 0; i < NUM_OF_EXTENTS; i++){



		/* To check if the number of blocks to allocate is now < 0. If so,
		   decrement the number of free blocks in the space and return the pointer
		   to the array of extents */
		if (numOfBlocksLeft < 1){


			// To point back to the beginning of the array of extents
			arrayOfExtentsPtr = arrayOfExtents;

			// To reduce number of free blocks by "numOfBlocks"
            volumeControlBlock->numOfFreeBlocks = volumeControlBlock->numOfFreeBlocks - numOfBlocks;

			// To write the volume control block to disk
			LBAwrite(volumeControlBlock, 1, volumeControlBlock->startingBlockOfVCB);
			
			// To write the free space map back to disk
			LBAwrite(freeSpaceMap, volumeControlBlock->lengthOfFreeSpaceMap, 
	                 volumeControlBlock->startingBlockOfFreeSpace);

			return arrayOfExtentsPtr;


		/* To check if the number of blocks left to allocate is between 1 and 
		   "MINIMUM_NUMBER_OF_EXTENT_BLOCKS" number of blocks. If so, just 
		   allocate that remaining number of blocks and fill in the remainder of
		   the extent */
		} else if ((numOfBlocksLeft >= 1) && (numOfBlocksLeft <= MINIMUM_NUMBER_OF_EXTENT_BLOCKS - 1)){


			// To get the starting block number of these final few blocks
			startingBlockNumber = getBlocks(numOfBlocksLeft);

			// To check if we've completely run out of space in our file system
			if (startingBlockNumber == -1){
				return NULL;
			}

			/* The next two lines are written to store into the extent the starting 
			   block number of consecutive 1s and the "count" number of blocks in
			   the extent */
			arrayOfExtentsPtr->blockNum = startingBlockNumber;
			arrayOfExtentsPtr->count = numOfBlocksLeft;

			// To clear mark the bits that have been allocated as "used", or 0
			clearMultipleBits(startingBlockNumber, numOfBlocksLeft);

			// To reduce number of free blocks by "numOfBlocks"
            volumeControlBlock->numOfFreeBlocks = volumeControlBlock->numOfFreeBlocks - numOfBlocks;

			// To write the volume control block to disk
			LBAwrite(volumeControlBlock, 1, volumeControlBlock->startingBlockOfVCB);
			
			// To write the free space map back to disk
			LBAwrite(freeSpaceMap, volumeControlBlock->lengthOfFreeSpaceMap, 
	                 volumeControlBlock->startingBlockOfFreeSpace);

			// To point back to the beginning of the array of extents
			arrayOfExtentsPtr = arrayOfExtents;

			return arrayOfExtentsPtr;


		/* The final condition that may be met where we have quite a few blocks to
		   allocate still */
		} else {

			// To get the starting block number of these final few blocks
			startingBlockNumber = getBlocks(MINIMUM_NUMBER_OF_EXTENT_BLOCKS);

			// To check if we've completely run out of space in our file system
			if (startingBlockNumber == -1){
				return NULL;
			}


			/* To get count how many consecutuve bits there are with values of 1 
			   past "MINIMUM_NUMBER_OF_EXTENT_BLOCKS" number of blocks  */
			count = getNextBit(startingBlockNumber + MINIMUM_NUMBER_OF_EXTENT_BLOCKS);

			/* To check if "count" is larger than the number of blocks left. This
			   is necessary as we don't want to give too many blocks to the caller */
			if ((count + MINIMUM_NUMBER_OF_EXTENT_BLOCKS) > numOfBlocksLeft){
				count = numOfBlocksLeft - MINIMUM_NUMBER_OF_EXTENT_BLOCKS;
			}

			/* The next two lines are written to store into the extent the starting 
			   block number of consecutive 1s and the "count" number of blocks in
			   the extent */
			arrayOfExtentsPtr->blockNum = startingBlockNumber;
			arrayOfExtentsPtr->count = MINIMUM_NUMBER_OF_EXTENT_BLOCKS + count;

			// To clear mark the bits that have been allocated as "used", or 0
			clearMultipleBits(startingBlockNumber, arrayOfExtentsPtr->count);


		}
		
		// To decrement the number of blocks left to allocate
		numOfBlocksLeft = numOfBlocksLeft - MINIMUM_NUMBER_OF_EXTENT_BLOCKS - count;
	
		// To increment the pointer to the array of extents
		arrayOfExtentsPtr++;

	}
	

	/* To check if we have been able to give the caller all of the blocks they wanted.
	   If not, return NULL */ 
	if (numOfBlocksLeft >= 1){
		
		return NULL;
	
	}

}





/* To count how many consecutuve bits there are with values of 1 past a certain
   position. Will be used as a helper function to blockAllocator(). */
int getNextBit(int position){


	// To check if the position is a valid position
	if (position < 0){

		// To return an error
		return -1;
	}

	
	/* To store the position of the bit in the free space map to use for bitmap
	   traversal (will increment) */
	int newPosition = position;
	
	// To store the byte number, or arrayNumber, of the byte we want to examine
	int arrayNumber = 0;

	// To store the bit number, or arrayIndex, of the bit we want to check 
	int arrayIndex = 0;

	// A variable to track which byte in the free space map we are iterating through
	unsigned char binaryNumber = 0;  

	/* A variable that will initially be set to 0x80 and will be halved in order
       to shift the bit with value 1 to the right. The & operator will be applied
       between "binaryNumber" and "value" so that I can check whether the current 
       bit is 1 or 0 */
    int value = 0; 

    /* To store the result when the & operator is applied to "binaryNumber" and
       "value" */
	int result = 0;

	// To store the number of consecutive bits of value 1 after the position
	int count = 0;



	for (int i = newPosition; i < volumeControlBlock->numOfTotalBlocks; i++)
	{
		
		// To get the arrayNumber or byte in the free space map
		arrayNumber = newPosition / 8;

		// To get the index of the bit in the byte
		arrayIndex = newPosition % 8;
		
		// To get the next byte of the free space map
		binaryNumber = freeSpaceMap[arrayNumber];

		
		
		// To iterate through the byte to get individual bits
		for (int j = arrayIndex; j < 8; j++)
		{

			/* To set "value" to the appropriate binary value we want to check the index
		   	   of the byte against */
			value = 0x80 / (1 << arrayIndex);
			
			// To store the result of "binaryNumber & value" for the following checks
			result = binaryNumber & value;


			// To check if the bit is 0. If so, return "count" 
			if (result == 0){
				

				// To return the num of consecutive bits, "count", of value 1
				return count;
				

        	// Otherwise, to check if we landed a value of 1.    
			} else if (result == value){
                

				/* To increment the "count"/number of consecutive bits that have
				   a value of 1 */
            	count = count + 1;

				// To increment our position in the free space bitmap
				newPosition = newPosition + 1;

				// To increment our bit position in the byte/"arrayIndex"
				arrayIndex++;
				
				
			}
		

		}
	

	}


	/* If this function is called at the end of the bitmap, block 19,531, return
	   0 since there aren't any more blocks to give ("count" has been intiliazed
	   to a value of 0 prior to the first 'for' loop") */
	return count;

}





/* A function to give to the caller free consecutive blocks. Acts as a helper 
   function to blockAllocator(), seen below. Will iterate through the free space
   map to find free *consecutive* blocks */
int getBlocks(int numOfBlocks){


    // To check if the requested number of blocks is valid. If not, return an error
	if (numOfBlocks < 1)
	{
		return -2;
	}



    // A variable to track which byte in the free space map we are iterating through
	unsigned char binaryNumber = 0;  

	/* A variable that will initially be set to 0x80 and will be halved in order
       to shift the bit with value 1 to the right. The & operator will be applied
       between "binaryNumber" and "value" so that I can check whether the current 
       bit is 1 or 0 */
    int value = 0; 

    /* To store the result when the & operator is applied to "binaryNumber" and
       "value" */
	int result = 0;

    // To track how many consecutive bits have the value of 1
	int numOfConsecutive = 0;

    /* To store the starting block number of the bit with a value of 1 */
	int startingBlockNumber = 0;




    // To iterate through each byte in the free space map
	for (int i = 0; i < volumeControlBlock->numOfTotalBlocks; i++){
		
        // To get the next byte of the free space map
        binaryNumber = freeSpaceMap[i];
        // To set "value" to 0b1000 0000
		value = 0x80;


        // To iterate through the byte to get individual bits
		for (int j = 0; j < 8; j++)
		{
            
            // To store the result of "binaryNumber & value" for the following checks
			result = binaryNumber & value;
			
			/* To check if the bit is 0. If so, reset the number of consecutive
               bits with a value of 1 to 0 and the starting block number to 0 */
			if (result == 0){
				
				numOfConsecutive = 0;
				startingBlockNumber = 0;
				
            // Otherwise, to check if we landed a value of 1    
			} else if (result == value){
                
                // To update count of consecutive bits with a value of 1
				numOfConsecutive++;
				
                // Update the starting block number where we found it
				if (numOfConsecutive == 1){
					
					startingBlockNumber = j + (8 * i);
				
				}
				
			}


            /* To check if the number of consecutive bits of value 1 is equal to
               the number of free blocks we wanted. If so, clear/mark those bits
               as used, reduce the count of the number of free blocks (which is 
               information contained in VCB), and return the starting block number */
			if (numOfConsecutive == numOfBlocks){

            
                /* To return the starting block number of the first free block
                   in the chain */
				return startingBlockNumber;
			
			
			}

			
            // To shift the bit with value of 1 to the right
			value = value / 2;
		}
		
		

	}

    // If there aren't enough free blocks to give to the caller
	return -1;

}

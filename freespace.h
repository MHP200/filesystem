/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: freespace.h
*
* Description: This is the header file for the free space management system
*              which will allow us to manage free space.
*
**************************************************************/
#include "volumecontrol.h"


// Our free space map to reference throughout the file system 
extern unsigned char *freeSpaceMap;




/* A function to initialize the free space map, where we allocate space for the 
   free space map, initialize the map with bit values of 1 (1 = free, 0 = used),
   mark the blocks holding this map as well as the volume control block as used, 
   and where we initialize certain fields of the volume control block such as
   the lenght of the map and its starting block number */
void freeSpaceInit();


/* A function to initialize the free space map with bit values of 1, where 1 = 
   free and 0 = used. Helper function for freeSpaceInit(). */
void initializeMap();

/* A function that will be used to alter a bit, specifically setting a bit to a 
   value of 1, indicating a free block, in the free space map. Can act as a
   helper function for initializeMap(), seen above, and freeBlocks(), seen below */
void setBit(int position);

/* A function that will be used to alter a bit, specifically setting a bit to a 
   value of 0, indicating a used block, in the free space map. Can act as a
   helper function for clearMultipleBits(), seen below  */ 
void clearBit(int position);


/* A function that will be used to alter multiple bits, specifically setting a 
   consecutive number of bits to values of 0, where 0 indicates a used block, in
   the free space map. Can act as a helper function for blockAllocator() and 
   freeSpaceInit(). */ 
void clearMultipleBits(int position, int numOfBlocksToClear);


/* A function to give to the caller free consecutive blocks. Can act as a helper 
   function to blockAllocator(), seen below. Will iterate through the free space
   map to find free *consecutive* blocks */
int getBlocks(int numOfBlocks);


/* To count how many consecutuve bits there are with values of 1 past a certain
   position. Will be used as a helper function to blockAllocator(). */
int getNextBit(int position);


/* A function to give the caller the requested amount of free blocks. Will mark 
   blocks as used and returns an array of extents to fulfill our chosen extent-based
   allocation method for directory entries */
struct ExtentNode *blockAllocator(int numOfBlocks, struct ExtentNode arrayOfExtents[NUM_OF_EXTENTS]);


/* A function to free the blocks associated with a directory entry. It sets every
   bit for those extents values of 1, which indicates free (0 = used) */
void freeBlocks(struct ExtentNode arrayOfExtents[NUM_OF_EXTENTS]);


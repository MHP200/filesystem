/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: directories.h
*
* Description: This is the header file for our directory system which provides
*              us functions that allow us to both create and initialize the root
*              directory.
*
**************************************************************/


#ifndef  DIRECTORIES_H
#define  DIRECTORIES_H

#include "freespace.h"




extern struct directoryEntry * rootDirectory;
extern struct directoryEntryCalculations * directoryEntryCalcs;


//Global Variables to store calculations
struct directoryEntryCalculations{

	int totalBytesForEntries;
	int lengthOfDirectory;
	int totalExtraEntries;
	int totalNumOfEntries;
	
} directoryEntryCalculations;



/* Calculates the total number of bytes needed for root dir
* @param totalEntries, the number of bytes needed for root dir
* @param directoryCalcs, a struct which stores the total bytes needed for root dir
*/
int numBytesForEntries(int numEntries,struct directoryEntryCalculations* directoryCalcs);

/* Calculates the total number of blocks needed
* @param directoryCalcs, a struct which stores the total blocks needed for root dir
*/
int numBlocksToAskFreeSpace(struct directoryEntryCalculations* directoryCalcs);

/* Used for debugging purposes to print each element in array of directory entries
 * @param needs the total number of entries in root dir
 */
void debugPrint(int total);


/* Sets each data field in the root directory to its proper value
 * @param directoryCalcs, a struct which stores the necessary variables
 */
void initializeDirFields(struct directoryEntryCalculations *calcs, struct directoryEntry *directory, struct directoryEntry * parentDirectory);

/* Calls all functions outlined above to initialize the root directory
 * @param numEntries, number of entries requested for root dir
 */
struct directoryEntry *initializeDir(int numEntries, struct directoryEntry * parentDirectory);

#endif 

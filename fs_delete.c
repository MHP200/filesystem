/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: fs_delete.c
*
* Description: this function will marking the block unused and asking the free space to free the blocks
**************************************************************/



#include "mfs.h"
#include "parsepath.h"

//removes a file
int fs_delete(char* filename) {
    
    // To create space to store the return values of the parsepath() function
    struct directoryDetails *returnValues = malloc(sizeof(directoryDetails));

    // To parse the pathname to delete a file
    parsePath(filename, currentDirectory, returnValues);

    // To point to the directory we want access to to delete a file within that directory
    struct directoryEntry *parentDir = returnValues->ptrToDirectory;

    // To store the index of the directory entry for the given filename in the current directory
    int index = locateEntry(parentDir, returnValues->finalElementName);
    
    // if the file is not found
    if (index == -1){ 
        
        // To free the space allocated to the returnValues of parsepath()
        free(returnValues);
        
        // return some error if file not found
        return -1;
    } 
    
    
    /* To free the blocks associated with the file by calling the freeBlocks function with 
    the extents array of the directory entry */
    freeBlocks(parentDir[index].extents); 

        
    // To write the free space map to disk
    LBAwrite(freeSpaceMap, volumeControlBlock->lengthOfFreeSpaceMap, 
             volumeControlBlock->startingBlockOfFreeSpace);


    // To reset the file name to the null terminator
    strcpy(parentDir[index].fileName, "\0"); 
    // To mark the entry as used
    parentDir[index].used = false;


    // To write the parentDir to disk
    extentsWrite(parentDir, 0);
    

    // To free the space allocated to the returnValues of parsepath()
    free(returnValues);


    // To return a success
    return 0; 
}

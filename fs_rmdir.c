/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: fs_rmdir.c
*
* Description: This is the .c file for the directory function fs_rmdir(). In 
*              here, we'll be defining and implementing it such that the user
*              will be able to remove a directory.
*
**************************************************************/



#include "mfs.h"
#include "parsepath.h"




int fs_rmdir(const char *pathname){

    // To create space to store the return values of the parsepath() function
    struct directoryDetails *returnValues = malloc(sizeof(directoryDetails));

    // To parse the pathname to delete a directory 
    parsePath(pathname, currentDirectory, returnValues);

    // To check for an invalid path
    if (returnValues == NULL){
        return -1;
    }

    /* To point to the directory we want access to so that we can delete a directory
       within that directory */
    struct directoryEntry *parentDir = returnValues->ptrToDirectory;

    // To initialize a pointer that will eventually point to a newly loaded directory
    struct directoryEntry *childDirectory = NULL;


    



    // To store the index in the parent directory of the directory we want deleted
    int index = locateEntry(parentDir, returnValues->finalElementName);
    
    // To check for an invalid path name
    if (index == -1){
        
        free(childDirectory);

        return -1;
    
    }

    /* To load that child directory so that we can iterate through it, making sure
       it's completely empty */
    childDirectory = loadDirectory(index, parentDir);





    // To store/keep track of how many empty directory entries we've encountered
    int count1 = 0;
    /* To iterate through the child directory, the one we want deleted, to make sure
       it is completely empty */
    for (int i = 2; i < childDirectory[0].numOfEntries; i++){

        if (childDirectory[i].used == false){ // If empty, increment

            count1 = count1 + 1;
            
        }

    }
    /* To check if the counter is equal to the number of entries past "." and 
       "..". If it is, which means that everything except for "." and ".." are
       empty, we can now remove the directory */
    if (count1 == (childDirectory[0].numOfEntries - 2)){

        // To free the blocks associated with the child directory
        freeBlocks(parentDir[index].extents);
        
        // To write the free space map to disk
        LBAwrite(freeSpaceMap, volumeControlBlock->lengthOfFreeSpaceMap, 
                 volumeControlBlock->startingBlockOfFreeSpace);

        // To set that deleted child directory's filename to the null terminator 
        parentDir[index].fileName[0] = '\0';
        // To mark it unused
        parentDir[index].used = false;
        
        // To write the parentDir to disk
        extentsWrite(parentDir, 0);

        // To free the space allocated to load the child directory into memory
        free(childDirectory);

        return 0;

    }


    // Otherwise, if we can't remove it, say because it's not empty, return an error
    return -1;

}
/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: fs_stat.c
*
* Description: This is the .c file for the directory function fs_stat(). In 
*              here, we'll be defining and implementing it such that the user
*              will be able to attain directory entry details.
*
**************************************************************/



#include "mfs.h"
#include "parsepath.h"

int fs_stat(const char *path, struct fs_stat *buf){


    // To create space to store the return values of the parsepath() function
    struct directoryDetails *returnValues = malloc(sizeof(directoryDetails));

    // To parse the pathname to eventually store directory entry information 
    parsePath(path, currentDirectory, returnValues);

    // To check for an invalid path
    if (returnValues == NULL){
        return -1;
    }

    // To point to the directory we want access to to eventually access a target directory entry
    struct directoryEntry *parentDir = returnValues->ptrToDirectory;

    /* To get the index in the parentDir of the directory entry that contains
       the final name that we want to check the file type of */
    int index = locateEntry(parentDir, returnValues->finalElementName);


    // To check if the target directory entry even exists
    if (index == -1){

        return -1;

    }


    // To store the number of blocks in the directory entry
    int numOfBlocks = 0;
    // To iterate through the table of extents to count the number of allocated blocks
    for (int i = 0; i < NUM_OF_EXTENTS; i++){


        /* If the extent has a startingBlockNumber of 0 then break.
        * A startingBlockNumber of 0 means you have reached the end
        * of extents. 
        */
        if (parentDir[index].extents[i].blockNum == 0){
            break;
        }

        // To increment the number of allocated blocks
        numOfBlocks = numOfBlocks + parentDir[index].extents[i].count;
        
    }


    /* The next few lines are written to fill the values in the instance of the 
       fs_stat struct */
    buf->st_size = parentDir[index].size;
    buf->st_blocks = numOfBlocks;
    //buf->
    buf->st_createtime = parentDir[index].dateCreated;
    buf->st_modtime = parentDir[index].dateModified;
    buf->st_accesstime = parentDir[index].dateCreated;


    return 0;

}
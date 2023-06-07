/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: fs_move.c
*
* Description: This is the .c file for the directory function fs_move(). It is not
*              found in mfs.h, rather it's a function we're creating to implement the
*              the "mv" command found in fsshell.c, allowing users to move
*              files/directories to another directory. This file is where we create
*              the directory function necessary to do so.
*
**************************************************************/


#include "fs_move.h"

int fs_move(const char *pathname1, const char *pathname2){


    // To create space to store the return values of the parsepath() function
    struct directoryDetails *returnValues1 = malloc(sizeof(directoryDetails));

    // To create space to store the return values of the parsepath() function
    struct directoryDetails *returnValues2 = malloc(sizeof(directoryDetails));

    // To parse the pathname to reach a source directory 
    parsePath(pathname1, currentDirectory, returnValues1);

    // To check for an invalid path
    if (returnValues1 == NULL){
        return -1;
    }

    
    // To point to the directory that we want a file moved from
    struct directoryEntry *sourceDirectory = returnValues1->ptrToDirectory;

    
    /* To store the index of an already existing file/directory in the destination directory */
    int indexOfExistingFile = 0;
    /* To store the index of the source's file/directory we want moved */
    int indexOfSourceFile = locateEntry(sourceDirectory, returnValues1->finalElementName);
    // To check if that name even exists in the source
    if (indexOfSourceFile == -1){
        
        return -1;
    }

    // To parse the pathname to reach a destination/target directory 
    parsePath(pathname2, currentDirectory, returnValues2);

    // To check for an invalid path
    if (returnValues2 == NULL){
        return -1;
    }
    
    
    // To point to the directory that we want a file moved from
    struct directoryEntry *destinationDirectory = returnValues2->ptrToDirectory;

    

    /* To iterate across the entire destination directory to see if there is already
       an entry with the name we want to give the soon-to-be moved file/directory.
       If so, get its index */
    for (int i = 0; i < destinationDirectory[0].numOfEntries; i++){

        
        if ((strcmp(destinationDirectory[i].fileName, returnValues1->finalElementName) == 0) 
            && destinationDirectory[i].used == true){
            
            indexOfExistingFile = i;
        }

    }



    // To check if it is the case where the destination has a file/dir of that source's name
    if (indexOfExistingFile != 0){
        

        /* To copy the source file's/directory's name into the directory entry of
           the destinationDirectory */
        strcpy(destinationDirectory[indexOfExistingFile].fileName, returnValues1->finalElementName);
        /* The next ~10 lines are written to copy the source file's/directory's values,
           such as size and extents, into the directory entry of the destination directory */
        destinationDirectory[indexOfExistingFile].size = sourceDirectory[indexOfSourceFile].size;
        destinationDirectory[indexOfExistingFile].numOfEntries = sourceDirectory[indexOfSourceFile].numOfEntries;
        destinationDirectory[indexOfExistingFile].dateCreated = sourceDirectory[indexOfSourceFile].dateCreated;
        destinationDirectory[indexOfExistingFile].dateModified = sourceDirectory[indexOfSourceFile].dateModified;
        destinationDirectory[indexOfExistingFile].fileType = sourceDirectory[indexOfSourceFile].fileType;
        destinationDirectory[indexOfExistingFile].used = true;
        for (int j = 0; j < NUM_OF_EXTENTS; j++){
            destinationDirectory[indexOfExistingFile].extents[j].blockNum = sourceDirectory[indexOfSourceFile].extents[j].blockNum;
            destinationDirectory[indexOfExistingFile].extents[j].count = sourceDirectory[indexOfSourceFile].extents[j].count;
        }

        // To reset the file name to the null terminator
        sourceDirectory[indexOfSourceFile].fileName[0] = '\0';
        // To mark the entry as used
        sourceDirectory[indexOfSourceFile].used = false;

        // To write the destination directory to disk
        extentsWrite(destinationDirectory, 0);

        // To write the source directory to disk
        extentsWrite(sourceDirectory, 0);

        // To free the space allocated to the returnValues1 of parsepath()
        free(returnValues1);

        // To free the space allocated to the returnValues2 of parsepath()
        free(returnValues2);

        // // To free the spaces allocated for the two directories
        // free(destinationDirectory);
        // free(sourceDirectory);
            

        // To return a value indicating success
        return 0;

    }



    /* If there doesn't exist such a name in the destination, we can just create
       a new entry */ 
    for (int i = 2; i < destinationDirectory[0].numOfEntries; i++){{

        if (destinationDirectory[i].used == false){
            
            /* To copy the source file's/directory's name into the directory entry of
               the destinationDirectory */
            strcpy(destinationDirectory[i].fileName, returnValues2->finalElementName);
            /* The next ~10 lines are written to copy the source file's/directory's values,
               such as size and extents, into the directory entry of the destination directory */
            destinationDirectory[i].size = sourceDirectory[indexOfSourceFile].size;
            destinationDirectory[i].numOfEntries = sourceDirectory[indexOfSourceFile].numOfEntries;
            destinationDirectory[i].dateCreated = sourceDirectory[indexOfSourceFile].dateCreated;
            destinationDirectory[i].dateModified = sourceDirectory[indexOfSourceFile].dateModified;
            destinationDirectory[i].fileType = sourceDirectory[indexOfSourceFile].fileType;
            destinationDirectory[i].used = true;
            for (int j = 0; j < NUM_OF_EXTENTS; j++){
                destinationDirectory[i].extents[j].blockNum = sourceDirectory[indexOfSourceFile].extents[j].blockNum;
                destinationDirectory[i].extents[j].count = sourceDirectory[indexOfSourceFile].extents[j].count;
            }

            // To reset the file name of the source to the null terminator
            sourceDirectory[indexOfSourceFile].fileName[0] = '\0';
            // To mark the entry of the source as used
            sourceDirectory[indexOfSourceFile].used = false;

            // To write the destination directory to disk
            extentsWrite(destinationDirectory, 0);

            // To write the source directory to disk
            extentsWrite(sourceDirectory, 0);

            // To free the space allocated to the returnValues1 of parsepath()
            free(returnValues1);

            // To free the space allocated to the returnValues2 of parsepath()
            free(returnValues2);

            // // To free the spaces allocated for the two directories
            // free(destinationDirectory);
            // free(sourceDirectory);
            

            // To return a value indicating success
            return 0;

        }
    }

    }


    
    // This is reached when an error has occurred
    return -1;


}
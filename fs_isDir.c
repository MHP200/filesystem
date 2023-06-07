/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: fs_isDir.c
*
* Description: The fs_isDir function takes a char pointer pathname as input and returns an int value. 
* The function checks whether the specified path is a directory or not. 
* If the path is a directory, the function returns 1, otherwise it returns 0.
*
**************************************************************/



#include "mfs.h"
#include "parsepath.h"

//return 1 if directory, 0 otherwise
int fs_isDir(char * pathname) { 
 
    
    // To create space to store the return values of the parsepath() function
    struct directoryDetails *returnValues = malloc(sizeof(directoryDetails));

    // To parse the pathname to check if the target is a directory
    parsePath(pathname, currentDirectory, returnValues);

    // To check for an invalid path
    if (returnValues == NULL){
        return -1;
    }

    /* To store the parent directory of the directory entry that we want to check
      the file type of */
    struct directoryEntry *parentDir = returnValues->ptrToDirectory;

    /* To get the index in the parentDir of the directory entry that contains
       the final name that we want to check the file type of */
    int index = locateEntry(parentDir, returnValues->finalElementName);


    // To check if the target directory entry even exists
    if (index == -1){

        return -1;

    }



    
    if (parentDir[index].fileType == 0) { // If the fileType field of the entry is 0, indicating a directory

        // To free the space allocated to the returnValues of parsepath()
        free(returnValues);

        return 1; // Return 1 to indicate that it is a directory

    } else { // Otherwise, if the fileType field is not 0 (i.e. it is 1, indicating a file)

        // To free the space allocated to the returnValues of parsepath()
        free(returnValues);

        return 0; // Return 0 to indicate that it is not a directory (i.e. it is a file)

    }

}
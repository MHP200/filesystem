/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: fs_isFile.c
*
* Description: The fs_isFile function takes a char pointer filename as input and returns an int value. 
* The function checks whether the specified file is a regular file and not a directory or symbolic link. 
**************************************************************/

#include "mfs.h"
#include "parsepath.h"

int fs_isFile(char * filename){
    
    int isDir = fs_isDir(filename);    
    
    // If the given pathname refers to a directory, return 0 because it is not a file
    if (isDir == 1) {
     
        return 0;
    
    } else if (isDir == 0) { // If it's 0, then it's a file

        return 1;
    
    } else {

        return -1; // Otherwise, it's neither one, in fact, it may be an invalid path
    
    }
    
}
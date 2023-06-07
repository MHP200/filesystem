/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: fs_setcwd.c
*
* Description: This is the .c file for the directory function fs_getcwd(). In 
*              here, we'll be defining and implementing it such that we are
*              getting the current working directory.
**************************************************************/





#include "mfs.h"
#include "parsepath.h"


char * fs_getcwd(char *pathname, size_t size){    
    // Check if absolutePathOfCurrentDir is initilized
    // If not, initilize the it
    if(absPathOfCurrentDir == NULL){
        absPathOfCurrentDir = malloc(sizeof(absolutePathInfo));        
        absPathOfCurrentDir->numOfTokensInPath = 0;
        // Initialize the path tokens
        for(int i=0;i<PATH_DEPTH;i++){
            strcpy(absPathOfCurrentDir->pathTokens[i],"");
        }
    }

    char tempString[PATH_DEPTH* (NAME_LENGTH+1)];
    int offset = 0; // To track the offset in tempString for formatting the absolute path
    // The primary path is "/"
    strcpy(tempString,"/");
    offset ++;
    for(int i =0;i<absPathOfCurrentDir->numOfTokensInPath;i++){
        // To avoid double slash at the beginning of path
        if(i != 0){
            memcpy(tempString+offset,"/",1);
            offset ++;
        }
        // Append the tokens to the tempString
        memcpy(tempString+offset,absPathOfCurrentDir->pathTokens[i],
               strlen(absPathOfCurrentDir->pathTokens[i]));
        offset += strlen(absPathOfCurrentDir->pathTokens[i]);
    }

    // To copy the path to the caller's buffer
    memcpy(pathname,tempString,size);
    return pathname;
}
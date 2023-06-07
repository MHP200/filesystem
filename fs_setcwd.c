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
* Description: This is the .c file for the directory function fs_setcwd(). In 
*              here, we'll be defining and implementing it such that we are
*              setting the current working directory.
**************************************************************/



 #include "mfs.h"
 #include "parsepath.h"



struct directoryEntry *currentDirectory;
struct absolutePathInfo * absPathOfCurrentDir;

int fs_setcwd(char *pathname){
    // To parse and try to obtain the details of the directory holding the last element in pathname
    struct directoryDetails * parseResult;
    parseResult = malloc(sizeof(struct directoryDetails));
    parseResult = parsePath(pathname, currentDirectory,parseResult);

    // To check if the path is valid
    // The return value is NULL, the middle path is invalid(either directory entry is not found 
    //  or is not a directory), return -1
    if(parseResult == NULL){
        return -1; 
    }

    // check if the last element is a directory
    int index;
    index = locateEntry(parseResult->ptrToDirectory,parseResult->finalElementName);
    // if the last element is not a directory, do not access it, return -1
    if(index == -1){
        free(parseResult); // Free unused resource
        return -1;
    }

    // The path is valid and the last element is a directory, so we can now update
    // the currentDirectory point to the directory which is pointed by the last element
    currentDirectory = loadDirectory(index,parseResult->ptrToDirectory);

    // Begins to modify the absolutePathOfCurrentDir
    // First, to tokenize the path
    char tokenArray[PATH_DEPTH][NAME_LENGTH];
    int numOfTokens = tokenizationOfPath(pathname,tokenArray);

    // Check if absolutePathOfCurrentDir is initilized
    // If not, initilize the it
    if(absPathOfCurrentDir == NULL){
        absPathOfCurrentDir = malloc(sizeof(absolutePathInfo));        
        // Set the number of tokens of the absolute path to 0
        absPathOfCurrentDir->numOfTokensInPath = 0;
        // Initialize the path tokens
        for(int i=0;i<PATH_DEPTH;i++){
            strcpy(absPathOfCurrentDir->pathTokens[i],"");
        }
    }

    // If the first character of pathname is "/", it's an absolute path.
    // Else, the path is a relative path.
    if(pathname[0] == '/'){
        // Empty the previous path's token array
        for(int i=0;i<PATH_DEPTH;i++){
            strcpy(absPathOfCurrentDir->pathTokens[i],"");
        }
        // Reset the number of tokens of the path to 0
        absPathOfCurrentDir->numOfTokensInPath = 0;
    }   

    // The new tokens will apend to the path token array
    for(int i =0;i<numOfTokens; i++){
        // "." will not make any change to the previous path
        if(strcmp(tokenArray[i],".")==0){
            continue;
        }
        // ".." will remove the last token from token array of absolute path if there is 
        // at least one token in the array
        if(strcmp(tokenArray[i],"..")==0){
            if(absPathOfCurrentDir->numOfTokensInPath > 0){
                // Set the last token in pathTokens to empty
                strcpy(absPathOfCurrentDir->pathTokens[absPathOfCurrentDir->numOfTokensInPath -1],"");
                // Update the number of tokens in array
                absPathOfCurrentDir->numOfTokensInPath --;
            }
            continue;
        }
        // If the path has reach the limit of PATH_DEPTH, exit
        if(absPathOfCurrentDir->numOfTokensInPath == PATH_DEPTH){
            free(parseResult); // Free unused resource
            return 0;
        }
        // Copy the tokens from new tokens array into path's token array
        strcpy(absPathOfCurrentDir->pathTokens[absPathOfCurrentDir->numOfTokensInPath],tokenArray[i]);
        // Update the number of tokens in array
        absPathOfCurrentDir->numOfTokensInPath ++;
    }

    free(parseResult); // Free unused resource
    return 0;
}
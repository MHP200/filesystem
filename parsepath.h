/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: parsepath.h
*
* Description: This is the header file for the parse path function containing
*              the helper functions necessary to fully implement parse path. 
*
**************************************************************/




#include "directories.h"

#define PATH_DEPTH 30
#define NAME_LENGTH 30
#define EXTRA_BLOCKS_FOR_NEW_DIR 3




// To keep the currentDirectory loaded into memory
extern struct directoryEntry *currentDirectory;

// To keep track of the absolute path name for the current directory
extern struct absolutePathInfo * absPathOfCurrentDir;




// To store the info of the absolute path
struct absolutePathInfo{
   // To store the tokens of the absolute path
   char pathTokens[PATH_DEPTH][NAME_LENGTH];

   // To store the number of tokens in the path
   int numOfTokensInPath;

}absolutePathInfo;




// Return values to store the details from the search
struct directoryDetails
{
    // To store the index of the directory entry in the directory
    int indexOfDirectoryEntry;

    // To store the pointer to the current directory to keep it loaded
    struct directoryEntry *ptrToDirectory;
    
    /* To check if the final element of the path is either a file or a directory.
       A value of 0 = true, it is a directory, and a value of 1 = false, 
       indicating that it's a file */
    int isDirectory;

    // The name of the final element of the pathname
    char *finalElementName;


} directoryDetails;





// A function to traverse the loaded directory to find the next target directory
// - Mahdi's part
// - Traverse the array of directory entries
// - Return index of entry if found. Return -1 if not found
int locateEntry(struct directoryEntry *directoryEntry, char *nextName);

// A function to load the located directory
// - Amit's part
// - Malloc space for a directory
// - Traverse the directory entry's array of extents and LBAread() the blocks
//   into the malloces space
// - Make sure that if you encounter an extent whose startingBlockNumber is 0,
//   break out of the loop
struct directoryEntry *loadDirectory(int index, struct directoryEntry * directory);


// A function to parse/tokenize the path into individual elements
// - Jinwie's part
// - Tokenize path name using strtok_r
// - Return array of strings representing the parts of the path
int tokenizationOfPath(const char *path, char tokenArray[][NAME_LENGTH]);


/* A function to integrate all of the above helper functions together in order
   to fully parse a pathname to perform various directory functions */
struct directoryDetails *parsePath(const char *path, struct directoryEntry *parentDirectory, struct directoryDetails *returnValues);
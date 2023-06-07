/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: fs_openrdclose
*
* Description: Functions to open, read, and close a directory
*
**************************************************************/
#include "mfs.h"
#include "parsepath.h"
//Opens a directory given its path
//@ param path to directory 
fdDir * fs_opendir(const char *pathname){
    
    struct directoryDetails returnValues;

    //loads a directory based on a given pathname
    parsePath(pathname, currentDirectory, &returnValues);

    //Set the current directory to the new current directory given by parsepath
    fdDir * dirp=malloc(sizeof(fdDir));

   // Set the fdDir fields to their respective values
    dirp->dirEntryPosition=0;
    dirp->directoryStartLocation=currentDirectory->extents[dirp->dirEntryPosition].blockNum;
    strcpy(dirp->dirEntryInfo.d_name,currentDirectory[dirp->dirEntryPosition].fileName);
    dirp->dirEntryInfo.fileType=(unsigned char)currentDirectory[dirp->dirEntryPosition].fileType;
    return dirp;
  
}
//Reads a single directory and returns its info 
//@param fdDir pointer
struct fs_diriteminfo *fs_readdir(fdDir *dirp){
    //Checks if itearated throught all entries in currentDirectory
    if(dirp == NULL || dirp->dirEntryPosition>currentDirectory->numOfEntries)
    {
        return NULL;
    }
    //If the name of the directory starts with a null byte increment the dirEntry position
    //This makes sure that when read dir is called only the entries with data will be passed back to the user
    //printf("current dir=%s strcmp(filename,null)=%d\n",currentDirectory[dirp->dirEntryPosition].fileName,strcmp(currentDirectory[dirp->dirEntryPosition].fileName,"\0")==0);
    while(dirp->dirEntryPosition < currentDirectory->numOfEntries && strcmp(currentDirectory[dirp->dirEntryPosition].fileName,"\0")==0)
    {
        
        dirp->dirEntryPosition++;

    }
    //Checks if itearated throught all entries in currentDirectory
    if( dirp->dirEntryPosition >= currentDirectory->numOfEntries ) {
        return NULL;
    }

    //Set the fdDir fields to their respective values
    //Values are copied from the directoryEntry structure
    strcpy(dirp->dirEntryInfo.d_name,currentDirectory[dirp->dirEntryPosition].fileName);
    dirp->dirEntryInfo.fileType=(unsigned char)currentDirectory[dirp->dirEntryPosition].fileType;
    dirp->dirEntryPosition++;
    
    return &(dirp->dirEntryInfo);
    
}
//Frees all pointers used in open and read
//@param takes a fdDir pointer
int fs_closedir(fdDir *dirp){ 
    if(dirp!=NULL)
    {
        //Frees the fd pointer returned by fs_opendir()
        free(dirp);
        dirp=NULL;
        return 0;
    }  
    return 0;
}
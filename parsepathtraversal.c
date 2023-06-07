/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: parsepathtraversal.c
*
* Description: This is the .c file for the parse path function containing
*              the helper function to traverse the loaded directory to find the next target directory
**************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parsepath.h"



// - Return index of entry if found. Return -1 if not found
int locateEntry(struct directoryEntry *directoryEntry, char *nextName) {
    
    //    /home/jinwei/foo/myFile
    
    
    // Iterate over the directory entries array using a for loop
    for (int i = 0; i < directoryEntry[0].numOfEntries; i++) {
        
        // Compare the file name in the current directory entry to the target name
        // strcmp() compares two strings character by character.
        if ((strcmp(directoryEntry[i].fileName, nextName) == 0) && 
            directoryEntry[i].fileType == 0) {
        
            // Return the index of the matching entry
            // return index;
            return i;
        
        }
        
    }
    
    // Return -1 if no matching entry was found
    return -1;

}
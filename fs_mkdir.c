/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: fs_mkdir.c
*
* Description: This is the .c file for the directory function fs_mkdir(). In 
*              here, we'll be defining and implementing it such that the user
*              will be able to create a new directory.
*
**************************************************************/

#include "mfs.h"
#include "parsepath.h"


int fs_mkdir(const char *pathname, mode_t mode){


    // To create space to store the return values of the parsepath() function
    struct directoryDetails *returnValues = malloc(sizeof(directoryDetails));

    // To parse the pathname to make a new directory 
    parsePath(pathname, currentDirectory, returnValues);

    // To check for an invalid path
    if (returnValues == NULL){
        return -1;
    }

    // To point to the directory in which we want to create a new directory
    struct directoryEntry *parentDir = returnValues->ptrToDirectory;

    // To initialize a pointer that will eventually point to a newly created directory
    struct directoryEntry *newDirectory = NULL;

    
    
    

    

    /* To iterate across the entire current/"parentDir" directory to see if the
       there is already an entry with the name we want to give the new directory
       ("finalElementName" stored in "returnValues"). If so, return an error. */
    for (int i = 2; i < parentDir[0].numOfEntries; i++){

        
        if ((strcmp(parentDir[i].fileName, returnValues->finalElementName) == 0) 
            && parentDir[i].used == true){
            
            return -1;
        }

    }

    

    
    /* To iterate through the directory to see if there's an empty/unused index to
       use to create a new directory. If we find one, we initialize this directory 
       entry with values stored in the "." entry of the new/child directory */
    for (int i = 2; i < parentDir[0].numOfEntries; i++){
        
        // To check if we've found an empty directory entry
        if (parentDir[i].used == false){
            
            // To initialize a new directory
            newDirectory = initializeDir(NUM_ENTRIES, parentDir);
            /* To copy the new/child directory's name into the directory entry of
               the parentDir */
            strcpy(parentDir[i].fileName, returnValues->finalElementName);
            /* The next ~10 lines are written to copy the new/child directory's values,
               such as size and extents, into the directory entry of the parentDir */
            parentDir[i].size = newDirectory[0].size;
            parentDir[i].numOfEntries = newDirectory[0].numOfEntries;
            parentDir[i].dateCreated = newDirectory[0].dateCreated;
            parentDir[i].dateModified = newDirectory[0].dateModified;
            parentDir[i].fileType = newDirectory[0].fileType;
            parentDir[i].used = true;
            for (int j = 0; j < NUM_OF_EXTENTS; j++){
                parentDir[i].extents[j].blockNum = newDirectory[0].extents[j].blockNum;
                parentDir[i].extents[j].count = newDirectory[0].extents[j].count;
            }
            // To write the parentDir to disk
            extentsWrite(parentDir, 0);

            // To write the newDirectory to disk
            extentsWrite(newDirectory, 0);

            // To free the space allocated to the returnValues of parsepath()
            free(returnValues);

            // // To free the spaces allocated for the two directories
            // free(parentDir);
            // free(newDirectory);
            

            // To return a value indicating success
            return 0;
        }
    
    }



    /* If we can't find any space in our parent directory to create a new directory,
       we must now begin allocating more space to store more entries. We start by
       getting "EXTRA_BLOCKS_FOR_NEW_DIR" number of contiguous blocks to store more
       directory entries */
    int startingBlockNum = getBlocks(EXTRA_BLOCKS_FOR_NEW_DIR);
    /* Next, we want to calculate how many additional entries we'll be able to store */
    int numOfAdditionalEntries = (EXTRA_BLOCKS_FOR_NEW_DIR * volumeControlBlock->sizeOfBlocks) / sizeof(directoryEntry);
    /* After that, we want to calculate how many additional bytes we'll need to be 
       able to reallocate the parent directory to store more entries */
    int numOfAdditionalBytes = EXTRA_BLOCKS_FOR_NEW_DIR * volumeControlBlock->sizeOfBlocks;
    /* To store the number of entries we previously had. Will be used to iterate
       across the child directories whose ".." entries need to be updated to reflect 
       the changes in the values stored of the parent's "." entry, such as size,
       "numOfEntries", etc. */
    int oldNumberOfEntries = parentDir[0].numOfEntries;
    /* Since we are allocating more space, and since the first entry in the newly
       allocated space will be unused, we can store the index in the parent directory
       of this soon-to-be created entry, which just so happens to have a value equal
       to the value of the former number of entries */
    int latestIndexIntoParentDir = oldNumberOfEntries;
    /* To declare a pointer that will eventually point to a newly loaded child directory
       for the purpose of accessing that child's ".." entry to update it's extents
       table once the parent has been updated */
    struct directoryEntry *childDirectory = NULL;




    /* To iterate through the parent directory's extents to see if we can find free
       blocks adjacent to an extent range. If so, instead of creating a new extent
       entry, add to the count of the extent with adjacent free blocks. This 'for' 
       loop is reached if we can't find an empty/unused directory entry */
    for (int i = 0; i < NUM_OF_EXTENTS; i++){
        
        // If free blocks adjacent to an extent range exist
        if (startingBlockNum == (parentDir[0].extents[i].blockNum + 
                                 parentDir[0].extents[i].count)){
            
            
            // To increment the count of that extent
            parentDir[0].extents[i].count = parentDir[0].extents[i].count + EXTRA_BLOCKS_FOR_NEW_DIR;
            /* To reallocate space for the parent directory as we want to expand it
               to fit more entries */
            parentDir = realloc(parentDir, parentDir[0].size + numOfAdditionalBytes);
            // To add to its current size
            parentDir[0].size = parentDir[0].size + numOfAdditionalBytes;           
            // To add to its current number of entries
            parentDir[0].numOfEntries = parentDir[0].numOfEntries + numOfAdditionalEntries;
            // To mark bits as used
            clearMultipleBits(startingBlockNum, EXTRA_BLOCKS_FOR_NEW_DIR);
            // To write the free space map to disk
            LBAwrite(freeSpaceMap, volumeControlBlock->lengthOfFreeSpaceMap, 
                     volumeControlBlock->startingBlockOfFreeSpace);



            // To call the directory system to create a new directory
            newDirectory = initializeDir(NUM_ENTRIES, parentDir);
            /* The next ~12 lines are written to store into the index of the parent
               directory the metadata about the new directory */
            strcpy(parentDir[latestIndexIntoParentDir].fileName, returnValues->finalElementName);
            parentDir[latestIndexIntoParentDir].size = newDirectory[0].size;
            parentDir[latestIndexIntoParentDir].numOfEntries = newDirectory[0].numOfEntries;
            parentDir[latestIndexIntoParentDir].dateCreated = newDirectory[0].dateCreated;
            parentDir[latestIndexIntoParentDir].dateModified = newDirectory[0].dateModified;
            parentDir[latestIndexIntoParentDir].fileType = newDirectory[0].fileType;
            parentDir[latestIndexIntoParentDir].used = true;
            for (int j = 0; j < NUM_OF_EXTENTS; j++){
                parentDir[latestIndexIntoParentDir].extents[j].blockNum = newDirectory[0].extents[j].blockNum;
                parentDir[latestIndexIntoParentDir].extents[j].count = newDirectory[0].extents[j].count;
            }
            // To write the newDirectory to disk
            extentsWrite(newDirectory, 0);


            /* The next ~12 lines are written to load each child of the parent directory
               into memory and update each of those children's ".." entries, since
               the parent is now modified and since ".." entries store info about their
               parent. This will allow a proper "cd" command to take place */
            for (int j = 2; j < oldNumberOfEntries; j++){

                childDirectory = loadDirectory(j, parentDir);
                // Accessing ".." entry, hence the '1' in brackets
                childDirectory[1].extents[i].count = parentDir[0].extents[i].count;
                childDirectory[1].size = parentDir[0].size;
                childDirectory[1].numOfEntries = parentDir[0].numOfEntries;

                // To write the childDirectory to disk
                extentsWrite(childDirectory, 0);
                
                // To free the loaded child directory since we're moving on to the next one
                free(childDirectory);
            }

            
            // To write the parentDir to disk
            extentsWrite(parentDir, 0);
            

            // To free the space allocated for the return values of parse path
            free(returnValues);

            // // To free the spaces allocated for the two directories
            // free(parentDir);
            // free(newDirectory);

            return 0;
        }

    }



    
    
    /* To iterate through the parent directory's extents to see if we can find an
       empty extent entry to fill. This 'for' loop is reached if 1) we can't find
       an empty/unused directory entry and 2) if the above 'for' loop is not reached,
       that is, we can't find free blocks adjacent to an extent range */
    for (int i = 0; i < NUM_OF_EXTENTS; i++){

        // To check if there is an empty extent entry
        if (parentDir[0].extents[i].blockNum == 0){

            /* To reallocate space for the parent directory as we want to expand it
               to fit more entries */
            parentDir = realloc(parentDir, parentDir[0].size + numOfAdditionalBytes);
            // To add to its current size
            parentDir[0].size = parentDir[0].size + numOfAdditionalBytes;           
            // To add to its current number of entries
            parentDir[0].numOfEntries = parentDir[0].numOfEntries + numOfAdditionalEntries;
            // To mark bits as used
            clearMultipleBits(startingBlockNum, EXTRA_BLOCKS_FOR_NEW_DIR);
            // To write the free space map to disk
            LBAwrite(freeSpaceMap, volumeControlBlock->lengthOfFreeSpaceMap, 
                     volumeControlBlock->startingBlockOfFreeSpace);
            // To fill a new extent entry with a staring block number
            parentDir[0].extents[i].blockNum = startingBlockNum;
            /* To fill a new extent entry with a count of 
               "EXTRA_BLOCKS_FOR_NEW_DIR" number of blocks */
            parentDir[0].extents[i].count = EXTRA_BLOCKS_FOR_NEW_DIR;




            // To call the directory system to create a new directory
            newDirectory = initializeDir(NUM_ENTRIES, parentDir);
            /* The next ~12 lines are written to store into the index of the parent
               directory the metadata about the new directory */
            strcpy(parentDir[latestIndexIntoParentDir].fileName, returnValues->finalElementName);
            parentDir[latestIndexIntoParentDir].size = newDirectory[0].size;
            parentDir[latestIndexIntoParentDir].numOfEntries = newDirectory[0].numOfEntries;
            parentDir[latestIndexIntoParentDir].dateCreated = newDirectory[0].dateCreated;
            parentDir[latestIndexIntoParentDir].dateModified = newDirectory[0].dateModified;
            parentDir[latestIndexIntoParentDir].fileType = newDirectory[0].fileType;
            parentDir[latestIndexIntoParentDir].used = true;
            for (int j = 0; j < NUM_OF_EXTENTS; j++){
                parentDir[latestIndexIntoParentDir].extents[j].blockNum = newDirectory[0].extents[j].blockNum;
                parentDir[latestIndexIntoParentDir].extents[j].count = newDirectory[0].extents[j].count;
            }
            // To write the newDirectory to disk
            extentsWrite(newDirectory, 0);



            /* The next ~12 lines are written to load each child of the parent directory
               into memory and update each of those children's ".." entries, since
               the parent is now modified and since ".." entries store info about their
               parent. This will allow a proper "cd" command to take place */
            for (int j = 2; j < oldNumberOfEntries; j++){

                childDirectory = loadDirectory(j, parentDir);

                // Accessing ".." entry, hence the '1' in brackets
                for (int k = 0; k < NUM_OF_EXTENTS; k++){
                    childDirectory[1].extents[k].blockNum = parentDir[0].extents[k].blockNum;
                    childDirectory[1].extents[k].count = parentDir[0].extents[k].count;
                }

        
                childDirectory[1].size = parentDir[0].size;
                childDirectory[1].numOfEntries = parentDir[0].numOfEntries;


                // To write the childDirectory to disk
                extentsWrite(childDirectory, 0);

                
                // To free the loaded child directory since we're moving on to the next one
                free(childDirectory);

            }

            // To write the parentDir to disk
            extentsWrite(parentDir, 0);
            
            // To free the space allocated for the return values of parse path
            free(returnValues);

            // // To free the spaces allocated for the two directories
            // free(parentDir);
            // free(newDirectory);
            
    
            return 0;

        } 

    }

    
    /* To return an error if we can't allocate space for a new directory, which is
       most likely due to there not being anymore extents we can provide since the
       cap is "NUM_OF_EXTENTS" */
    return -1;
}
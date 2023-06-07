/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: parsepathintegration.c
*
* Description: This is the .c file for the parse path function that fully integrates
*              all of the helper functions necessary to implement parsepath()
**************************************************************/



#include "parsepath.h"




/* A function to integrate all of the helper functions for parsepath() together
   in order to fully parse a pathname to perform various directory functions */
struct directoryDetails *parsePath(const char *path, struct directoryEntry *parentDirectory, struct directoryDetails *returnValues){
   

   /* A variable to store the index of the child directory that is contained within
      the parent directory */
   int indexOfDirectory = 0;

   // A variable to store the number of tokens associated with a pathname
   int numOfTokens = 0;

   // An array to store the tokens collected from the parsing process
   char arrayOfTokens[PATH_DEPTH][NAME_LENGTH];

   /* A variable to store the name of the directory that we want to locate within
      a parent directory */
   char *nextName = NULL;

   /* To store the parent directory, as we want to reset this variable upon every
      iteration of the below 'for' loop in our search */
   struct directoryEntry *parentDir ;
   // If the pathname is absolute path, need to set the current directory back to root directory
   if(path[0] == '/'){

      parentDir = rootDirectory;
   }
   else{
      parentDir = parentDirectory;
   }


   // The number of tokens in the array of tokens. This line also performs the parsing
   numOfTokens = tokenizationOfPath(path, arrayOfTokens);

    
   
   // To iterate through the array of tokens to locate and load directories
   for (int i = 0; i < numOfTokens - 1; i++){
        
        
      // To store the name of the next directory we want to locate
      nextName = arrayOfTokens[i];
        
      /* To store the index of the directory that has been located so that we
         may load it. This line also performs the directory search */
      indexOfDirectory = locateEntry(parentDir, nextName);

      // To check if the path is invalid. If so, return an error, specifically, NULL
      if (indexOfDirectory == -1){
         
         return NULL;
        
      }

      /* To reset the value of "parentDir" for the next search or to store the
         "parentDir" in the struct of return values, "directoryDetails", so that
         we can access it later for future use and other purposes */ 
      parentDir = loadDirectory(indexOfDirectory, parentDir);

   }



   /* To populate the values of the struct of return values, "directoryDetails",
      for future use */
   returnValues->indexOfDirectoryEntry = indexOfDirectory;
   returnValues->ptrToDirectory = parentDir;
   returnValues->finalElementName = arrayOfTokens[numOfTokens - 1];

    


   // To return the struct of return values, "directoryDetails"
   return returnValues;

}

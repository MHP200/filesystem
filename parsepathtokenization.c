/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: parsepathtokenization.c
*
* Description: This is the .c file for the function which tokenizes a path name 
*
**************************************************************/




#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parsepath.h"


/*Will take a path and an array of char[NAME_LENGTH] as argument,
  parse and tokenize the path, then store the tokens into array.
  ****This method will return the number of tokens.
  ****If the path starts with a "/", the first token will be "/".("/" represents root directory)
  For example,
  Path: "."                          ---> Array: ["."]
  Path: ".."                         ---> Array: [".."]
  Path: "./home/student/test/myfile" ---> Array: [".","home","student","test","myfile"]
  Path: "../home/student/test/myfile"---> Array: ["..","home","student","test","myfile"]
  Path: "/home/student/test/myfile"  ---> Array: ["home","student","test","myfile"]
  Path: "test/myfile"                ---> Array: ["test","myfile"]
*/
int tokenizationOfPath(const char *path, char tokenArray[][NAME_LENGTH]){
    // Empty the tokenArray before fill in new tokens
    // To delete path elements from the previous function call.
    for(int i = 0; i<PATH_DEPTH;i++){
        strcpy(tokenArray[i],"");
    }
    // "/" is the main delimiter, " " is to clear out space and empty input
    char delim[] = "/ ";
    // strtok_r can not process an immutable string, the content need to
    // be copied into a temporary char[] 
    char temp[strlen(path)];
    strcpy(temp,path);
    int index = 0;  // To record the number of tokens
    // To tokenize the path
    char * token;
    char * rest = temp;
    while((token = strtok_r(rest, delim, &rest)))
	{   
        strcpy(tokenArray[index],token);
        index++;
	}
    return index;
}
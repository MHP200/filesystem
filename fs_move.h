/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Names: Essa Husary, Amit Bal, Mahdi Hassanpour, Jinwei Tang
* Student IDs: Essa (917014896), Amit (922832232), Mahdi (922278744), Jinwei (922350439)
* GitHub Name: EssaHusary
* Group Name: Geek Squad
* Project: Basic File System
*
* File: fs_move.h
*
* Description: This is the .h file for the directory function fs_move(). It is not
*              found in mfs.h, rather it's a function we're creating to implement the
*              the "mv" command found in fsshell.c, allowing users to move
*              files/directories to another directory.
*
**************************************************************/



#include "parsepath.h"
#include "mfs.h"


int fs_move(const char *pathname1, const char *pathname2);
/* CMPSC 311, Spring 2012, Project 7
 *
 * Authors:   Erich Stoekl and Andrew Moyer
 * Email:     ems5311@psu.edu and abm5149@psu.edu
 *
 * cmpsc311.h
 * 
 * Function declarations for cmpsc311.c utility functions.
 *
 */

#ifndef CMPSC311_H
#define CMPSC311_H

#include <stddef.h>
#include <stdbool.h>

// global max value macros:

#define MAXLINE 128
#define MAXARGS 128

//------------------------------------------------------------------------------

// global variables set from the command line

extern char *prog;	// program name
extern int verbose;	// -v option, extra output

extern FILE *f_p;
extern char *filename;
extern char *startfile;

extern char *prompt;

//------------------------------------------------------------------------------

// utility functions

// guard against null pointer for character string output
const char *safe_string(const char *str);

//------------------------------------------------------------------------------

// check function arguments

#define verify(tf, msg) cmpsc311_verify(tf, msg, __func__, __LINE__)

void cmpsc311_verify(const bool tf, const char *msg				, const char *func, const int line);

//------------------------------------------------------------------------------

// check function return values
//   malloc
//   strdup
//   fopen

#define Malloc(size) cmpsc311_malloc(size, __func__, __LINE__)
#define Strdup(s1) cmpsc311_strdup(s1, __func__, __LINE__)
#define Fopen(filename,mode) cmpsc311_fopen(filename, mode, __func__, __LINE__)

void *cmpsc311_malloc(size_t size						, const char *func, const int line);
char *cmpsc311_strdup(const char *s1						, const char *func, const int line);
FILE *cmpsc311_fopen(const char * restrict filename, const char * restrict mode	, const char *func, const int line);

//------------------------------------------------------------------------------

#endif

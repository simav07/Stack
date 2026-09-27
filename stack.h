#ifndef STACK
#define STACK

typedef double stackElem_t;
#define STACK_ELEM_FORMAT  "%lg"
#define STACK_POIZON NAN

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <math.h>

//---------------------------------------------------------------------------------------
// Colors
//---------------------------------------------------------------------------------------

#define YELLOW "\x1b[33m"
#define RED    "\x1b[31m"
#define GREEN  "\x1b[32m"
#define CYAN   "\x1b[36m"

#define BOLD_RED    "\x1b[1;31m"
#define BOLD_GREEN  "\x1b[1;32m"
#define BOLD_CYAN   "\x1b[1;36m"
#define BOLD_YELLOW "\x1b[1;33m"

// Colors reset
#define RESET  "\x1b[0m"

//! Stack and all using parameters
struct stack_t {

    stackElem_t * data = NULL;
    ssize_t size = 0;
    ssize_t capasity = 0;

};

//! Filename for logging
const char LOGFILE_NAME[] = "MyLogfile.txt";

#ifdef MYDEBUG

#define ASSERT(right_instr) do { \
    if (!right_instr) { \
        fprintf(stderr, "\nAssertion failed: (%s), file <%s>, line: %d\n\n", #right_instr, __FILE__, __LINE__); \
        printf("Current function: %s\n", __PRETTY_FUNCTION__);\
        printf(RED "Link to the line:\n" RESET);          \
        printf("File: " __FILE__ ":%d:1:\n\n", __LINE__); \
        printf(RED "--------------------------------------------------" RESET); \
        abort(); \
    } \
} while (0)
#else
#define ASSERT(right_instr) do {} while(0)
#endif

//-------------------------------------------------------------------------
// LOGGING
//-------------------------------------------------------------------------

#ifdef LOG_MODE

#define STACK_LOGGING(logfile, stk) STACK_LOG(logfile, stk, #stk, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#else
#define STACK_LOGGING(logfile, stk) do {} while(0)

#endif
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
//  Functions
// --------------------------------------------------------------------------

//! Maximum complete stack printout
void PrintStack(FILE * stream, stack_t stk);

void STACK_LOG(FILE * logFile, stack_t stk, const char stkName[], const char * fileName, const char *funcName, unsigned int nLine);
void CleanFile(const char filename[]);


#endif
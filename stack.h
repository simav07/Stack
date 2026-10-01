#ifndef STACK
#define STACK

typedef double stackElem_t;
#define STACK_ELEM_FORMAT  "%lg"
#define STACK_POISON NAN

#include "colors.h"

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <math.h>

//! Stack and all using parameters
struct stack_t {

    //! Left canary
    const stackElem_t leftCanary = 0xDEADBABE;

    stackElem_t * data = NULL;
    stackElem_t * dataBegin = NULL;
    ssize_t size = 0;
    ssize_t capacity = 0;

    //! Right canary
    const stackElem_t rightCanary = 0xCAFEBABE;
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
#define ASSERT(right_instr) do {} while(0) // (void) 0 -- alternative
#endif

//-------------------------------------------------------------------------
// LOGGING
//-------------------------------------------------------------------------

#ifdef LOG_MODE

#define STACK_LOGGING(logfile, stk) STACK_LOG(logfile, &(stk), #stk, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#define WRITE_ERROR_LOG(logfile, message) WriteErrorLog(logfile, message, __FILE__, __PRETTY_FUNCTION__)

#else
#define STACK_LOGGING(logfile, stk) do {} while(0)

#define WRITE_ERROR_LOG (void)0

#endif

// --------------------------------------------------------------------------
//  Functions
// --------------------------------------------------------------------------

//! Maximum complete stack printout
void StackDump(FILE * stream, const stack_t * stk);

//! Printout to the logfile (using StackDump)
void STACK_LOG(FILE * logFile, const stack_t * stk, const char stkName[], const char * fileName, const char *funcName, unsigned int nLine);

//! Write verified by StackVerify error to the logfile
void WriteErrorLog(FILE * file_p, const char * message, const char * fileName, const char *funcName);

#endif
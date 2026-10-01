#ifndef STACK
#define STACK

typedef double stackElem_t;
#define STACK_ELEM_FORMAT  "%lg"
#define STACK_POISON NAN

#define STACK_LEFT_CANARY  0xDEADBABE
#define STACK_RIGHT_CANARY 0xCAFEBABE

#include "colors.h"
#include "log.h"

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
    const stackElem_t leftCanary = STACK_LEFT_CANARY;

    stackElem_t * data = NULL;
    stackElem_t * dataBegin = NULL;
    ssize_t size = 0;
    ssize_t capacity = 0;

    //! Number of canaries at one side of stack
    const size_t nCanaries = 1;

    //! Right canary
    const stackElem_t rightCanary = STACK_RIGHT_CANARY;
};

//! Filename for logging
const char LOGFILE_NAME[] = "MyLogfile.txt";

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
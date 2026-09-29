// 1. Открывать log_file глобально для всего файла
// 2. ssize_t (для проверки на отриц число)
// 3. Информация про переменную структуры в log file (её название и тип)
// 4. Для печати stack_t в лог файл сделать
// 5. Вызов CheckStackError из StackVerify (обработка ошибок)
// 6. При вызове StackVerify с помощью условной компиляции передается строка и функция в которой вызвана,
//    затем в файл или в консоль печатается код ошибки, расшифровка ошибки и строка, файл
// 7. Добавить проверки на размеры структуры, корректность параметров, корректность данных внутри
// 8. информация о структуре в усл компиляции
// 9. нужно ли заполнять НЕ ядом ячейки памяти, которые мы перестали использовать (размер стека был уменьшен)
// 10. печатать ошибку в лог файл (сейчас только в консоль)

typedef int err_t;

#include "stack.h"

enum {
    INIT_NUM = 100, //! Starting value for protecting

    ERR_OK,             //! All good
    ERR_CTOR,           //! Getted stack was already constructed
    ERR_NULL_STACK,     //! Getted null-pointer to stack
    ERR_CALLOC,         //! If calloc returned NULL
    ERR_REALLOC,        //! If realloc returned NULL
    ERR_CAPACITY,       //! Incorrect capacity value
    ERR_FILEOPEN,       //! fopen returned NULL
    ERR_BAD_SIZE,       //! The size is more than capacity
    ERR_ZERO_SIZE,      //! Stack try to return value but size is zero
    ERR_SIGN_SIZE,      //! The size is less than zero
    ERR_DATA_PTR,       //! Pointer to the stack if null, but stack already constructed
    ERR_SIGN_CAPACITY,  //! The capacity is less than zero
    ERR_FAKE_POP_PTR,   //! Stack try to return poison value
    ERR_MEMORY_INIT,    //! Incorrect pointers for the place of new memory
    ERR_NULL_POINTER,   //! Getted null-pointer to stack
};

const ssize_t MIN_CAPACITY = 5;

const int MEMORY_UP_COEFF = 2;

//! Stack initialisation
err_t StackCtor(stack_t * stk, ssize_t capacity);

err_t NewMemoryInit(stack_t * stk, stackElem_t * firstIndex, stackElem_t * lastIndex);

//! Stack distraction
err_t StackDtor(stack_t * stk);

//! Standard push
err_t StackPush(stack_t * stk, stackElem_t value);
//! Standard pop
stackElem_t StackPop(stack_t * stk, err_t * PopStatus);

//! Checking all possibly stack errors
err_t CheckStackError(const stack_t * stk);

//! Get error description
const char * ErrorDescription(err_t errCode);

//! Verification of all errors
err_t StackVerify(const stack_t * stk, err_t ERR_CODE);

//! Pointer to LogFile
static FILE * LOGFILE = NULL;

err_t CheckFile(FILE * file_p);

int main() {

    LOGFILE = fopen(LOGFILE_NAME, "w");
    if (!LOGFILE) printf("Error path to LOGFILE\n");

    stack_t stk1 = {};
    if (!StackCtor(&stk1, 10)) {
        printf("StackCtor error\n");
        return false;
    }

    for (size_t i = 0; i < 16; i++) {

        err_t PushStatus = StackPush(&stk1, 1000 + i);

        if (PushStatus != ERR_OK) return false;

        STACK_LOGGING(LOGFILE, stk1);

    }

    for (size_t i = 0; i < 20; i++) {

        err_t PopStatus = ERR_OK;

        StackPop(&stk1, &PopStatus);

        if (PopStatus != ERR_OK) return false;

        STACK_LOGGING(LOGFILE, stk1);

    }
// bad tests 
    StackDtor(&stk1);
    fclose(LOGFILE);

    return true;
}

err_t CheckFile(FILE * file_p) {

    if (file_p == NULL) return ERR_FILEOPEN;

    return ERR_OK;
}

err_t StackCtor(stack_t * stk, ssize_t capacity) {

    if (stk == NULL) return ERR_NULL_STACK;

    //! Error from constructor: stack is already constructed and maybe data has been using
    if ((*stk).data != NULL || (*stk).capacity != 0 || (*stk).size != 0) {
        return ERR_CTOR;
    }
    
    if (capacity <= 0) {
        return ERR_CAPACITY;
    }
    
    stackElem_t * NewPtr = (stackElem_t *)calloc(capacity, sizeof(stackElem_t));

    if (NewPtr == NULL) return ERR_CALLOC;

    (*stk).data = NewPtr;

    (*stk).capacity = capacity;

    NewMemoryInit(stk, (*stk).data, (*stk).data + (*stk).capacity);

    return ERR_OK;
}

err_t NewMemoryInit(stack_t * stk, stackElem_t * firstIndex, stackElem_t * lastIndex) {

    ASSERT(firstIndex);
    ASSERT(lastIndex);
    ASSERT(stk);
    ASSERT(stk->data);

    if ((firstIndex >= lastIndex) || (firstIndex < (stk->data + stk->size))) return ERR_MEMORY_INIT;

    stackElem_t * currIndex = firstIndex;

    for (currIndex; currIndex <= lastIndex; currIndex++) {

        ASSERT(currIndex);

        *currIndex = STACK_POISON;

    }

    return ERR_OK;
}

err_t StackDtor(stack_t * stk) {

    ASSERT(stk);

    free((*stk).data);
    stk->data = NULL;

    (*stk).capacity = 0;
    (*stk).size = 0;

    return true;
}

err_t StackPush(stack_t * stk, stackElem_t value) {

    ASSERT((StackVerify(stk, ERR_OK) == ERR_OK)); // ASSERT_OK

    // TODO  to func
    if (stk->size == stk->capacity) {

        // copypaste?
        stackElem_t * NewPtr = (stackElem_t *)realloc((*stk).data, 
                                sizeof(stackElem_t)*(ssize_t)((*stk).capacity * MEMORY_UP_COEFF));

        if (NewPtr == NULL) return ERR_REALLOC;

        stk->data = NewPtr;

        (*stk).capacity = (ssize_t)((*stk).capacity * MEMORY_UP_COEFF);

        NewMemoryInit(stk, (*stk).data + (*stk).size, (*stk).data + (*stk).capacity);

    }

    (*stk).data[(*stk).size++] = (stackElem_t)value;

    ASSERT((StackVerify(stk, ERR_OK) == ERR_OK));

    return ERR_OK;
}

stackElem_t StackPop(stack_t * stk, err_t * PopStatus) {

    ASSERT(stk);
    ASSERT((StackVerify(stk, ERR_OK) == ERR_OK));
    
    if (stk->size == 0) *PopStatus = ERR_FAKE_POP_PTR;

    stk->size--;

    ASSERT((StackVerify(stk, ERR_OK) == ERR_OK));

    stackElem_t currElem = stk->data[(*stk).size];

    ASSERT(currElem);
    stk->data[stk->size] = STACK_POISON;

    ASSERT((StackVerify(stk, ERR_OK) == ERR_OK));

    //! Free up memory if there's a lot of space
    //! It works only if we have more than one realloc
    if (((*stk).capacity > MIN_CAPACITY) && (*stk).size < ((*stk).capacity / 4)) {

        stackElem_t * NewPtr = (stackElem_t *)realloc((*stk).data,
                                                       sizeof(stackElem_t)*(ssize_t)(((*stk).capacity) /MEMORY_UP_COEFF));

        if (NewPtr == NULL) {
            *PopStatus = ERR_REALLOC;
            return STACK_POISON;
        }

        (*stk).data = NewPtr;

        (*stk).capacity = (ssize_t)(((*stk).capacity) / MEMORY_UP_COEFF);
        
    }

    return currElem;
}

err_t CheckStackError(const stack_t * stk) {

    if (stk == NULL) return ERR_NULL_POINTER;

    if (stk->data == NULL) return ERR_NULL_STACK;

    if (stk->size > stk->capacity) return ERR_BAD_SIZE;

    if (stk->size < 0) return ERR_SIGN_SIZE;

    if (stk->capacity < 0) return ERR_SIGN_CAPACITY;

    if (stk->data == NULL) return ERR_DATA_PTR;
    
    return ERR_OK;
}

err_t StackVerify(const stack_t * stk, err_t ERR_CODE) {

    if (ERR_CODE == ERR_OK) {
        ERR_CODE = CheckStackError(stk);
    }
    if (ERR_CODE == ERR_OK) {
        return ERR_OK;
    }

    const char * err_descr = ErrorDescription(ERR_CODE);

    fprintf(stderr, RED "%s\n" RESET, err_descr);
    WRITE_ERROR_LOG(LOGFILE, err_descr);
    STACK_LOGGING(LOGFILE, (*stk));
    return ERR_CODE;
}

const char * ErrorDescription(err_t errCode) {

    switch (errCode)
    {

    case ERR_CTOR:
        return "ERR_CTOR: Getted stack was already constructed";

    case ERR_NULL_STACK:
        return "ERR_NULL_STACK: Getted null-pointer to stack";

    case ERR_CALLOC:
        return "ERR_CALLOC: If calloc returned NULL";

    case ERR_REALLOC:
        return "ERR_REALLOC: If realloc returned NULL";

    case ERR_CAPACITY:
        return "ERR_CAPACITY: Incorrect capacity value";

    case ERR_FILEOPEN:
        return "ERR_FILEOPEN: fopen returned NULL";

    case ERR_BAD_SIZE:
        return "ERR_BAD_SIZE: The size is more than capacity";

    case ERR_ZERO_SIZE:
        return "ERR_ZERO_SIZE: Stack try to return value but size is zero";

    case ERR_SIGN_SIZE:
        return "ERR_SIGN_SIZE: The size is less than zero\n";

    case ERR_DATA_PTR:
        return "ERR_DATA_PTR: Pointer to the stack is null, but stack already constructed";

    case ERR_SIGN_CAPACITY:
        return "ERR_SIGN_CAPACITY: The capacity is less than zero";

    case ERR_FAKE_POP_PTR:
        return "ERR_FAKE_POP_PTR: Stack try to return poison value";
    
    case (ERR_MEMORY_INIT):
        return "ERR_MEMORY_INIT: Incorrect pointers for the place of new memory";
    
    case (ERR_NULL_POINTER):
        return "ERR_NULL_POINTER: Getted null-pointer to stack";

    default:
        return "UNKNOWN ERROR: Unknown error code";
    }
}

void StackDump(FILE * stream, const stack_t * stk) {

    ASSERT(stream);

    if (stk == NULL) {
        fprintf(stream, "ATTENTION! POINTER TO STACK IS NULL\n");
    }

    fprintf(stream, "Capacity = %llu\n", stk->capacity);
    fprintf(stream, "Size = %zd", stk->size);
    fprintf(stream, "\ndata[%p]\n{\n", stk->data);

    if (stk->data == NULL) {
        fprintf(stream, "Data is NULL, there is no elements\n");
        return;
    }

    for (ssize_t i = 0; i < stk->size; i++) {
        fprintf(stream, "\t*[%u] = " STACK_ELEM_FORMAT "\n", i, stk->data[i]);
    }

    for (ssize_t i = stk->size; i < stk->capacity; i++) {
        fprintf(stream, "\t [%u] = " STACK_ELEM_FORMAT "\n", i, stk->data[i]);
    }

    fprintf(stream, "}");
}

void STACK_LOG(FILE * logFile, const stack_t * stk, const char stkName[], const char * fileName,
                const char *funcName, unsigned int nLine) {

    ASSERT(stk);
    ASSERT(fileName);
    ASSERT(funcName);
    ASSERT(stkName);

    if (!logFile) {
        fprintf(stderr, "Error of opening file: <%s>\n", LOGFILE_NAME); 
        return;
    }

    fprintf(logFile, "\n--------------------------------------------------------\n");
    fprintf(logFile, "stack_t \"%s\"", stkName);
    fprintf(logFile, " created by function <%s> in <%s>. Line: %u\n", funcName, fileName, nLine);

    StackDump(logFile, stk);

    //! Forced writing to disk 
    fflush(logFile);
}

void WriteErrorLog(FILE * logfile, const char * message, const char * fileName, const char *funcName) {

    ASSERT(logfile);
    ASSERT(fileName);
    ASSERT(funcName);

    fprintf(logfile, "\n-------------------WARNING------------------------------");
    fprintf(logfile, "\nCalled from <%s> in <%s>.\n", funcName, fileName);

    fprintf(logfile, "\nVerified error: %s\n", message);
    fprintf(logfile, "--------------------------------------------------------\n");
    fprintf(logfile, "\n");
}
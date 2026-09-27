// 1. Открывать log_file глобально для всего файла
// 2. ssize_t (для проверки на отриц число)
// 3. Информация про переменную структуры в log file (её название и тип)
// 4. Для печати stack_t в лог файл сделать STACK_LOG вида STACK_LOG(stack_t, stk1)
//                                                                  (const char typeName[], stack_t stk1)
// 5. Вызов StackVerify из StackError (обработка ошибок)

typedef int err_t;

#include "stack.h"

enum {

    ERR_CTOR, //! Getted stack was already constructed


};

const ssize_t MIN_CAPASITY = 5;

const unsigned int MEMORY_UP_COEFF = 2;

//! Pointer to LogFile
//FILE * LOGFILE = NULL;

//! Stack initialisation
err_t StackCtor(stack_t * stk, ssize_t capasity);

//! Stack distraction
err_t StackDtor(stack_t * stk);

//! Standard push
err_t StackPush(stack_t * stk, stackElem_t value);
//! Standard pop
stackElem_t StackPop(stack_t * stk);

//! Checking all possibly stack errors
err_t StackError(stack_t stk);

static FILE * LOGFILE;

int main() {

    CleanFile(LOGFILE_NAME);
    LOGFILE = fopen(LOGFILE_NAME, "a");

    stack_t stk1 = {};

    if (!StackCtor(&stk1, 3)) {
        printf("StackCtor error\n");
        return false;
    }

    StackPush(&stk1, 1);

    STACK_LOGGING(LOGFILE, stk1);

    StackPush(&stk1, 2);

    STACK_LOGGING(LOGFILE, stk1);

    StackPush(&stk1, 3);

    STACK_LOGGING(LOGFILE, stk1);

    StackPush(&stk1, 3);

    STACK_LOGGING(LOGFILE, stk1);

    StackPush(&stk1, 3);

    STACK_LOGGING(LOGFILE, stk1);

    StackPush(&stk1, 3);

    STACK_LOGGING(LOGFILE, stk1);

    StackPop(&stk1);

    STACK_LOGGING(LOGFILE, stk1);

    StackDtor(&stk1);
    fclose(LOGFILE);

    return true;
}

err_t StackCtor(stack_t * stk, ssize_t capasity) {

    ASSERT(stk);

    //! Error from constructor: stack is already constructed and maybe data has been using
    if ((*stk).data[0] == STACK_POIZON || (*stk).data[(*stk).capasity] == STACK_POIZON || (*stk).capasity != 0) {
        return ERR_CTOR;
    }

    (*stk).data = (stackElem_t *)calloc(capasity, sizeof(stackElem_t));
    (*stk).capasity = capasity;

    //! Show constructed struct
    //STACK_LOGGING(LOGFILE, *stk);

    return true;
}

err_t NewMemoryInit(stack_t * stk, stackElem_t * firstIndex, stackElem_t * lastIndex) {

    ASSERT(firstIndex);
    ASSERT(lastIndex);
    ASSERT(stk);

    stackElem_t * currIndex = firstIndex;

    for (currIndex; currIndex <= lastIndex; currIndex++) {

        ASSERT(currIndex);

        *currIndex = STACK_POIZON;

    }

    //STACK_LOGGING(LOGFILE, *stk);

    return true;
}

err_t StackDtor(stack_t * stk) {

    ASSERT(stk);

    free((*stk).data);

    (*stk).capasity = 0;
    (*stk).size = 0;

    return true;
}

err_t StackPush(stack_t * stk, stackElem_t value) {

    ASSERT(stk);
    ASSERT(StackError(*stk)); // ASSERT_OK
    // вызов StackDump

    //STACK_LOGGING(LOGFILE, *stk);

    if ((*stk).size ==(*stk).capasity) {

        printf("Stack if full\n");

        (*stk).data = (stackElem_t *)realloc((*stk).data, (ssize_t)((*stk).capasity * MEMORY_UP_COEFF));
        (*stk).capasity = (ssize_t)((*stk).capasity * MEMORY_UP_COEFF);
        NewMemoryInit(stk, (*stk).data + (*stk).size + 1, (*stk).data + (*stk).capasity);

    }

    (*stk).data[(*stk).size++] = (stackElem_t)value;

    return true;
}

stackElem_t StackPop(stack_t * stk) {

    ASSERT(stk);
    ASSERT(StackError(*stk));
    
    stackElem_t currElem = (*stk).data[(*stk).size-1];
    ASSERT(currElem);

    //STACK_LOGGING(LOGFILE, *stk);

    (*stk).size--;
    (*stk).data[(*stk).size] = STACK_POIZON;

    ASSERT(StackError(*stk));

    //! Free up memory if there's a lot of space
    //! It works only if we have more than one realloc
    if (((*stk).capasity > MIN_CAPASITY) && (*stk).size > ((*stk).size / 4)) {

        (*stk).data = (stackElem_t *)realloc((*stk).data, (ssize_t)(((*stk).capasity) / MEMORY_UP_COEFF));
        (*stk).capasity = (ssize_t)(((*stk).capasity) / MEMORY_UP_COEFF);

    }

    return currElem;
}

err_t StackError(stack_t stk) {

    if ((stk.size <= stk.capasity) && (stk.capasity > 0) && (stk.data != NULL)) return true;
    
    return false;
}

void PrintStack(FILE * stream, stack_t stk) {

    ASSERT(stream);
    ASSERT(stk.data);

    fprintf(stream, "Capasity = %llu\n", stk.capasity);
    fprintf(stream, "Size = %llu\n", stk.size);
    fprintf(stream, "data[%p]\n{\n", stk.data);

    for (unsigned int i = 0; i < stk.size; i++) {
        fprintf(stream, "\t*[%u] = " STACK_ELEM_FORMAT "\n", i, stk.data[i]);
    }

    for (unsigned int i = stk.size; i < stk.capasity; i++) {
        fprintf(stream, "\t [%u] = " STACK_ELEM_FORMAT "\n", i, stk.data[i]);
    }

    fprintf(stream, "}");
}

void STACK_LOG(FILE * logFile, stack_t stk, const char stkName[], const char * fileName, const char *funcName, unsigned int nLine) {

    ASSERT(fileName);
    ASSERT(funcName);
    ASSERT(stkName);

    if (!logFile) {
        printf("Error of opening file: <%s>\n", LOGFILE_NAME);
        return;
    }

    fprintf(logFile, "\n--------------------------------------------------------\n");
    fprintf(logFile, "stack_t \"%s\"", stkName);
    fprintf(logFile, " created by function <%s> in <%s>. Line: %u\n", funcName, fileName, nLine);

    PrintStack(logFile, stk);

    //! Forced writing to disk
    fflush(logFile);
}

void CleanFile(const char filename[]) {

    ASSERT(filename);
    FILE * file_p = fopen(filename, "w");
    fclose(file_p);
}

int IsBigger(double p, double q, double epsilon) {

    ASSERT(isfinite (p));
    ASSERT(isfinite(q));
    ASSERT(isfinite (epsilon));

    if ((p - q) > epsilon)
        return true;
    return false;
}
typedef double stackElem_t;
typedef int err_t;

#include "stack.h"

//! Stack initialisation
err_t StackCtor(stack_t * stk, size_t capasity);

//! Stack distraction
err_t StackDtor(stack_t * stk);

err_t StackPush(stack_t * stk, stackElem_t value);

//! Checking all possibly stack errors
err_t StackError(stack_t stk);

int main() {

    stack_t stk1 = {};
    StackCtor(&stk1, 5);
    printf("%llu\n", stk1.capasity);

}

err_t StackCtor(stack_t * stk, size_t capasity) {

    ASSERT(stk);

    (*stk).data = (stackElem_t *)calloc(capasity, sizeof(stackElem_t));
    (*stk).capasity = capasity;
    return true;
}

err_t StackDtor(stack_t * stk) {

    ASSERT(stk);

    free((*stk).data);

    (*stk).capasity = 0;
    (*stk).size = 0;
}

err_t StackPush(stack_t * stk, stackElem_t value) {

    ASSERT(stk);
    ASSERT(StackError(stk));

    if ((*stk).size == (*stk).capasity) {

        (*stk).data = (stackElem_t *)realloc((*stk).data, (size_t)((*stk).capasity * 1.5));
        (*stk).capasity = (size_t)((*stk).capasity * 1.5);

    }

    (*stk).data[(*stk).size++] = value;

}

err_t StackError(stack_t stk) {

    if ((stk.size <= stk.capasity) && (stk.capasity > 0) && (stk.data != NULL)) return true;
    
    return false;
}
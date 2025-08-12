#include <sys/types.h>
#include <stdlib.h>

typedef struct stack_t {
    stack_data_t *head;
    stack_data_t *tail;
    
    u_int32_t size;
} Stack;

typedef struct stack_data_t {
    void *data;
    void *next;
} stack_data_t;


int isEmpty(Stack*);
void *stackPop(Stack*);
void stackPush(Stack*, void*);
Stack initStack(void*);
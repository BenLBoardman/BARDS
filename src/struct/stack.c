#include "stack.h"


Stack initStack(void *head) {
    Stack newStack;

    newStack.head = NULL;
    newStack.tail = NULL;
    newStack.size = 0;

    if(head != NULL)
        StackPush(&newStack, head);

    return newStack;
}

void pushStack(Stack *stack, void *data) {
    stack_data_t *stData = malloc(sizeof(stack_data_t));

    stData->data = data;
    stData->next = NULL;

    if(stack->size == 0) {
        stack->head = data;
        stack->tail = data;
    } else {
        stack->tail->next = data;
        stack->tail = data;
    }
    stack->size++;
}

void popStack(Stack *stack) {
    stack_data_t *stData = stack->head;
    void *data = stData->data;

    if(stack->size == 0){
        return NULL;
    } else if(stack->size == 1) {
        stack->head == NULL;
        stack->tail == NULL;
    } else {
        stack->head = stack->head->next;
    }

    free(stData);
    return data;
}

int isEmpty(Stack *stack) {
    return stack->size == 0;
}
#include "datastruct.h"

//Add a Precinct to a linked list
// Input: A pointer to the head of the linked list, a pointer to the precinct to add
void ll_add(LinkList *list, Precinct *precinct) {
    LinkList *curr = list;
    LinkList *new;
    
    if(curr->data == NULL) { //empty list case
        curr->data = precinct;
        return;
    }

    new = malloc(sizeof(LinkList));
    new->data = precinct; 
    for(curr; curr->next != NULL; curr = curr->next);
    curr->next = new;
    new->prev = curr;

}

//Given a pointer to a precinct, remove it from the linked list if it exists
// Input: A pointer to the head of the linked list, a pointer ot the precinct to be removed
// Output: a pointer to the removed precinct, or NULL if it doesn't exist
Precinct *ll_remove(LinkList *list, Precinct *target) {
    LinkList *remove, prev, next;
    Precinct *out;

    remove = ll_find(list, target);
    if(remove == NULL) {
        return;
    }

    if(remove == list) {
        list = remove->next;
    } else {
        remove->prev->next = remove->next;
    }

    if(remove->next == NULL) {
        remove->next->prev = remove->prev;
    }

    out = remove->data;
    free(remove);
    return out;
}


//Given a pointer to a Precinct, determine if it is present in a linked list.
// Input: a pointer to the head of a linked list, a pointer to a Precinct to find
// Output: a pointer to the LinkList containing the target (if found), or NULL if it is not found.
LinkList *ll_find(LinkList *list, Precinct *target) {
    LinkList *curr = list;
    for(; curr != NULL; curr = curr->next) {
        if(curr->data == target) { //directly comparing memory addresses here is fine
            return curr;
        }
    }
    return NULL;
}

//Given a pointer to a Precinct, return it if it is present.
// Input: a pointer to the head of a linked list, a pointer to a Precinct to find
// Output: a pointer to the Pointer containing the target (if found), or NULL if it is not found.
Precinct *ll_get(LinkList *list, Precinct *target) {
    LinkList *find;
    find = ll_find(list, target);
    return find == NULL ? NULL : find->data;
}


// This function gets the size of a linked list
// Input: a pointer to the head of a linked list
// Output: the number of elements in the linked list
int ll_size(LinkList* list) {
    int i = 0;
    LinkList *curr = list;
    for(i; curr != NULL && curr->data != NULL; i++, curr = curr->next);
    return i;
}


// This function frees all elements of a linked list
// Input: a pointer to the head of a linked list
void ll_free(LinkList* list) {
    LinkList *curr = list;

    while(curr != NULL) {
        curr = list->next;
        free(list);
        list = curr;
    }
}


// Hash a precinct based on its id (order in the state precinct list)
// Input: a pointer to the precinct to be hashed
int hash(Precinct *precinct) {
    return precinct->id % HASH_TABLE_BUCKETS; //simple hash algorithm, works since the hashing is based on an id
}


// Determine if a precinct is in the hash table already
// Input: a pointer to a hashtable and a pointer to a precinct
// Output: A boolean corresponding to whether the precinct was found
int ht_find(HashTable *table, Precinct *precinct) {
    int pHash = hash(precinct);

    return ll_find(table->table[pHash], precinct) != NULL ? 1 : 0;
}


//Insert a precinct into a hashtable, if it isn't present already
// Input: a pointer to a hashtable and a pointer to a precinct
void ht_insert(HashTable *table, Precinct *precinct) {
    int pHash;

    if(ht_find(table, precinct)) {
        return;
    }

    pHash = hash(precinct);
    ll_add(table->table[pHash], precinct);    
}

Stack st_init(Precinct *head) {
    Stack newStack;

    newStack.head = NULL;
    newStack.tail = NULL;
    newStack.size = 0;

    if(head != NULL)
        st_push(&newStack, head);

    return newStack;
}

void st_push(Stack *stack, Precinct *data) {
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

void st_pop(Stack *stack) {
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

int st_isEmpty(Stack *stack) {
    return stack->size == 0;
}
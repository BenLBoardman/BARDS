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
    LinkList *remove;
    Precinct *out;

    remove = ll_find(list, target);
    if(remove == NULL) {
        return NULL;
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

//This function gets the size of a linked list
// Input: a pointer to the head of a linked list
// Output: the number of elements in the linked list
int ll_size(LinkList* list) {
    int i = 0;
    LinkList *curr = list;
    for(i; curr != NULL && curr->data != NULL; i++, curr = curr->next);
    return i;
}

//This function frees all elements of a linked list
// Input: a pointer to the head of a linked list
void ll_free(LinkList* list) {
    LinkList *curr = list;

    while(curr != NULL) {
        curr = list->next;
        free(list);
        list = curr;
    }
}

//Initiate a hash table with a specific capacity
// Input: A capacity for the hash table.
// Output: A pointer to the newly initiated HashTable
HashTable *ht_init(int capacity) {
    HashTable *table;
    table = malloc(sizeof(HashTable));

    table->capacity = capacity;
    table ->size = 0;
    table->table = malloc(sizeof(LinkList) * capacity);

    for(int i = 0; i < capacity; i++) {
        table->table[i] = malloc(sizeof(LinkList));
    }

    return table;
}

//Hash a precinct based on its id (order in the state precinct list)
// Input: a pointer to the hash table and a pointer to the precinct to be hashed
// Output: a hash value to locate the precinct in the hash table
int hash(HashTable *table, Precinct *precinct) {
    return precinct->id % table->capacity; //simple hash algorithm, works since the hashing is based on an id
}

//Determine if a precinct is in the hash table already
// Input: a pointer to a hashtable and a pointer to a precinct
// Output: A boolean corresponding to whether the precinct was found
int ht_find(HashTable *table, Precinct *precinct) {
    int pHash = hash(table, precinct);

    return ll_find(table->table[pHash], precinct) != NULL ? 1 : 0;
}

//Get the linked list entry at a specific index.
//May be deprecated/removed if it isn't used.
// Input: A pointer to the hash table, and an index
// Output: A pointer to the linked-list entry at the given index
LinkList *ht_get(HashTable *table, int index) {
    return table->table[index];
}

//Insert a precinct into a hashtable, if it isn't present already
// Input: a pointer to a hashtable and a pointer to a precinct
void ht_insert(HashTable *table, Precinct *precinct) {
    int pHash;

    if(ht_find(table, precinct)) {
        return;
    }

    pHash = hash(table, precinct);
    ll_add(table->table[pHash], precinct);
    table->size++;    
}

//Remove and return a precinct from a hash table, if it exists
// Input: a pointer to a hashtable and a pointer to a precinct to remove
// Output: the removed precinct.
Precinct *ht_remove(HashTable *table, Precinct *precinct) {
    LinkList *ht_list;
    ht_list = ht_get(table, hash(table, precinct));

    if(ht_list != NULL) 
        ll_remove(ht_list, precinct);

    table->size--;
    return precinct;
}

//Free the memory associated with a hash table
// Input: a hash table to free
void ht_free(HashTable *table) {
    for(int i = 0; i < table->capacity; i++) {
        ll_free(table->table[i]);
    }

    free(table->table);
    free(table);
}

//Initiate a new stack, with an optional head value
// Input: A pointer to the precinct to serve as the head of the stack (OPTIONAL)
// Output: a pointer to the newly created stack
Queue *qu_init(Precinct *head) {
    Queue *newStack = malloc(sizeof(Queue));

    newStack->head = NULL;
    newStack->tail = NULL;
    newStack->size = 0;

    if(head != NULL)
        qu_push(newStack, head);

    return newStack;
}

//Push a precinct onto the stack
// Input: A pointer to the stack and a pointer to the precinct to add to the stack
void qu_push(Queue *stack, Precinct *data) {
    queue_data_t *stData = malloc(sizeof(queue_data_t));

    stData->data = data;
    stData->next = NULL;

    if(stack->size == 0) {
        stack->head = stData;
        stack->tail = stData;
    } else {
        stack->tail->next = stData;
        stack->tail = stData;
    }
    stack->size++;
}

//Pop a precinct from the stack
// Input: A pointer to the stack
// Output: A pointer to the popped precinct
Precinct *qu_pop(Queue *stack) {
    queue_data_t *quData = stack->head;
    Precinct *data = quData->data;

    if(stack->size == 0){
        return NULL;
    } else if(stack->size == 1) {
        stack->head = NULL;
        stack->tail = NULL;
    } else {
        stack->head = stack->head->next;
    }

    free(quData);
    return data;
}

//Determine if a given stack is empty
// Input: a pointer to a stack
// Output: 1 if the stack is empty, 0 if it isn't
int qu_isEmpty(Queue *stack) {
    return stack->size == 0;
}

//Free a stack
// Input: a pointer to a stack
void qu_free(Queue *stack) {
    queue_data_t *curr, *prev;

    curr = stack->head;
    while(curr != NULL) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(stack);
}
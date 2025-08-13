#include "hashTable.h"

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



int hash(HashTable *, Precinct *);
int exists(HashTable *, Precinct *);
void insert(HashTable *, Precinct *);
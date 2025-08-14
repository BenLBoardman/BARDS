#include <stdlib.h>
#include <sys/types.h>

#include "state.h"

#define HASH_TABLE_BUCKETS 1024 //should be large enough to avoid many collisions on even the largest states


typedef struct link_list_t {
    struct link_list_t *prev;
    Precinct *data;
    struct link_list_t *next;
} LinkList;

typedef struct hash_table_t {
    LinkList **table;
    u_int32_t size;
    u_int32_t capacity;
} HashTable;

typedef struct stack_data_t {
    Precinct *data;
    struct stack_data_t *next;
} stack_data_t;

typedef struct stack_t {
    stack_data_t *head;
    stack_data_t *tail;
    
    u_int32_t size;
} Stack;

void ll_add(LinkList*, Precinct*);
Precinct *ll_remove(LinkList*, Precinct*);
LinkList *ll_find(LinkList*, Precinct*);
Precinct *ll_get(LinkList*, Precinct*);
int ll_size(LinkList*);
void ll_free(LinkList*);

HashTable *ht_init(int);
int hash(HashTable *, Precinct *);
int ht_find(HashTable *, Precinct *);
LinkList *ht_get(HashTable *, int);
void ht_insert(HashTable *, Precinct *);
Precinct *ht_remove(HashTable *, Precinct *);

Stack *st_init(Precinct*);
int st_isEmpty(Stack*);
Precinct *st_Pop(Stack*);
void st_push(Stack*, Precinct*);
void st_free(Stack*);
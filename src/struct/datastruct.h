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

typedef struct queue_data_t {
    Precinct *data;
    struct queue_data_t *next;
} queue_data_t;

typedef struct queue_t {
    queue_data_t *head;
    queue_data_t *tail;
    
    u_int32_t size;
} Queue;

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

Queue *qu_init(Precinct*);
int qu_isEmpty(Queue*);
Precinct *qu_pop(Queue*);
void qu_push(Queue*, Precinct*);
void qu_free(Queue*);
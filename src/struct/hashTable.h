#include <stdlib.h>
#include <sys/types.h>

#include "state.h"

#define HASH_TABLE_BUCKETS 1024 //should be large enough to avoid many collisions on even the largest states


typedef struct link_list_t {
    LinkList *prev;
    Precinct *data;
    LinkList *next;
} LinkList;

typedef struct hash_table_t {
    LinkList *table[HASH_TABLE_BUCKETS];
    u_int32_t size;
} HashTable;

void ll_add(LinkList*, Precinct*);
Precinct *ll_remove(LinkList*, Precinct*);
LinkList *ll_find(LinkList*, Precinct*);
Precinct *ll_get(LinkList*, Precinct*);
int ll_size(LinkList*);
void ll_free(LinkList*);

int hash(HashTable *, Precinct *);
int exists(HashTable *, Precinct *);
void insert(HashTable *, Precinct *);
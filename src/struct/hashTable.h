
#include <sys/types.h>

#define HASH_TABLE_BUCKETS 1024 //should be large enough to avoid many collisions on even the largest states


typedef struct link_list_t {
    void *self;
    void *next;
} LinkList;

typedef struct hash_table_t {
    LinkList table[HASH_TABLE_BUCKETS];
    u_int32_t size;
} HashTable;

void ll_add(LinkList*);
void *ll_pop(LinkList*);
void ll_remove(LinkList*);
void *ll_find(LinkList*, void*);
void ll_size(LinkList*);
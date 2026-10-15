/*--------------------------------------------------------------------*/
/* symtablehash.c (HashTable implementation)                          */
/* Author:                                                            */
/*--------------------------------------------------------------------*/

#include "symtable.h"
#define initial_num_buckets 509

struct SymTable {
    size_t numBuckets;
    Node_T* buckets;
};

typedef struct SymTable* SymTable_T;
typedef struct Node* Node_T;

struct Node {
    const char* key;
    void* value;
    struct Node* next;
};

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */
static size_t SymTable_hash(const char* pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

SymTable_T SymTable_new(void)
{
    SymTable_T table;
    table = malloc(sizeof(struct SymTable));
    if (table == NULL)
        return NULL;
    table->numBuckets = initial_num_buckets;
    table->buckets = calloc(initial_num_buckets, sizeof(Node_T));
    if (table->buckets == NULL) {
        free(table);
        return NULL;
    }
    return table;
}
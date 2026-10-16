/*--------------------------------------------------------------------*/
/* symtablehash.c (HashTable implementation)                          */
/* Author:                                                            */
/*--------------------------------------------------------------------*/

#include "symtable.h"
#define initial_num_buckets 509

typedef struct Node* Node_T;

struct SymTable {
    size_t numBuckets;
    Node_T* buckets;
    size_t count;
};

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

static SymTable_T create_table(size_t numElements)
{
    SymTable_T table;
    table = malloc(sizeof(struct SymTable));
    if (table == NULL)
        return NULL;
    table->numBuckets = numElements;
    table->buckets = calloc(numElements, sizeof(Node_T));
    table->count = 0;
    if (table->buckets == NULL) {
        free(table);
        return NULL;
    }
    return table;
}

SymTable_T SymTable_new(void)
{
    return create_table(initial_num_buckets);
}

void SymTable_free(SymTable_T table) {
    Node_T current;
    Node_T next;
    size_t i;

    assert(table != NULL);

    for (i = 0; i < table->numBuckets; i++) {
        for (current = table->buckets[i]; current != NULL; current = next) {
            next = current->next;
            free((char*)current->key);
            free(current);
        }
    }
    free(table->buckets);
    free(table);
}


size_t SymTable_getLength(SymTable_T table) {
    assert(table != NULL);

    return table->count;
}

void* SymTable_replace(SymTable_T table, const char* key, 
    const void* value) {
    size_t hash;
    Node_T current;
    void* oldValue = NULL;

    assert(table != NULL);
    assert(key != NULL);

    hash = SymTable_hash(key, table->numBuckets);

    for (current = table->buckets[hash]; current != NULL; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            oldValue = current->value;
            current->value = (void*)value;
            break;
        }
    }

    return oldValue;
}

int SymTable_contains(SymTable_T table, const char* key) {
    size_t hash;
    Node_T current;

    assert(table != NULL);
    assert(key != NULL);

    hash = SymTable_hash(key, table->numBuckets);
    for (current = table->buckets[hash]; current != NULL; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return 1;
        }
    }
    return 0;
}

void* SymTable_get(SymTable_T table, const char* key) {
    size_t hash;
    Node_T current;

    assert(table != NULL);
    assert(key != NULL);

    hash = SymTable_hash(key, table->numBuckets);

    for (current = table->buckets[hash]; current != NULL; current = current->next) {
        if (strcmp(current->key, key) == 0) {
           return current->value;
        }
    }

    return 0;
}

void* SymTable_remove(SymTable_T table, const char* key) {
    Node_T prev= NULL;
    Node_T current;
    size_t hash;
    void* value;

    assert(table != NULL);
    assert(key != NULL);

    hash = SymTable_hash(key, table->numBuckets);
    for (current = table->buckets[hash]; current != NULL; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                table->buckets[hash] = current->next;
            } else {
                prev->next = current->next;
            }
            value = current->value;
            free((char*)current->key);
            free(current);

            table->count--;

            return value;
        }
        prev = current;
    }
    return NULL;
}

void SymTable_map(SymTable_T table, SymTableMapFunction apply, const void* param) {
    size_t i;
    Node_T current;

   assert(table != NULL);
   assert(apply != NULL);

    for (i = 0; i < table->numBuckets; i++) {
        for (current = table->buckets[i]; current != NULL; current = current->next) {
            (*apply)(current->key, current->value, (void*)param);
        }
    }
}

static void SymTable_reHash_and_put(const char* key, void* value, void* newTarget) {
    SymTable_put(newTarget, key, (SymTable_T)value);
}
static SymTable_T SymTable_expand(SymTable_T table, size_t newSize) {
    SymTable_T newTable;
    SymTableMapFunction rehashFunc;

    assert(table != NULL);

    newTable = create_table(newSize);
    if (newTable == NULL) {
        return table;
    }

    rehashFunc = SymTable_reHash_and_put;

    SymTable_map(table, rehashFunc, newTable);

    return newTable;
}

int SymTable_put(SymTable_T table, const char* key, const void* value) {
    size_t hash;
    const char* keyCopy;
    Node_T new;

    assert(table != NULL);
    assert(key != NULL);
    
    if (SymTable_contains(table, key)) {
        return 0;
    }

    keyCopy = malloc(strlen(key) + 1);
    if (keyCopy == NULL) {
        return 0;
    }
    strcpy((char*)keyCopy, key);

    if (table->count + 1 > 32749) {
        SymTable_expand(table, 65521);
    } else if (table->count + 1 > 16381) {
        SymTable_expand(table, 32749);
    } else if (table->count + 1 > 8191) {
        SymTable_expand(table, 16381);
    } else if (table->count + 1 > 4093) {
        SymTable_expand(table, 8191);
    } else if (table->count + 1 > 2039) {
        SymTable_expand(table, 4093);
    } else if (table->count + 1 > 1021) {
        SymTable_expand(table, 2039);
    } else if (table->count + 1 > 509) {
        SymTable_expand(table, 1021);
    }

    hash = SymTable_hash(keyCopy, table->numBuckets);

    new = (Node_T)malloc(sizeof(struct Node));
    if (new == NULL) {
        return 0;
    }
    new->key = keyCopy;
    new->value = (void*)value;
    new->next = table->buckets[hash];
    table->buckets[hash] = new;
    
    table->count++;
    return 1;
}
/*--------------------------------------------------------------------*/
/* symtablehash.c (HashTable implementation)                          */
/* Author:   William and Megan                                        */
/*--------------------------------------------------------------------*/

#include "symtable.h"

/* The number of buckets in hash table to begin with */
#define initial_num_buckets 509
/* defines number of buckets in hash table after doubling once */
#define second_bucket_size 1021
/* defines number of buckets in hash table after doubling twice */
#define third_bucket_size 2039
/* defines number of buckets in hash table after doubling 3 times */
#define fourth_bucket_size 4093
/* defines number of buckets in hash table after doubling 4 times */
#define fifth_bucket_size 8191
/* defines number of buckets in hash table after doubling 5 times */
#define sixth_bucket_size 16381
/* defines number of buckets in hash table after doubling 6 times */
#define seventh_bucket_size 32749
/* defines number of buckets in hash table after doubling 7 times */
#define eighth_bucket_size 65521

/* Defines Node_T as a pointer of struct Node type */
typedef struct Node *Node_T;

/* Defines structure of symbol table */
struct SymTable
{
    /* Number of buckets */
    size_t numBuckets;
    /* Array of bucket pointers */
    Node_T *buckets;
    /* Number of bindings */
    size_t count;
};

/* Defines structure of a node */
struct Node
{
    /* binding key pointer */
    const char *key;
    /* binding value pointer */
    void *value;
    /* next node pointer */
    struct Node *next;
};

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
    const size_t HASH_MULTIPLIER = 65599;
    size_t u;
    size_t uHash = 0;

    assert(pcKey != NULL);

    for (u = 0; pcKey[u] != '\0'; u++)
        uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

    return uHash % uBucketCount;
}

/* Returns pointer to bucket with size numElements */
static Node_T *SymTable_create_buckets(size_t numElements)
{
    Node_T *buckets;
    buckets = calloc(numElements, sizeof(Node_T));
    return buckets;
}

SymTable_T SymTable_new(void)
{
    SymTable_T table;
    table = malloc(sizeof(struct SymTable));
    if (table == NULL)
        return NULL;
    table->numBuckets = initial_num_buckets;
    table->buckets = SymTable_create_buckets(initial_num_buckets);
    table->count = 0;
    if (table->buckets == NULL)
    {
        free(table);
        return NULL;
    }
    return table;
}

/* Returns nothing and frees all nodes in bucket */
static void SymTable_free_buckets(Node_T bucket)
{
    Node_T current;
    Node_T next;

    for (current = bucket; current != NULL; current = next)
    {
        next = current->next;
        free((char *)current->key);
        free(current);
    }
}

void SymTable_free(SymTable_T table)
{
    size_t i;

    assert(table != NULL);

    for (i = 0; i < table->numBuckets; i++)
    {
        SymTable_free_buckets(table->buckets[i]);
    }
    free(table->buckets);
    free(table);
}

size_t SymTable_getLength(SymTable_T table)
{
    assert(table != NULL);

    return table->count;
}

void *SymTable_replace(SymTable_T table, const char *key,
                       const void *value)
{
    size_t hash;
    Node_T current;
    void *oldValue = NULL;

    assert(table != NULL);
    assert(key != NULL);

    hash = SymTable_hash(key, table->numBuckets);

    for (current = table->buckets[hash]; current != NULL; current = current->next)
    {
        if (strcmp(current->key, key) == 0)
        {
            oldValue = current->value;
            current->value = (void *)value;
            break;
        }
    }

    return oldValue;
}

int SymTable_contains(SymTable_T table, const char *key)
{
    size_t hash;
    Node_T current;

    assert(table != NULL);
    assert(key != NULL);

    hash = SymTable_hash(key, table->numBuckets);
    for (current = table->buckets[hash]; current != NULL; current = current->next)
    {
        if (strcmp(current->key, key) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void *SymTable_get(SymTable_T table, const char *key)
{
    size_t hash;
    Node_T current;

    assert(table != NULL);
    assert(key != NULL);

    hash = SymTable_hash(key, table->numBuckets);

    for (current = table->buckets[hash]; current != NULL; current = current->next)
    {
        if (strcmp(current->key, key) == 0)
        {
            return current->value;
        }
    }

    return NULL;
}

void *SymTable_remove(SymTable_T table, const char *key)
{
    Node_T prev = NULL;
    Node_T current;
    size_t hash;
    void *value;

    assert(table != NULL);
    assert(key != NULL);

    hash = SymTable_hash(key, table->numBuckets);
    for (current = table->buckets[hash]; current != NULL; current = current->next)
    {
        if (strcmp(current->key, key) == 0)
        {
            if (prev == NULL)
            {
                table->buckets[hash] = current->next;
            }
            else
            {
                prev->next = current->next;
            }
            value = current->value;
            free((char *)current->key);
            free(current);

            table->count--;

            return value;
        }
        prev = current;
    }
    return NULL;
}

void SymTable_map(SymTable_T table, SymTableMapFunction apply, const void *param)
{
    size_t i;
    Node_T current;

    assert(table != NULL);
    assert(apply != NULL);

    for (i = 0; i < table->numBuckets; i++)
    {
        for (current = table->buckets[i]; current != NULL; current = current->next)
        {
            (*apply)(current->key, current->value, (void *)param);
        }
    }
}

/* retuns nothing and rehashes table to fit newSize of elemets */
static void SymTable_expand(SymTable_T table, size_t newSize)
{
    Node_T *newBuckets;
    Node_T current;
    Node_T next;
    size_t i;
    size_t newHash;

    assert(table != NULL);

    newBuckets = SymTable_create_buckets(newSize);
    if (newBuckets == NULL)
    {
        return;
    }
    for (i = 0; i < table->numBuckets; i++)
    {
        for (current = table->buckets[i]; current != NULL; current = next)
        {
            next = current->next;
            newHash = SymTable_hash(current->key, newSize);
            current->next = newBuckets[newHash];
            newBuckets[newHash] = current;
        }
    }
    free(table->buckets);

    table->buckets = newBuckets;
    table->numBuckets = newSize;
}

int SymTable_put(SymTable_T table, const char *key, const void *value)
{
    size_t hash;
    const char *keyCopy;
    Node_T new;

    assert(table != NULL);
    assert(key != NULL);

    if (SymTable_contains(table, key))
    {
        return 0;
    }

    keyCopy = malloc(strlen(key) + 1);
    if (keyCopy == NULL)
    {
        return 0;
    }
    strcpy((char *)keyCopy, key);

    new = (Node_T)malloc(sizeof(struct Node));
    if (new == NULL)
    {
        free((char *)keyCopy);
        return 0;
    }

    /* if (table->count == table->numBuckets)
    {
        switch (table->numBuckets)
        {
        case (initial_num_buckets):
            SymTable_expand(table, second_bucket_size);
            break;
        case (second_bucket_size):
            SymTable_expand(table, third_bucket_size);
            break;
        case (third_bucket_size):
            SymTable_expand(table, fourth_bucket_size);
            break;
        case (fourth_bucket_size):
            SymTable_expand(table, fifth_bucket_size);
            break;
        case (fifth_bucket_size):
            SymTable_expand(table, sixth_bucket_size);
            break;
        case (sixth_bucket_size):
            SymTable_expand(table, seventh_bucket_size);
            break;
        case (seventh_bucket_size):
            SymTable_expand(table, eighth_bucket_size);
            break;
        default:
            break;
        }
    }
*/
    hash = SymTable_hash(keyCopy, table->numBuckets);

    new->key = keyCopy;
    new->value = (void *)value;
    new->next = table->buckets[hash];
    table->buckets[hash] = new;

    table->count++;
    return 1;
}
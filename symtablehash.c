/*--------------------------------------------------------------------*/
/* symtablehash.c (HashTable implementation)                          */
/* Author:                                                            */
/*--------------------------------------------------------------------*/

#include "symtable.h"
#define initial_num_buckets 509

typedef struct Node *Node_T;

struct SymTable
{
    size_t numBuckets;
    Node_T *buckets;
    size_t count;
};

struct Node
{
    const char *key;
    void *value;
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

    if (table->count == table->numBuckets)
    {
        switch (table->numBuckets)
        {
        case (509):
            SymTable_expand(table, 1021);
            break;
        case (1021):
            SymTable_expand(table, 2039);
            break;
        case (2039):
            SymTable_expand(table, 4093);
            break;
        case (4093):
            SymTable_expand(table, 8191);
            break;
        case (8191):
            SymTable_expand(table, 16381);
            break;
        case (16381):
            SymTable_expand(table, 32749);
            break;
        case (32749):
            SymTable_expand(table, 65521);
            break;
        default:
            break;
        }
    }

    hash = SymTable_hash(keyCopy, table->numBuckets);

    new->key = keyCopy;
    new->value = (void *)value;
    new->next = table->buckets[hash];
    table->buckets[hash] = new;

    table->count++;
    return 1;
}
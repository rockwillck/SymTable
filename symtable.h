#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct SymTable *SymTable_T;

/* SymTable_new takes no parameters and returns new table
initizing the first pointer to NULL and count to 0. */
SymTable_T SymTable_new(void);

/* SymTable_free takes  a symbol table as a parameter, frees all keys, nodes,
and finally the table itself. The function returns nothing. */
void SymTable_free(SymTable_T table);

/* SymTable_getLength takes a symbol table as its parameter and
returns the count of nodes in the linked list. */
size_t SymTable_getLength(SymTable_T table);

/* SymTable_put takes a symbol table, a key, and value as parameters.
It creates a new node at the front of the linked list containing a copy
of the key and the given value. Returns 0 if the key already exists or
memory allocation fails, and 1 if successful. */
int SymTable_put(SymTable_T table, const char *key,
                 const void *value);

/* SymTable_replace takes a symbol table, a key, and a value as parameters. Replaces the old value
the key pointed too with the new value and returns the old value */
void *SymTable_replace(SymTable_T table, const char *key,
                       const void *value);

/* SymTable_contains takes a symbol table and key as parameters, compares the key passed in
to each key in the symbol table. If the key is found in the symbol table
returns 1 otherwise returns 0. */
int SymTable_contains(SymTable_T table, const char *key);

/* SymTable_get takes symbol table and key as parameters, compares the key passed in
to each key in the symbol table. If the key is found in the symbol table
returns the value of the key. If the key is not found returns NULL. */
void *SymTable_get(SymTable_T table, const char *key);

/* SymTable_remove takes symbol table and key as parameters, compares the key passed in
to each key in the symbol table. If the key is found in the symbol table removes node and
returns the value that was removed. If the key is not found in the symbol table returns NULL. */
void *SymTable_remove(SymTable_T table, const char *key);

/* Defines a function pointer type that take in a key, value, and extra parameter */
typedef void (*SymTableMapFunction)(const char *key, void *value,
                                    void *param);
/* SymTable_map takes a symbol table, apply function and extra parameter. Iterates through 
each node in the symbol table and applies SymTableMapFunction to each element. Returns nothing */
void SymTable_map(SymTable_T table, SymTableMapFunction apply,
                  const void *param);

#endif

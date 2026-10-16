#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct SymTable* SymTable_T;

SymTable_T SymTable_new(void);

void SymTable_free(SymTable_T table);

size_t SymTable_getLength(SymTable_T table);

int SymTable_put(SymTable_T table, const char* key, 
  const void* value);

void* SymTable_replace(SymTable_T table, const char* key, 
  const void* value);

int SymTable_contains(SymTable_T table, const char* key);

void* SymTable_get(SymTable_T table, const char* key);

void* SymTable_remove(SymTable_T table, const char* key);

typedef void (*SymTableMapFunction)(const char* key, void* value, 
   void* param);

void SymTable_map(SymTable_T table, SymTableMapFunction apply, 
  const void* param);

#endif
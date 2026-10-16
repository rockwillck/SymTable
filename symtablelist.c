/*--------------------------------------------------------------------*/
/* symtablelist.c (LinkedList implementation)                         */
/* Author:                                                            */
/*--------------------------------------------------------------------*/

#include "symtable.h"

struct SymTableNode {
   const char* key;
   void* value;
   struct SymTableNode* next;
};

typedef struct SymTableNode* Node_T;

struct SymTable {
   Node_T first;
   size_t count;
};

SymTable_T SymTable_new(void)
{
   SymTable_T table;

   table = (SymTable_T)malloc(sizeof(struct SymTable));
   if (table == NULL) {
      return NULL;
   }

   table->first = NULL;
   table->count = 0;
   return table;
}

void SymTable_free(SymTable_T table) {
   Node_T current;
   Node_T next;

   assert(table != NULL);

   for (current = table->first; current != NULL; current = next) {
      next = current->next;
      free((char*)current->key);
      free(current);
   }
   free(table);
}

size_t SymTable_getLength(SymTable_T table) {
   assert(table != NULL);

   return table->count;
}

int SymTable_put(SymTable_T table, const char* key, const void* value) {
   Node_T new;
   const char* keyCopy;
   
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

   new = (Node_T)malloc(sizeof(struct SymTableNode));
   if (new == NULL) {
      free((char*)keyCopy);
      return 0;
   }
   
   new->key = keyCopy;
   new->value = (void*)value;
   new->next = table->first;
   table->first = new;
   table->count++;
   return 1;
}

void* SymTable_replace(SymTable_T table, const char* key, 
   const void* value) {
   Node_T current;
   void* oldValue = NULL;

   assert(table != NULL);
   assert(key != NULL);

   for (current = table->first; current != NULL; 
      current = current->next) {
      if (strcmp(current->key, key) == 0) {
         oldValue = current->value;
         current->value = (void*)value;
         break;
      }
   }
   return oldValue;
}

int SymTable_contains(SymTable_T table, const char *key) {
   Node_T current;

   assert(table != NULL);
   assert(key != NULL);

   for (current = table->first; current != NULL; 
      current = current->next) {
      if (strcmp(current->key, key) == 0) {
         return 1;
      }
   }
   return 0;
}

void *SymTable_get(SymTable_T table, const char *key) {
   Node_T current;

   assert(table != NULL);
   assert(key != NULL);

   for (current = table->first; current != NULL; 
      current = current->next) {
      if (strcmp(current->key, key) == 0) {
         return current->value;
      }
   }
   return NULL;
}

void *SymTable_remove(SymTable_T table, const char *key) {
   Node_T current;
   Node_T prev = NULL;
   void* value;

   assert(table != NULL);
   assert(key != NULL);

   for (current = table->first; current !=NULL; 
      current = current->next) {
      if (strcmp(current->key, key) == 0) {
         if (prev == NULL) {
            table->first = current->next;
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

void SymTable_map(SymTable_T table, SymTableMapFunction apply, 
   const void* param) {
   Node_T current;

   assert(table != NULL);
   assert(apply != NULL);

   for (current = table->first; current != NULL; 
      current = current->next) {
      (*apply)(current->key, current->value, (void*)param);
   }
}
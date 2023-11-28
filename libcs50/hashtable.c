/* 
 * hashtable.c - CS50 'hashtable' module
 *
 * see hashtable.h for more information.
 *
 * Grace Wang, April 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "hash.h"
#include "set.h"
#include "mem.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
/* none */

/**************** global types ****************/
typedef struct hashtable {
  int len;       // len of array
  set_t** sets;       // array of pointers to sets
} hashtable_t;

/**************** local functions ****************/
/* none */

/**************** heashtable_new() ****************/
/* see hashtable.h for description */
hashtable_t*
hashtable_new(const int num_slots)
{
  if (num_slots > 0) {
    // create hashtable with array of size num_slots
    hashtable_t* ht = mem_malloc(sizeof(hashtable_t));
    ht->len = num_slots;
    if (ht != NULL) {
      ht->sets = mem_calloc(ht->len, sizeof(set_t*));
      return ht;
    }
  }
  return NULL; // if num_slots is less than or equal to 0
}

/**************** hashtable_insert() ****************/
/* see hashtable.h for description */
bool
hashtable_insert(hashtable_t* ht, const char* key, void* item)
{
  if (ht != NULL && key != NULL && item != NULL) {
    // index of where the item would be hashed to
    unsigned long h = hash_jenkins(key, ht->len);
    // if no set exists in index h create new set
    if(ht->sets[h] == NULL) {
      ht->sets[h] = set_new();
    }
    // insert key and item into set
    set_insert(ht->sets[h], key, item);
    return true;
  }
  return false;
}

/**************** hashtable_find() ****************/
/* see hashtable.h for description */
void*
hashtable_find(hashtable_t* ht, const char* key) {
  if (ht != NULL && key != NULL) {
    // index of set containing key
    unsigned long h = hash_jenkins(key, ht->len);
    return set_find(ht->sets[h], key);
  }
  return NULL;
}

/**************** hashtable_print() ****************/
/* see hashtable.h for description */
void
hashtable_print(hashtable_t* ht, FILE* fp,
                void (*itemprint)(FILE* fp, const char* key, void* item))
{
  if (fp != NULL) {
    if (ht != NULL) {
      // for each set in ht, print it on a separate line
      for (int i = 0; i < ht->len; i++) {
        set_print(ht->sets[i], fp, itemprint);
        fputc('\n', fp);
      }
    } else {
      fputs("(null)", fp);     // if ht is null
    }
  }
}

/**************** hashtable_iterate() ****************/
/* see hashtable.h for description */
void
hashtable_iterate(hashtable_t* ht, void* arg,
                  void (*itemfunc)(void*arg, const char* key, void* item))
{
  if (ht != NULL && itemfunc != NULL) {
    // goes through each set in ht
    for (int i = 0; i < ht->len; i++) {
      // goes through each item in the set
      set_iterate(ht->sets[i], arg, itemfunc);
    }
  }
}

/**************** hashtable_delete() ****************/
/* see hashtable.h for description */
void
hashtable_delete(hashtable_t* ht, void (*itemdelete)(void* item))
{
  if (ht != NULL) {
    // goes through each set in ht
    for (int i = 0; i < ht->len; i++) {
      if(ht->sets[i] != NULL) {
        // deletes each item in set
        set_delete(ht->sets[i], itemdelete);
      }
    }
    mem_free(ht->sets);     // frees array
    mem_free(ht);     // frees hashtable
  }
}

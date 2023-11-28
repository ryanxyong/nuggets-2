  
/* 
 * counters.c - CS50 'counters' module
 *
 * see counters.h for more information.
 *
 * Grace Wang, April 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "counters.h"
#include "mem.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct countnode {
  int count;       // pointer to data for this item
  int key;
  struct countnode* next;       // link to next node
} countnode_t;

/**************** global types ****************/
typedef struct counters {
  struct countnode* head;       // head of the list of items in counter
} counters_t;

/**************** local functions ****************/
/* not visible outside this file */
static countnode_t* countnode_new(const int key);

/**************** counters_new() ****************/
/* see counters.h for description */
counters_t*
counters_new(void)
{
  counters_t* counter = mem_malloc(sizeof(counters_t));

  if (counter == NULL) {
    return NULL;
  } else {
    // initialize contents of counters structure
    counter->head = NULL;
    return counter;
  }
}

/**************** counters_add() ****************/
/* see counters.h for description */
int
counters_add(counters_t* ctrs, const int key)
{
  // defensive conditionals
  if (ctrs != NULL && key >= 0) {
    countnode_t* curr = ctrs->head;
    // loop through to find key
    while (curr != NULL) {
      if(curr->key == key) {
        // increase count by 1 and return
        curr->count = curr->count + 1;
        return curr->count;
      }
      curr = curr->next;
    }
    // if key was not able to be found
    if (curr == NULL){
      // create new node with the key
      curr = countnode_new(key);
      curr->next = ctrs->head;
      ctrs->head = curr;
      return curr->count;
    }
  }
  return 0;
}

/**************** counters_get() ****************/
/* see counters.h for description */
int
counters_get(counters_t* ctrs, const int key)
{
  // defensive conditionals
  if (ctrs != NULL) {
    // loop through until key is found
   countnode_t* curr = ctrs->head;
    while (curr != NULL) {
      if (curr->key == key) {
        return curr->count;
      }
      curr = curr->next;
    }
  }
  return 0;
}

/**************** counters_set() ****************/
/* see counters.h for description */
bool
counters_set(counters_t* ctrs, const int key, const int count)
{
  // defensixe conditionals
  if (ctrs != NULL && key >= 0 && count >= 0) {
    // if key is not already in ctrs
    if (counters_get(ctrs, key) == 0) {
      // create node with key and add to ctrs
      countnode_t* new = countnode_new(key);
      new->count = count;       // set count
      new->next = ctrs->head;
      ctrs->head = new;
      return true;
    } else {
      // find key and change count
      countnode_t* curr = ctrs->head;
      while (curr != NULL) {
        if (curr->key == key) {
          curr->count = count;
          return true;
        }
        curr = curr->next;
      }
    }
  }
  return false;
}

/**************** counters_print() ****************/
/* see counters.h for description */
void
counters_print(counters_t* ctrs, FILE* fp)
{
  // defensive conditionals
  if (fp != NULL) {
    if (ctrs != NULL) {
      fputc('{',fp);
      // loops through each node and prints key and count
      for (countnode_t* node = ctrs->head; node != NULL; node = node->next) {
        fprintf(fp, "%d", node->key);
        fputc('=', fp);
        fprintf(fp, "%d", node->count);
        fputc(',', fp);
      }
      fputc('}', fp);
    } else {
      fputs("(null)", fp);
    }
  }
}

/**************** counters_iterate() ****************/
/* see counters.h for description */
void
counters_iterate(counters_t* ctrs, void* arg, 
                      void (*itemfunc)(void* arg, 
                                       const int key, const int count))
{
  // defensive conditionals
  if (ctrs != NULL && itemfunc != NULL) {
    // loop through ctrs and call itemfunc on each node
    for (countnode_t* node = ctrs->head; node != NULL; node = node->next) {
      (*itemfunc)(arg, node->key, node->count);
    }
  }
}

/**************** counters_delete() ****************/
/* see counters.h for description */
void
counters_delete(counters_t* ctrs)
{
  if (ctrs != NULL) {
    // loop through each node and free it
    for (countnode_t* node = ctrs->head; node != NULL; ) {
      countnode_t* next = node->next;
      mem_free(node);
      node = next;
    }
    // free ctrs
    mem_free(ctrs);
  }

  #ifdef MEMTEST
  mem_report(stdout, "End of counters_delete");
  #endif
}

/**************** countnode_new ****************/
/* Allocate and initialize a countnode */
static countnode_t*
countnode_new(const int key)
{
  // allocate memory
  countnode_t* node = mem_malloc(sizeof(countnode_t));

  if (node == NULL) {
    return NULL;
  } else {
    node->count = 1;      // set count to 1
    node->key = key;       // set key
    return node;
  }
}


/* 
 * set.c - CS50 'set' module
 * 
 * see set.h for more information
 *
 * Grace Wang, April 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "set.h"
#include "mem.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct setnode {
  void* item;       // pointer to data for this item
  char* key;       // pointer to key for this item
  struct setnode *next;       // link to next node
} setnode_t;

/**************** global types ****************/
typedef struct set {
  struct setnode *head;       // head of the list of items in set
} set_t;

/**************** local functions ****************/
/* not visible outside this file */
static setnode_t* setnode_new(const char* key, void* item);

/**************** set_new() ****************/
/* see set.h for description */
set_t*
set_new(void)
{
  set_t* set = mem_malloc(sizeof(set_t));

  if (set == NULL) {
    return NULL;              // error allocating set
  } else {
    // initialize contents of set structure
    set->head = NULL;
    return set;
  }
}

/**************** set_insert() ****************/
/* see set.h for description */
bool
set_insert(set_t* set, const char* key, void* item)
{
  if (set != NULL && key != NULL && item != NULL) {
    // if the key does not already exist in the set
    if (set_find(set, key) == NULL) {
      //create new set with key and item
      setnode_t* new = setnode_new(key, item);  
      if (new != NULL) {
        // add it to the head of the list
        new->next = set->head;
        set->head = new;
        return true;
      }
    }
  }
  
  #ifdef MEMTEST
    mem_report(stdout, "After set_insert");
  #endif
  return false;
}

/**************** set_find() ****************/
/* see set.h for description */
void*
set_find(set_t* set, const char* key)
{
  // defensive conditionals
  if(set == NULL || key == NULL) {
    return NULL;
  } else if(set->head == NULL) {
    return NULL;
  } else {
    // go through the set and check if there is a match
    setnode_t* curr = set->head;
    while(curr != NULL) {
      if(strcmp(curr->key, key) == 0) {
        return curr->item;    // returns if it is a match
      } else {
        curr = curr->next;
      }
    }
  }
  return NULL;
}

/**************** set_print() ****************/
/* see set.h for description */
void
set_print(set_t* set, FILE* fp,
          void (*itemprint)(FILE* fp, const char* key, void* item) )
{
  // defensize conditionals
  if (fp != NULL) {
    if (set != NULL) {
      fputc('{', fp);
      // loops through to each setnode
      for (setnode_t* node = set-> head; node != NULL; node = node->next) {
        //print this node
        if (itemprint != NULL) {
          //print the node's item and key
          (*itemprint)(fp, node->key, node->item);
          fputc(',',fp);
        }
      }
      fputc('}',fp);
    } else {
      fputs("(null)", fp);
    }
  }
}

/**************** set_iterate() ****************/
/* see set.h for description */
void
set_iterate(set_t* set, void* arg,
                 void (*itemfunc)(void* arg, const char* key, void* item) )
{
  // defensive conditioals 
  if (set != NULL && itemfunc != NULL) {
    // loops through each setnode and calls itemfunc
    for (setnode_t* node = set->head; node != NULL; node = node->next) {
      (*itemfunc)(arg, node->key, node->item);
    }
  }
}

/**************** set_delete() ****************/
/* see set.h for description */
void
set_delete(set_t* set, void (*itemdelete)(void* item) )
{
  if (set !=NULL) {
    // loops through each setnode to free node
    for (setnode_t* node = set->head; node != NULL; ){
      if (itemdelete != NULL) {
        (*itemdelete)(node->item);      // frees each item
      }
      setnode_t* next = node->next;     // stores next node
      mem_free(node->key);      // frees the key
      mem_free(node);      // frees the node
      node = next;     // next node
    }

    mem_free(set);      //frees the set
  }

  #ifdef MEMTEST
  mem_report(stdout, "End of set_delete");
  #endif
}
  
/**************** setnode_new ****************/
/* Allocate and initialize a bagnode */
static setnode_t*  // not visible outside this file
setnode_new(const char* key, void* item)
{
  setnode_t* node = mem_malloc(sizeof(setnode_t));
  
  if (node == NULL) {
    // error allocating memory for node; return error
    return NULL;
  } else {
    // creating memory for string
    node->key = malloc(strlen(key)+1);  // referenced from trees7.c
    if(node->key == NULL) {
      mem_free(node);
      return NULL;
    } else {
      strcpy(node->key, key);    // copy over key
      node->item = item;
      node->next = NULL;
      //mem_free((void*)key);     // free key that was passed
      return node;
    }
  }
}
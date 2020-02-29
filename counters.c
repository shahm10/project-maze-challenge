/* 
 * counters.c - CS50 counters module
 *
 * A "counter set" is a set of counters, each distinguished by an integer key.
 * It's a set - each key can only occur once in the set - but instead of
 * storing (key,item) pairs, it tracks a counter for each key.  It starts
 * empty. Each time `counters_add` is called on a given key, that key's
 * counter is incremented. The current counter value can be retrieved by
 * asking for the relevant key.
 * 
 * Evan Fu
 * CS50, January 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "counters.h"
#include "memory.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct counter { //Node
    int key;
    int count;	      // Count for keys
    struct counter *next;	  // link to next node
} counter_t;

/**************** global types ****************/
typedef struct counters {
    struct counter *head;
} counters_t; // opaque to users of the module

/**************** global functions ****************/
/* that is, visible outside this file */
/* see counters.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static counter_t *counter_new(const int key);
static counter_t* counters_find(counters_t *ctrs, const int key);

/**************** counters_new() ****************/
/* Create a new (empty) counter structure.
 *
 * We return:
 *   pointer to a new counterset; NULL if error. 
 * We guarantee:
 *   counterset is intialized empty.
 * Caller is responsible for:
 *   later calling counters_delete();
 */
counters_t *counters_new(void) {
    counters_t *counters = malloc(sizeof(counters_t));

    if (counters == NULL) {
        return NULL; // error allocating counterset
    } else {
        // initialize contents of counters structure
        counters->head = NULL;
        return counters;
    }
}

/**************** counter_new ****************/
/* Allocate and initialize a counterset node */
// the 'static' modifier means this function is not visible 
// outside this file
static counter_t* counter_new(const int key) {
  counter_t *node = malloc(sizeof(counter_t));

  if (node == NULL) {
    // error allocating memory for node; return error
    return NULL;
  } else {
    node->key = key;
    node->count = 0;
    node->next = NULL;
    return node;
  }
}

/**************** counters_add ****************/
/* Increment the counter indicated by key.
 * 
 * Caller provides:
 *   valid pointer to counterset, and key(must be >= 0)
 * We return:
 *   the new value of the counter related to the indicated key.
 *   0 on error (if ctrs is NULL or key is negative)
 * We guarantee:
 *   counter values will always be >= 1.
 * We do:
 *  ignore if ctrs is NULL or key is negative.
 *  if the key does not yet exist, create a counter for it and initialize to 1.
 *  if the key does exist, increment its counter by 1.
 */
int counters_add(counters_t *ctrs, const int key) {
    if (ctrs == NULL || key < 0) {
        return 0;
    } else {
        if (counters_get(ctrs, key) > 0) { // if key does exist
            counter_t *node = counters_find(ctrs, key);
            node->count++; // increment the key's counter
            return counters_get(ctrs, key);
        } else { // if key does not exist
            // allocate a new node to be added to counterset
            counter_t *new = counter_new(key);
            if (new != NULL) {
                new->count = 1; // initialize counter to 1
                // add it to the head of the list
                new->next = ctrs->head;
                ctrs->head = new;
                return new->count;
            }
        }
        return 0;
    }
}

/**************** counters_get ****************/
/* Return current value of counter associated with the given key.
 *
 * Caller provides:
 *   valid pointer to counterset, and key(must be >= 0)
 * We return:
 *   current value of counter associte with the given key, if present,
 *   0 if ctrs is NULL or if key is not found.   
 * Note:
 *   counterset is unchanged as a result of this call.
 */
int counters_get(counters_t *ctrs, const int key) {
    if (ctrs == NULL) {
        return 0; //  bad counterset
    } else {
        // iterate through each node and find matching key
        for (counter_t *node = ctrs->head; node != NULL; node = node->next) {
            if (node->key == key)
                return node->count;
        }
        return 0;
    }
}

/**************** counters_set ****************/
/* Set the current value of counter associated with the given key.
 *
 * Caller provides:
 *   valid pointer to counterset, 
 *   key(must be >= 0), 
 *   counter value(must be >= 0).
 * We return:
 *   false if ctrs is NULL, if key < 0 or count < 0, or if out of memory.
 *   otherwise returns true.
 * We do:
 *   If the key does not yet exist, create a counter for it and initialize to
 *   the given value. 
 *   If the key does exist, update its counter value to the given value.
 */
bool counters_set(counters_t *ctrs, const int key, const int count) {
    if (ctrs == NULL || key < 0 || count < 0) {
        return false;
    } else {
        if (counters_get(ctrs, key) > 0) { // if key does exist
            counter_t *node = counters_find(ctrs, key);
            node->count = count;
        } else {
            counter_t *new = counter_new(key);
            new->count = count;
            // add it to the head of the list
            new->next = ctrs->head;
            ctrs->head = new;
        }
    }
    return true;
}

/**************** counters_print ****************/
/* Print all counters; provide the output file.
 *
 * Caller provides:
 *   valid pointer to counterset, 
 *   FILE open for writing.
 * We print:
 *   Nothing if NULL fp. 
 *   "(null)" if NULL ctrs.
 *   otherwise, comma=separated list of key=counter pairs, all in {brackets}.
 */
void counters_print(counters_t *ctrs, FILE *fp) {
    if (fp != NULL) {
        if (ctrs == NULL) { 
            fputs("(null)", fp); // null counterset
        } else {
            fputc('{', fp);
            for (counter_t *node = ctrs->head; node != NULL; node = node->next) {
                int key = node->key;
                int count = node->count;
                fprintf(fp, "(%d,%d)", key, count);
                if (node->next != NULL) { // separate by commas
                    fputc(',', fp);
                }
            }
            fputs("}\n", fp);
        }
    }
}

/**************** counters_iterate ****************/
/* Iterate over all counters in the set.
 *
 * Caller provides:
 *   valid pointer to counterset, 
 *   arbitrary void*arg,
 *   valid pointer to itemfunc that can handle one item.
 * We do:
 *   nothing, if ctrs==NULL or itemfunc==NULL.
 *   otherwise, call itemfunc once for each item, with (arg, key, count).
 * Note:
 *   the order in which items are handled is undefined.
 *   the counterset is unchanged by this operation.
 */
void counters_iterate(counters_t *ctrs, void *arg, void (*itemfunc)(void *arg, 
    const int key, const int count)) {
    if (ctrs != NULL && itemfunc != NULL) {
        for (counter_t *node = ctrs->head; node != NULL; node = node->next) {
            (*itemfunc)(arg, node->key, node->count);
        }
    }
}

/**************** counters_delete ****************/
/* Delete the whole counterset.
 *
 * Caller provides:
 *   a valid pointer to counterset.
 * We do:
 *   we ignore NULL ctrs.
 *   we free all memory we allocate for this counterset.
 */
void counters_delete(counters_t *ctrs) {
    if (ctrs != NULL) {
        for (counter_t *node = ctrs->head; node != NULL; ) {
            counter_t *next = node->next;
            free(node); // free current node and iterate to next one
            node = next;
        }
    }
    free(ctrs);
}

/**************** counters_find() ****************/
/* Helper function to find node in counterset given a key
 *
 * Caller provides:
 *   a valid pointer to counterset.
 *   a valid key
 * We do:
 *   we ignore NULL/empty ctrs and bad keys.
 *   we return the node associated with the provided key.
 */
static counter_t* counters_find(counters_t *ctrs, const int key) {
    if (ctrs == NULL || key <= 0) {
        return NULL; // bad counterset
    } else if (ctrs->head == NULL) {
        return NULL; // counterset is empty
    } else {
        for (counter_t *node = ctrs->head; node != NULL; node = node->next) {
            if (node->key == key)
                return node;
        }
        return NULL;
    }
    return NULL;
}
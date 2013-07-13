//
//  msg_queue.c
//  APNSServer
//
//  Created by Tristan Seifert on 12/07/2013.
//  Copyright (c) 2013 Squee! Apps. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msg_queue.h"

extern inline msg_queue_entry *msg_queue_find_last_queue_entry();
extern inline int msg_queue_allocate_memory(int numItems);
extern inline msg_queue_entry *msg_queue_find_empty_entry();

// hooray for linked lists!
static msg_queue_entry *msg_queue = NULL;

/*
 * Sets up memory needed for the queue.
 */
void msg_queue_begin() {
    msg_queue = malloc(sizeof(msg_queue_entry));
    memset(msg_queue, 0x00, sizeof(msg_queue_entry));
}

/*
 * Inserts a message into the queue.
 */
int msg_queue_insert(push_msg *message) {
    msg_queue_entry *entry = msg_queue_find_empty_entry();
    
    // check that we actually have memory for the entry
    if(entry == 0) {
        return -1;
    }
    
#ifdef LINKED_LIST_DEBUG
    printf("Inserted item 0x%X at 0x%X.\n", (int) message, (int) entry);
#endif
    
    entry->content = message;
    
    return 0;
}

/*
 * Returns a reference to the linked list.
 */
volatile msg_queue_entry *msg_queue_get_ptr() {
    volatile msg_queue_entry *ptr = msg_queue;
    return ptr;
}

#pragma mark - Linked lists
/*
 * Searches through the queue to find the last entry.
 */
inline msg_queue_entry *msg_queue_find_last_queue_entry() {
    // check if queue has any memory allocated
    if(msg_queue == NULL) {
        return msg_queue;
    }
    
    // If there's stuff in the queue, iterate until we find the last entry
    msg_queue_entry *queue_read_ptr = msg_queue;
    while(queue_read_ptr != NULL) {
        msg_queue_entry *entry = queue_read_ptr;
        
        if(entry->next_entry != NULL) {
            queue_read_ptr = (msg_queue_entry *) entry->next_entry;
        } else {
            // we've found the last item, break out of the loop
            break;
        }
    }
    
    // Return last item
    return queue_read_ptr;
}

/*
 * Allocates enough memory (and sets up structs with no content ptr) for the
 * specified number of items.
 */
inline int msg_queue_allocate_memory(int numItems) {
    // loop through number of items to allocate, create struct and stuff
    for (int i = 0; i < numItems; i++) {
        msg_queue_entry *end_of_queue = msg_queue_find_last_queue_entry();
        msg_queue_entry *newItem =  malloc(sizeof(msg_queue_entry));
        memset(newItem, 0x00, sizeof(msg_queue_entry));
        
        if(newItem == 0) {
            return -1; // out of memory
        }
        
        newItem->content = NULL;
        newItem->next_entry = NULL;
        
        if(end_of_queue != NULL) {
            end_of_queue->next_entry = (struct msg_queue_entry *) newItem;
            end_of_queue = newItem;
        } else {
            msg_queue = newItem;
        }
    }
    
    return 0;
}

/*
 * Finds the first empty (i.e. null content ptr) entry in the list
 */
inline msg_queue_entry *msg_queue_find_empty_entry() {
    // check if queue has any memory allocated
    if(msg_queue == NULL) {
#ifdef LINKED_LIST_DEBUG
        printf("Had to allocate message queue memory.\n");
#endif
        
        msg_queue_allocate_memory(1); // allocate memory for one item
        return msg_queue;
    }
    
#ifdef LINKED_LIST_DEBUG
    int i = 0;
#endif
    
    msg_queue_entry *queue_ptr = msg_queue;
    while(queue_ptr != NULL) {
#ifdef LINKED_LIST_DEBUG
        printf("Checking item %i at 0x%X\n", i, (int) queue_ptr);
        i++;
#endif
        
        if(queue_ptr->content == NULL) {
            break; // this item has no content, exit loop
        } else { // item has content assigned
            if(queue_ptr->next_entry != NULL) { // there's a next item
                queue_ptr = (msg_queue_entry *) queue_ptr->next_entry;
            } else { // we're at the end of the list, so allocate some memory
                msg_queue_entry *newItem =  malloc(sizeof(msg_queue_entry));
                memset(newItem, 0x00, sizeof(msg_queue_entry));
                
                if(newItem == 0) {
                    return 0; // out of memory err
                }
                
                // set it's content and next entry ptr to null
                newItem->content = NULL;
                newItem->next_entry = NULL;
                
                // update ptr of current item to point to new one
                queue_ptr->next_entry = (struct msg_queue_entry *) newItem;
                queue_ptr = newItem;
                break;
            }
        }
    }
    
    return queue_ptr;
}

/*
 * Marks the specified entry as "deleted," frees it's content memory and marks
 * it for later re-use when needed.
 */
inline void msg_queue_delete_entry(msg_queue_entry *entry) {
    free(entry->content);
    entry->content = NULL;
}

/*
 * Prints a pretty representation of the message queue to the console.
 */
void msg_queue_print_pretty_rep() {
    msg_queue_entry *entry = msg_queue;
    
    if(msg_queue == NULL) {
        printf("No items in message queue.\n");
        return;
    }
    
    int i = 0;
    while(entry != NULL) {
        if(entry->content != NULL) {
            printf("Item %.4i: 0x%0.8X, points to 0x%.8X, content at 0x%.8X @\n", i, (int) entry, (int) entry->next_entry, (int)entry->content);
        } else {
            printf("Item %.4i: 0x%0.8X, points to 0x%.8X, content at 0x%.8X *\n", i, (int) entry, (int) entry->next_entry, (int)entry->content);            
        }
        
        i++;
        entry = (msg_queue_entry *) entry->next_entry;
    }
}
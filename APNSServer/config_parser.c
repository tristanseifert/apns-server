//
//  config_parser.c
//  APNSServer
//
//  Created by Tristan Seifert on 14/07/2013.
//  Copyright (c) 2013 Squee! Apps. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

#include "ansi_terminal_defs.h"
#include "config_parser.h"

config_entry *config_list  = NULL;

volatile config_entry *config_get_ptr();
extern inline config_entry *config_find_last_entry();
extern inline int config_allocate_memory(int numItems);
void config_print_pretty_rep();

void config_parse(char *path) {
    FILE *fp = fopen(path, "r");
    
    if(!fp) {
        perror(ANSI_COLOR_RED ANSI_BOLD "Error opening config file");
        printf(ANSI_RESET);
        exit(255);
    }
    
//    config_list = malloc(sizeof(config_entry));
//    memset(config_list, 0x00, sizeof(config_entry));
    
    // if you really need 1024 columns in the config file... well, no.
    char *lineBuffer = malloc(1024);
    char *lineBufferRead = lineBuffer;
    
    while (1) {
        memset(lineBuffer, 0x00, 1024);
        
        lineBufferRead = lineBuffer;
        if(fgets(lineBuffer, 1024, fp) == NULL) break; // read 'til EOF
        
        // Ignore comments and blank lines
        if(*lineBufferRead != '#' && *lineBufferRead != '\n') {
            char *locOfColon = memchr(lineBuffer, ':', 1024);
            int numBytesKey = (int)(locOfColon - lineBuffer);
            
            char *key = malloc(numBytesKey + 2);
            memset(key, 0x00, numBytesKey + 2);
            memcpy(key, lineBuffer, numBytesKey);
            
            char *locOfNewline = memchr(lineBuffer, '\n', 1024);
            int numBytesValue = (int)(locOfNewline - lineBuffer - numBytesKey - 1);
            
            char *value = malloc(numBytesValue + 2);
            memset(value, 0x00, numBytesValue + 2);
            memcpy(value, locOfColon + 1, numBytesValue);
            
            util_string_trim(value);
            
            config_allocate_memory(1); // allocate memory for next item
            config_entry *entry = config_find_last_entry();
            
            entry->key = key;
            
            if(*value == '"') {
                entry->type = kConfigValueString;
                
                char *newVal = util_config_strip_quotes(value);
                
                entry->data = newVal;
            } else {
                entry->type = kConfigValueNumber;

                // parse int
                entry->data = (void *) strtoll(value, (char **) NULL, 10);
            }
            
            // clean up memory
            free(value);
        }
    }
    
    config_allocate_memory(1); // allocate one more item
    
    free(lineBuffer);
    fclose(fp);
    
    config_print_pretty_rep();
}

void *config_get_value(char * key) {
    config_entry *entry = config_list;
    
    void *value = NULL;
    
    while(entry->next_entry != NULL) {
        // we've found the appropriate struct
        if(strcmp(key, entry->key) == 0) {
            value = entry->data;
            break;
        }
        
        entry = (config_entry *) entry->next_entry;
    }
    
    return value;
}

long long config_get_number(char * key) {
    config_entry *entry = config_list;
    
    void *value = NULL;
    
    while(entry->next_entry != NULL) {
        // we've found the appropriate struct
        if(strcmp(key, entry->key) == 0) {
            value = entry->data;
            break;
        }
        
        entry = (config_entry *) entry->next_entry;
    }
    
    return (long long) value;
}

#pragma mark - Linked lists
/*
 * Returns a reference to the linked list.
 */
volatile config_entry *config_get_ptr() {
    volatile config_entry *ptr = config_list;
    return ptr;
}

/*
 * Searches through the queue to find the last entry.
 */
inline config_entry *config_find_last_entry() {
    // check if queue has any memory allocated
    if(config_list == NULL) {
        return config_list;
    }
    
    // If there's stuff in the queue, iterate until we find the last entry
    config_entry *list_read_ptr = config_list;
    while(list_read_ptr != NULL) {
        config_entry *entry = list_read_ptr;
        
        if(entry->next_entry != NULL) {
            list_read_ptr = (config_entry *) entry->next_entry;
        } else {
            // we've found the last item, break out of the loop
            break;
        }
    }
    
    // Return last item
    return list_read_ptr;
}

/*
 * Allocates enough memory (and sets up structs with no content ptr) for the
 * specified number of items.
 */
inline int config_allocate_memory(int numItems) {
    if(config_list == NULL) {
        config_list = malloc(sizeof(config_entry));
        memset(config_list, 0x00, sizeof(config_entry));
        return 0;
    }
    
    // loop through number of items to allocate, create struct and stuff
    for (int i = 0; i < numItems; i++) {
        config_entry *endOfList = config_find_last_entry();
        config_entry *newItem =  malloc(sizeof(config_entry));
        memset(newItem, 0x00, sizeof(config_entry));
        
        if(newItem == 0) {
            return -1; // out of memory
        }
        
        newItem->next_entry = NULL;
        
        if(endOfList != NULL) {
            endOfList->next_entry = (struct config_entry *) newItem;
            endOfList = newItem;
        } else {
            config_list = newItem;
        }
    }
    
    return 0;
}

/*
 * Prints a pretty representation of the message queue to the console.
 */
void config_print_pretty_rep() {
    config_entry *entry = config_list;
    
    if(config_list == NULL) {
        printf("No items in config list queue.\n");
        return;
    }
    
    int i = 0;
    while(entry != NULL) {
        printf("Item %.4i: 0x%0.8X, points to 0x%.8X, content at 0x%.8X\n", i, (int) entry, (int) entry->next_entry, (int)entry->data);
        if(entry->type == kConfigValueString) {
            printf("\t   %s: %s\n", entry->key, entry->data);
        } else {
            printf("\t   %s: %lli\n", entry->key, (long long) entry->data);
        }
        
        i++;
        entry = (config_entry *) entry->next_entry;
    }
}
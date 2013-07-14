//
//  util.c
//  APNSServer
//
//  Created by Tristan Seifert on 12/07/2013.
//  Copyright (c) 2013 Squee! Apps. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <pthread.h>

void util_string_trim(char* string) {
    // remove leading spaces
    const char* firstNonSpace = string;
    while(*firstNonSpace != '\0' && isspace(*firstNonSpace)) {
        ++firstNonSpace;
    }
    
    size_t len = strlen(firstNonSpace)+1;
    memmove(string, firstNonSpace, len);
    
    // trailing spaces
    
    char* endOfString = string + len;
    while(string < endOfString  && isspace(*endOfString)) {
        --endOfString ;
    }
    
    *endOfString = '\0';
}

char *util_config_strip_quotes(char *string) {
    int length = (int) strlen(string);
    
    char *newString = malloc(length);
    strcpy(newString, string);
    
    char *strPtr = newString;
    strPtr += length - 1;
    *strPtr = 0x00;
    return newString + 1;
}
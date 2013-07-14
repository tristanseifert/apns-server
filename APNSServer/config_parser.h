//
//  config_parser.h
//  APNSServer
//
//  Created by Tristan Seifert on 14/07/2013.
//  Copyright (c) 2013 Squee! Apps. All rights reserved.
//

#ifndef APNSServer_config_parser_h
#define APNSServer_config_parser_h

typedef enum {
    kConfigValueString,
    kConfigValueNumber
} config_type;

typedef struct {
    char *key;
    void *data;
    config_type type;
    
    struct config_entry *next_entry;
} config_entry;


void config_parse(char *path);

void *config_get_value(char * key);
long long config_get_number(char * key);

#endif

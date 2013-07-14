//
//  msg.h
//  APNSServer
//
//  Created by Tristan Seifert on 12/07/2013.
//  Copyright (c) 2013 Squee! Apps. All rights reserved.
//

#ifndef APNSServer_msg_h
#define APNSServer_msg_h

typedef struct {
    // plain text
    char* text; // text content of app
    
    // localized text
    char* localized_template;
    struct {
        char* argument0;
        char* argument1;
        char* argument2;
        char* argument3;
    } localized_arguments;
    
    char* sound; // sound to play
    int badgeNumber; // app icon badge
    char* buttonTitle; // custom button
    
    char *custPayload; // custom data passed to app
    
    char* deviceID; // device's UUID
} push_msg;

#endif

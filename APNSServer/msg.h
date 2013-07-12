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
    char* text; // text content of app
    char* sound; // sound to play
    int badgeNumber; // app icon badge
    
    char *custPayload; // custom data passed to app
    
    char* deviceID; // device's UUID
} push_msg;

#endif

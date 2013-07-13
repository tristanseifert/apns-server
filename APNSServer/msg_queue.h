//
//  msg_queue.h
//  APNSServer
//
//  Created by Tristan Seifert on 12/07/2013.
//  Copyright (c) 2013 Squee! Apps. All rights reserved.
//

#include "msg.h"

#ifndef APNSServer_msg_queue_h
#define APNSServer_msg_queue_h

typedef struct {
    push_msg *content; // the message
    struct msg_queue_entry *next_entry; // next entry (0 if none)
} msg_queue_entry;

int msg_queue_insert(push_msg *message);

// hooray for linked lists!
static msg_queue_entry *msg_queue = NULL;

#endif

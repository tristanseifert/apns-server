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
void msg_queue_begin();
void msg_queue_print_pretty_rep();
volatile msg_queue_entry *msg_queue_get_ptr();


#endif

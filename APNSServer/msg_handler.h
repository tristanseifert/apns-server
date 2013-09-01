//
//  msg_handler.h
//  APNSServer
//
//  Created by Tristan Seifert on 12/07/2013.
//  Copyright (c) 2013 Squee! Apps. All rights reserved.
//

#ifndef APNSServer_msg_handler_h
#define APNSServer_msg_handler_h

int msg_handler_begin();
void msg_handler_end(int haveMercy);

static volatile int msg_queue_writelock = 0;

#endif

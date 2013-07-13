//
//  msg_handler.c
//  APNSServer
//
//  Created by Tristan Seifert on 12/07/2013.
//  Copyright (c) 2013 Squee! Apps. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>

#include "msg_handler.h"

static int msg_handler_should_run = 0;
static pthread_t msg_handler_pthread;

/*
 * This is the code that's executed in the message handling thread
 */
void *msg_handler_thread(void *param) {
    printf("Message handler thread has started.\n");
    
    while(msg_handler_should_run) {
        
    }
    
    pthread_exit(NULL);
}

/*
 * Sets up the thread for the message handler.
 *
 * Returns 0 if successful, pthread error code otherwise.
 */
int msg_handler_begin() {
    msg_handler_should_run = 1;
    
    int error = pthread_create(&msg_handler_pthread, NULL, msg_handler_thread, NULL);
    
    if(error) {
        msg_handler_should_run = 0;
    }
    
    return error;
}

/*
 * Terminates the message handler thread. If haveMercy != 0, the thread will be
 * allowed to complete it's current task before exiting.
 */
void msg_handler_end(int haveMercy) {
    msg_handler_should_run = 0;
    
    // just kill the thread
    if(!haveMercy) {
        pthread_cancel(msg_handler_pthread);
    }
}
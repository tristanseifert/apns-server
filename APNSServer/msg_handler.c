//
//  msg_handler.c
//  APNSServer
//
//  Created by Tristan Seifert on 12/07/2013.
//  Copyright (c) 2013 Squee! Apps. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "msg_handler.h"
#include "msg_queue.h"
#include "msg.h"

#include "ssl.h"
#include "jansson.h"
#include "apns_config.h"

static int msg_handler_should_run = 0;
static pthread_t msg_handler_pthread;

void msg_handler_send_push(push_msg *message);

/*
 * This is the code that's executed in the message handling thread
 */
void *msg_handler_thread(void *param) {
    printf("Message handler thread has started.\n");
    
    while(msg_handler_should_run) {
        msg_queue_writelock = 1;
        
        // check the message queue actually exists
        if(msg_queue_get_ptr() != NULL) {
#ifdef MSG_PROCESSING_DEBUG
            msg_queue_print_pretty_rep();
#endif
            
            int numNotifsProc = 0;
            
            msg_queue_entry *read_ptr = (msg_queue_entry *) msg_queue_get_ptr();
            
            // iterate through all the items in list
            while(read_ptr != NULL) {
                if(read_ptr->content != NULL) {
                    numNotifsProc++;
                    
                    // get the notification
                    push_msg *msg = read_ptr->content;
                    
#ifdef MSG_PROCESSING_DEBUG
                    printf("Notification title \"%s\" sent to %s\n", msg->text, msg->deviceID);
#endif
                    msg_handler_send_push(msg);
                    
                    // free memory
                    free(read_ptr->content);
                    read_ptr->content = NULL;
                }
                
                // advance pointer
                read_ptr =  (msg_queue_entry *) read_ptr->next_entry;
            }
            
#ifdef MSG_PROCESSING_DEBUG
            printf("Processed %i push notifications.\n", numNotifsProc);
#endif
        } else {
#ifdef MSG_PROCESSING_DEBUG
            printf("No messages to process.\n");
#endif
        }
        
        fflush(stdout);
        
        msg_queue_writelock = 0;
        
        sleep(5);
        // usleep(100000); // wait 100 ms
    }
    
    pthread_exit(NULL);
}

/*
 * Serialises a message, then transmits it to the APNS servers.
 */
void msg_handler_send_push(push_msg *message) {
    // 		$msg = chr(0) . pack('n', 32) . pack('H*', $deviceToken) . pack('n', strlen($payload)) . $payload;
  
    // set up json dict
    json_t *apsDict = json_object();
    json_t *jsonRoot = json_object();
    
    // set up the text
    if(message->text != NULL) {
        json_object_set(apsDict, "alert", json_string(message->text));
    } if(message->sound) {
        json_object_set(apsDict, "sound", json_string(message->text));
    } if(message->badgeNumber != -1) {
        json_object_set(apsDict, "badge", json_integer(message->badgeNumber));
    } if(message->custPayload) {
        json_object_set(jsonRoot, "c", json_string(message->custPayload));
    }
    
    json_object_set(jsonRoot, "aps", apsDict);
                        
    // encode
    char *jsonText = json_dumps(apsDict, JSON_ENSURE_ASCII | JSON_COMPACT);
    printf("Encoded JSON: %s\n", jsonText);
    
    // set up buffer
    int requiredMsgBufSize = (int) (strlen(jsonText) + 512); // Length of JSON plus header buf
    char *messageBuffer = malloc(sizeof(char) * requiredMsgBufSize);
    memset(messageBuffer, 0x00, sizeof(char) * requiredMsgBufSize);
    
//    ssl_write_to_sock((SSLConn *) shared_SSL_connection, messageBuffer, requiredMsgBufSize);
    
    free(jsonText);
    free(messageBuffer);
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
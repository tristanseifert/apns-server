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
#include <time.h>

#include "msg_handler.h"
#include "msg_queue.h"
#include "msg.h"

#include "ssl.h"
#include "jansson.h"
#include "config_parser.h"

volatile uint8_t msg_handler_should_run = 0;
volatile uint8_t handler_has_quit = 0;
static pthread_t msg_handler_pthread;

void msg_handler_send_push(push_msg *message);
uint8_t* msg_handler_convert_device_token(char *token);

/*
 * This is the code that's executed in the message handling thread
 */
void *msg_handler_thread(void *param) {
    printf("Message handler thread has started.\n");
    
    while(msg_handler_should_run == 1) {
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
            printf("Processed %i push notification(s).\n\n", numNotifsProc);
#endif
        } else {
#ifdef MSG_PROCESSING_DEBUG
            printf("No messages to process.\n");
#endif
        }
        
        fflush(stdout);
        
        msg_queue_writelock = 0;
        
        usleep(((int)(config_get_number("Push_Cycle_Length"))) * 1000);
    }
    
    handler_has_quit = 1;
    
    pthread_exit(NULL);
}

/*
 * Serialises a message, then transmits it to the APNS servers.
 */
void msg_handler_send_push(push_msg *message) {  
    // set up json dict
    json_t *apsDict = json_object();
    json_t *jsonRoot = json_object();
    
    // create JSON dict
    if(message->text) {
        json_object_set(apsDict, "alert", json_string(message->text));
    } else if(message->localized_template) {
        json_t *alertDict = json_object();
        
        json_object_set(alertDict, "loc-key", json_string(message->localized_template));
        
        // we've got to have at least the 1st arg
        if(message->localized_arguments.argument0) {
            json_t *alertArgs = json_array();
            
            json_array_append(alertArgs, json_string(message->localized_arguments.argument0));
            
            if(message->localized_arguments.argument1) {
                json_array_append(alertArgs, json_string(message->localized_arguments.argument1));
            } if(message->localized_arguments.argument2) {
                json_array_append(alertArgs, json_string(message->localized_arguments.argument2));
            } if(message->localized_arguments.argument3) {
                json_array_append(alertArgs, json_string(message->localized_arguments.argument3));
            }
            
            json_object_set(alertDict, "loc-args", alertArgs);
        }
        
        json_object_set(apsDict, "alert", alertDict);
    }
    
    if(message->sound) {
        json_object_set(apsDict, "sound", json_string(message->text));
    } if(message->badgeNumber != -1) {
        json_object_set(apsDict, "badge", json_integer(message->badgeNumber));
    } if(message->custPayload) {
        json_object_set(jsonRoot, "c", json_string(message->custPayload));
    }
    
    json_object_set(jsonRoot, "aps", apsDict);
                        
    // encode
    char *jsonText = json_dumps(jsonRoot, JSON_ENSURE_ASCII | JSON_COMPACT);
#ifdef MSG_PROCESSING_DEBUG
    printf("Encoded JSON: %s\n", jsonText);
#endif
    
    size_t jsonLength = strlen(jsonText);
    
    // set up buffer
    // message format is, |COMMAND|ID|EXPIRY|TOKENLEN|TOKEN|PAYLOADLEN|PAYLOAD|
    int requiredMsgBufSize = (int) (jsonLength + 512); // Length of JSON plus header
    uint8_t *messageBuffer = malloc(sizeof(uint8_t) * requiredMsgBufSize);
    memset(messageBuffer, 0x00, sizeof(uint8_t) * requiredMsgBufSize);
    uint8_t *msgBufWrite = messageBuffer;
    
    uint16_t networkOrderTokenLength = htons(32);
    uint16_t networkOrderPayloadLength = htons(jsonLength);
    uint32_t networkOrderIdentifier = htonl(0xDEADBEEF);
    uint32_t networkOrderExpiry = htonl(((int) time(NULL)) + (3600 * 24)); // store messages 1 day
    uint8_t command = 1;
    
    // write command
    *msgBufWrite++ = command;
    
    // provider preference
    memcpy(msgBufWrite, &networkOrderIdentifier, sizeof(uint32_t));
    msgBufWrite += sizeof(uint32_t);
    
    // expiry date network order
    memcpy(msgBufWrite, &networkOrderExpiry, sizeof(uint32_t));
    msgBufWrite += sizeof(uint32_t);
    
    // token length network order
    memcpy(msgBufWrite, &networkOrderTokenLength, sizeof(uint16_t));
    msgBufWrite += sizeof(uint16_t);
    
    // write device token
    uint8_t *binaryToken = msg_handler_convert_device_token(message->deviceID);
    memcpy(msgBufWrite, binaryToken, 32);
    msgBufWrite += 32;
    
    // payload length
    memcpy(msgBufWrite, &networkOrderPayloadLength, sizeof(uint16_t));
    msgBufWrite += sizeof(uint16_t);
    
    // copy payload
    memcpy(msgBufWrite, jsonText, jsonLength);
    msgBufWrite += jsonLength; // Apple prohibits 0x00 endings
    
    // write over SSL connection
    int error = ssl_write_to_sock((SSLConn *) SSL_get_shared_context(), messageBuffer, (int)(msgBufWrite - messageBuffer));
    
    if(error <= 0) {
        printf("SSL error sending notification: %i\n", error);
    }
    
#ifdef MSG_PROCESSING_DEBUG
    printf("Binary APNS message:\n");
    uint8_t *bTkRd = messageBuffer;
    for(int i =0; i < (msgBufWrite - messageBuffer); i++) {
        printf("%.2x", *bTkRd);
        bTkRd++;
    }    
    printf("\n\n");
#endif
    
    free(jsonText);
    free(binaryToken);
    free(messageBuffer);
}

/*
 * Converts a device token in the string representation to a binary rep in
 * memory.
 */
uint8_t* msg_handler_convert_device_token(char *token) {
    int tmpi;
    char tmp[3];
    tmp[2] = 0x00;
    
    uint8_t *deviceTokenBinary = malloc(32);
    uint8_t *deviceTokenBinaryWr = deviceTokenBinary;
    memset(deviceTokenBinary, 0x00, 32);
    
    for (int i = 0; i < 32; i++) {
        tmp[0] = *token++;
        tmp[1] = *token++;
        
        sscanf(tmp, "%x", &tmpi);
        
        *deviceTokenBinaryWr = tmpi & 0xFF;
        deviceTokenBinaryWr++;
    }
    
    return deviceTokenBinary;
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
    } else {
        while(handler_has_quit == 0);
    }
}
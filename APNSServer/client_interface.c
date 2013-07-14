//
//  client_interface.c
//  APNSServer
//
//  Created by Tristan Seifert on 12/07/2013.
//  Copyright (c) 2013 Squee! Apps. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#include "jansson.h"
#include "msg.h"
#include "msg_queue.h"

#include "ansi_terminal_defs.h"
#include "apns_config.h"

static int client_socket = 0;
static int client_should_run = 0;
static pthread_t client_service_thread;

void *client_interface_listening_thread(void *param);
void *client_interface_connection_handler(void *connection);
extern inline inline char* copy_json_info(json_t *value);

/*
 * Sets up a socket on which we listen for clients. Also creates the listening
 * thread.
 */
int client_interface_set_up() {
    char ourname[HOSTNAME_BUF_SIZE+1];
    struct sockaddr_in sa;
    struct hostent *hp;
    
    memset(&sa, 0, sizeof(struct sockaddr_in));
    gethostname(ourname, HOSTNAME_BUF_SIZE);
    
    hp = gethostbyname(ourname);
    if(hp == NULL) { // we apparently don't exist - what is this, the matrix?
        perror("Finding hostname");
        return -1;
    }
    
    sa.sin_family = hp->h_addrtype;
    sa.sin_port = htons(CLIENT_LISTEN_PORT);
    
    // create socket
    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Creating listening socket");
        return 255;
    }
    
    // bind socket
    if(bind(client_socket, (const struct sockaddr *) &sa, sizeof(struct sockaddr_in)) < 0) {
        close(client_socket);
        perror("Binding listening socket");
        return 512;
    }
    
    listen(client_socket, MAX_NUM_QUEUED_CLIENTS);
    
    // Set up the listening thread
    client_should_run = 1;
    
    int error = pthread_create(&client_service_thread, NULL, client_interface_listening_thread, NULL);
    
    if(error) {
        client_should_run = 0;
        close(client_socket);
        
        perror("Creating listening thread");
        
        return error | 0x8000;
    }
    
    return 0;
}

/*
 * Waits for a connection to come in on the socket and accepts it, then
 * establishes a socket with the client.
 */
int client_interface_get_connection() {
    int t;
    if((t = accept(client_socket, NULL, NULL)) < 0) {
        return -1;
    }
    
    return t;
}

/*
 * Cleans up and closes the socket created previously.
 * If mercy == 0, the thread is just killed and socket closed.
 */
void client_interface_stop(int mercy) {
    if(!mercy) {
        pthread_cancel(client_service_thread);
        close(client_socket);
    } else {
        client_should_run = 0;
    }
}

/*
 * This thread processes connections.
 */
void *client_interface_listening_thread(void *param) {
    int t; // holds the connection we establish with clients
    
    printf("Client listening thread active. Listening for clients on port %i...\n"
           , CLIENT_LISTEN_PORT);
    fflush(stdout);
    
    while(client_should_run) {
        if((t = client_interface_get_connection()) < 0) {
            if(errno == EINTR) { // The socket might sometimes get EINTR
                continue;
            }
            
            perror("Accept client connection"); // die, we got an unknown error
        }
        
        pthread_t thread; // we don't really care about this thread later, it'll die eventually
        pthread_create(&thread, NULL, client_interface_connection_handler, (void *) &t);
    }
    
    close(client_socket);
    
    return NULL;
}

/*
 * This function is called in a new thread to process a client that connects. It
 * handles the protocol and all that other fun stuff to translate messages the
 * clients send into a push notification we can send out.
 */
void *client_interface_connection_handler(void *connection) {
    int sock = *((int *) connection);

    printf("Client connected.\n");
    
    char *msg_buf = calloc(MAX_CLIENT_MSG_SIZE + 1, sizeof(char));
    char *msg_buf_write_ptr = msg_buf;
    
    long bytes_read_total = 0;
    long bytes_read = 0;
    
    // read one byte at a time
    while((bytes_read = read(sock, msg_buf_write_ptr, 1)) > 0) {
        bytes_read_total += bytes_read;
        
        // check if we have \n
        if(strcmp(msg_buf_write_ptr, "\n") == 0) {
//            printf("Received \\n.\n");
            break;
        }
        
        msg_buf_write_ptr += bytes_read;
        
        // have we got MAX_CLIENT_MSG_SIZE bytes and no \n?
        if(bytes_read_total == MAX_CLIENT_MSG_SIZE) {
            write(sock, "err", 3);
            printf(ANSI_COLOR_RED
                   "Client tried to write over 4K request data - potential"
                   "buffer overflow exploit attempt!" ANSI_RESET);
            
            write(sock, "overflow", 8);
            
            // Close socket
            free(msg_buf);
            close(sock);
            
            pthread_exit(NULL);
        }
    }
    
//    printf("Read %li bytes from client.\n%s\n", bytes_read_total, msg_buf);
    
    // try and parse JSON
    json_error_t json_error;
    json_t *parsed = json_loads(msg_buf, 0, &json_error);
    free(msg_buf);
    
    if(!parsed) {
        write(sock, "err", 3);
        printf("Error parsing JSON at line %i: %s\n", json_error.line, json_error.text);
        
        // free memory, close socket
        free(msg_buf);
        close(sock);
        
        pthread_exit(NULL);
    } else {
        if(!json_is_object(parsed)) {
            write(sock, "err_type", 8);
            printf("Expected JSON object, got %i", parsed->type);
            
            // free memory, close socket
            free(parsed);
            close(sock);
            
            pthread_exit(NULL);
        }
    }
    
    // allocate memory for the message
    push_msg *message = malloc(sizeof(push_msg));
    memset(message, 0x00, sizeof(push_msg));

    if(message == NULL) {
        write(sock, "nomem", 5);
        
        printf("Could not allocate push_msg object!\n");
        
        // free memory, close socket
        free(parsed);
        close(sock);
        
        pthread_exit(NULL);        
    }
    
    message->text = copy_json_info(json_object_get(parsed, "text"));
    message->sound = copy_json_info(json_object_get(parsed, "sound"));
    message->badgeNumber = (int) json_integer_value(json_object_get(parsed, "badge"));
    message->custPayload = copy_json_info(json_object_get(parsed, "custom"));
    message->deviceID = copy_json_info(json_object_get(parsed, "key"));
    message->buttonTitle = copy_json_info(json_object_get(parsed, "btnTitle"));
    message->localized_template = copy_json_info(json_object_get(parsed, "localized_text"));
    message->localized_arguments.argument0 = copy_json_info(json_object_get(parsed, "localized_arg0"));
    message->localized_arguments.argument1 = copy_json_info(json_object_get(parsed, "localized_arg1"));
    message->localized_arguments.argument2 = copy_json_info(json_object_get(parsed, "localized_arg2"));
    message->localized_arguments.argument3 = copy_json_info(json_object_get(parsed, "localized_arg3"));
    
    write(sock, "ok", 2);
    
    // Free memory, close socket
    free(parsed);
    close(sock);
    
//    printf("Text: %s\nDevice: %s\n", message->text, message->deviceID);
    
    int error = msg_queue_insert(message); // shove message into queue
    if(error) {
        printf("Error adding to queue: %i\n\n", error);
    }
    
    fflush(stdout);
    
    return NULL;
}

/*
 * This inline copies a string from the json_value struct to the destination
 * string buffer, ensuring that memory is allocated.
 */
inline char* copy_json_info(json_t *value) {
    if(!json_is_string(value)) {
        return NULL;
    }
    
    // convert string and get length
    const char *cString = json_string_value(value);
    size_t stringLength = strlen(cString);
    
    // alloc mem and copy
    char *destBuff = malloc(stringLength + 2); // alloc mem + 2
    memset(destBuff, 0x00, (sizeof(char) * stringLength + 2));
    strncpy(destBuff, cString, stringLength); // copy
    
    return destBuff; // return
}
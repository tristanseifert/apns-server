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

#include "json.h"

#include "apns_config.h"

static int client_socket = 0;
static int client_should_run = 0;
static pthread_t client_service_thread = NULL;

void *client_interface_listening_thread(void *param);
void *client_interface_connection_handler(void *connection);

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
        return -1;
    }
    
    sa.sin_family = hp->h_addrtype;
    sa.sin_port = htons(CLIENT_LISTEN_PORT);
    
    // create socket
    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return 255;
    }
    
    // bind socket
    if(bind(client_socket, (const struct sockaddr *) &sa, sizeof(struct sockaddr_in)) < 0) {
        close(client_socket);
        return 512;
    }
    
    listen(client_socket, MAX_NUM_QUEUED_CLIENTS);
    
    // Set up the listening thread
    client_should_run = 1;
    
    int error = pthread_create(&client_service_thread, NULL, client_interface_listening_thread, NULL);
    
    if(error) {
        client_should_run = 0;
        close(client_socket);
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
    volatile int t; // holds the connection we establish with clients
    
    while(client_should_run) {
        if((t = client_interface_get_connection()) < 0) {
            if(errno == EINTR) { // The socket might sometimes get EINTR
                continue;
            }
            
            perror("Accept client connection"); // die, we got an unknown error
        }
        
        // char sock[4] = &t;
        pthread_create(NULL, NULL, client_interface_connection_handler, (void *) &t);
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
    
    char *msg_buf = calloc(MAX_CLIENT_MSG_SIZE + 1, sizeof(char));
    char *msg_buf_write_ptr = msg_buf;
    
    long bytes_read_total = 0;
    long bytes_read = 0;
    
    // Read while we've got bytes
    while((bytes_read = read(sock, msg_buf_write_ptr, MAX_CLIENT_MSG_SIZE - bytes_read_total)) > 0) {
        bytes_read_total += bytes_read;
        msg_buf_write_ptr += bytes_read;
    }
    
    // try and parse JSON
    char jsonErr;
    json_settings settings = { 0 };
    json_value *parsed = json_parse_ex(&settings, msg_buf, MAX_CLIENT_MSG_SIZE, &jsonErr);
    
    return NULL;
}
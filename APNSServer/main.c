//
//  main.c
//  APNSServer
//
//  Created by Tristan Seifert on 25.05.13.
//  Copyright (c) 2013 Squee! Apps. All rights reserved.
//

#include "main.h"
#include "apns_config.h"
#include "ansi_terminal_defs.h"

#include "msg_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

void intHandler(int sig);

static SSLConn *shared_SSL_connection;

int main(int argc, const char * argv[]) {
    printf(ANSI_BOLD "Establishing SSL connection to Apple servers...\n" ANSI_RESET);
    
    shared_SSL_connection = SSL_Connect(APNS_HOST, APNS_PORT, RSA_CLIENT_CERT, RSA_CLIENT_KEY, CA_CERT_PATH);
    
    printf(ANSI_BOLD ANSI_COLOR_GREEN "\nSocket connected!\n\n" ANSI_RESET);
    
    // Trap Ctrl+C so we can clean up the connection (and flush queue)
    signal(SIGINT, intHandler);
    
    // Start threads
    msg_handler_begin();
    
    // Go into an infinite loop here
    while(1) {
        
    }
}

void intHandler(int sig) {
    // Ignore the signal so it can't happen twice
    signal(sig, SIG_IGN);
    
    printf(ANSI_BOLD ANSI_COLOR_RED "\nReceived Ctrl^C!\nFlushing buffers and disconnecting socket.\n" ANSI_RESET);
    
    printf(ANSI_BOLD "Stopping message handler thread...\n" ANSI_RESET);
    msg_handler_end(1);
    
    printf(ANSI_BOLD "Disconnecting socket and destroying SSL context...\n" ANSI_RESET);
    // Disconnect SSL socket
    SSL_Disconnect(shared_SSL_connection);
    exit(0);
}



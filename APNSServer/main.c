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

#include "msg_queue.h"
#include "msg_handler.h"
#include "client_interface.h"
#include "config_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

void intHandler(int sig);

int main(int argc, const char * argv[]) {
    printf(ANSI_RESET);
    
    // Try to parse config file
    config_parse(CONFIG_FILE_PATH);

    printf(ANSI_BOLD "Establishing SSL connection to Apple servers...\n" ANSI_RESET);
    
    SSL_Connect(config_get_value("APNS_Host"), (int) config_get_number("APNS_Port"),
                config_get_value("RSA_Client_Cert"), config_get_value("RSA_Client_Key"),
                config_get_value("RSA_CA_Cert_Path"));
    
    
    printf(ANSI_BOLD ANSI_COLOR_GREEN "\nSocket connected!\n\n" ANSI_RESET);
    
    // Trap Ctrl+C so we can clean up the connection (and flush queue)
    signal(SIGINT, intHandler);
    
    msg_queue_begin();
    
    // Start threads
    int error = msg_handler_begin();
    if(error) {
        printf("Error starting message handler: %i", error);
        return error;
    }
    
    // Set up listening socket
    error = client_interface_set_up();
    if(error) {
        printf("Error starting message handler: %i", error);
        return error;
    }
    
    printf("\nServer is ready. Waiting for connections.\n");
    
    // Go into an infinite loop here
    while(1) {
        sleep(5);
    }
}

void intHandler(int sig) {
    // Ignore the signal so it can't happen twice
    signal(sig, SIG_IGN);
    printf(ANSI_BOLD ANSI_COLOR_RED "\nReceived Ctrl^C!\n" ANSI_RESET);
    
    printf(ANSI_BOLD "Stopping client handler...\n" ANSI_RESET);
    client_interface_stop(1);    
    
    printf(ANSI_BOLD "Stopping message handler thread...\n" ANSI_RESET);
    msg_handler_end(1);
    
    printf(ANSI_BOLD "Disconnecting socket and destroying SSL context...\n" ANSI_RESET);
    // Disconnect SSL socket
    SSL_Disconnect((SSLConn *) SSL_get_shared_context());
    exit(0);
}



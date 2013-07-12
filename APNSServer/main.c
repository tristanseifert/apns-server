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

#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <openssl/ssl.h>

static SSLConn *shared_SSL_connection;

int main(int argc, const char * argv[]) {
    printf(ANSI_BOLD "Establishing SSL connection to Apple servers...\n" ANSI_RESET);
    
    shared_SSL_connection = SSL_Connect(APNS_HOST, APNS_PORT, RSA_CLIENT_CERT, RSA_CLIENT_KEY, CA_CERT_PATH);
    
    printf(ANSI_BOLD ANSI_COLOR_GREEN "\nSocket connected!\n\n" ANSI_RESET);
 to donigans ver
    // If we get here without a problem, we have a functional socket.
}



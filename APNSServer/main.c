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

int main(int argc, const char * argv[]) {
    /* printf("Initialising OpenSSL... ");
    initSSLLib();
    printf("Done.\n\n"); */
    
    // Try and resolve the APNS server    
    struct addrinfo *addrInfo;
    
    printf(ANSI_BOLD "Resolving APNS server address (%s)...\n" ANSI_RESET, APNS_HOST);

    // Try and resolve the APNS hostname
    if(getaddrinfo(APNS_HOST, NULL, NULL, &addrInfo) != 0) {
        perror("Can't resolve APNS host");
        return -1;
    }
    
    // Copy IP from returned server address
    struct sockaddr_in socketAddress;
    socketAddress.sin_addr = ((struct sockaddr_in *) addrInfo->ai_addr)->sin_addr;
    
    // Further set up sockaddr_in struct
    socketAddress.sin_port = htons(APNS_PORT); // Use specified port
    socketAddress.sin_family = AF_INET; // Use IPv4
    
    printf(ANSI_COLOR_GREEN "Resolved APNS host to %s" ANSI_RESET, inet_ntoa(socketAddress.sin_addr));
    
    
    // Set up the socket
    printf(ANSI_BOLD "\n\nCreating socket...\n" ANSI_RESET);
    int APNSSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if(APNSSocket == -1) {
        perror("Can't create socket");
        return -1;
    }
    
    printf(ANSI_BOLD ANSI_COLOR_GREEN "Socket created.\n" ANSI_RESET);
    
    printf(ANSI_BOLD "Connecting socket to server...\n" ANSI_RESET);
    
    // Try and connect to the server
    int connectErr;
    if ((connectErr = connect(APNSSocket, (const struct sockaddr*) &socketAddress, sizeof(socketAddress)))) {
        printf("connect() error %i\n", connectErr);
        perror("Can't connect socket");
        return -1;
    }
    
    printf(ANSI_BOLD ANSI_COLOR_GREEN "\nSocket connected!\n\n" ANSI_RESET);
    
    // If we get here without a problem, we (should) have a functional socket.
}

void initSSLLib() {
    SSL_load_error_strings();                /* readable error messages */
    SSL_library_init();                      /* initialize library */
}


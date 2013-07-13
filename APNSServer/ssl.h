//
//  ssl.h
//  APNSServer
//
//  Created by Tristan Seifert on 12/07/2013.
//  Copyright (c) 2013 Squee! Apps. All rights reserved.
//

#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <openssl/ssl.h>

#ifndef APNSServer_ssl_h
#define APNSServer_ssl_h

typedef struct {
    /* SSL Vars */
    SSL_CTX *ctx;
    SSL *ssl;
    SSL_METHOD *meth;
    X509 *server_cert;
    EVP_PKEY *pkey;

    /* Socket Communications */
    struct sockaddr_in server_addr;
    struct hostent *host_info;
    int sock;
} SSLConn;

volatile static SSLConn *shared_SSL_connection = NULL;

SSLConn *SSL_Connect(const char *host, int port, const char *certfile, const char *keyfile, const char* capath);
void SSL_Disconnect(SSLConn *sslcon);
extern inline int ssl_write_to_sock(SSLConn *conn, void *buf, int numBytes);

#endif

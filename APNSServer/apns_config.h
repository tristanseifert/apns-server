//
//  apns_config.h
//  APNSServer
//
//  Created by Tristan Seifert on 25.05.13.
//  Copyright (c) 2013 Squee! Apps. All rights reserved.
//

// Emit warning to change the config
#ifndef APNSServer_Config_Warning
#define APNSServer_Config_Warning
#warning Adjust settings in apns_config.h!
#endif

#ifndef APNSServer_apns_config_h
#define APNSServer_apns_config_h

// Set to one to enable all debug logging
#define APNS_DEBUG 1

// Set the host and port to connect to
#define APNS_HOST "gateway.sandbox.push.apple.com"
#define APNS_PORT 2195

// Location of APNS certs (absolute)
#define RSA_CLIENT_CERT "/Users/tristanseifert/FaceReel/APNSServer/certs/apns-dev-cert.pem"
#define RSA_CLIENT_KEY "/Users/tristanseifert/FaceReel/APNSServer/certs/apns-dev-key.pem"

// Location of the CA certificate (absolute)
#define CA_CERT_PATH "/Users/tristanseifert/FaceReel/APNSServer/certs/CACerts/"

// Number of bytes to allocate for hostname buffers
#define HOSTNAME_BUF_SIZE 1024

// Port that APNS server listens to for clients
#define CLIENT_LISTEN_PORT 55427

// Maximum number of clients to queue before refusing connections
#define MAX_NUM_QUEUED_CLIENTS 16

// Maximum size of messages received by clients
#define MAX_CLIENT_MSG_SIZE 1024*4

// Various debugging defines (should be commented out when compiled for prod)
#if APNS_DEBUG == 1
#define LINKED_LIST_DEBUG 1
#define MSG_PROCESSING_DEBUG 1
#define SSL_DEBUG 1
#endif

#endif
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
#define APNS_DEBUG 0

// Path to the config file
#define CONFIG_FILE_PATH "/usr/local/etc/apnsserver/apns.conf"


// Number of bytes to allocate for hostname buffers
#define HOSTNAME_BUF_SIZE 1024


// Various debugging defines (should be commented out when compiled for prod)
#if APNS_DEBUG == 1
#define LINKED_LIST_DEBUG 1
#define MSG_PROCESSING_DEBUG 1
#define SSL_DEBUG 1
#define CONFIG_DEBUG 1
#endif

#endif

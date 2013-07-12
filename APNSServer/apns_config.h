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

// Set the host and port to connect to
#define APNS_HOST "gateway.sandbox.push.apple.com"
#define APNS_PORT 2195

// Location of APNS certs (absolute)
#define RSA_CLIENT_CERT "/Users/tristanseifert/FaceReel/APNSServer/certs/apns-dev-cert.pem"
#define RSA_CLIENT_KEY "/Users/tristanseifert/FaceReel/APNSServer/certs/apns-dev-key.pem"

// Location of the CA certificate (absolute)
#define CA_CERT_PATH "/Users/tristanseifert/FaceReel/APNSServer/certs/CACerts/"

#endif
APNSServer
===========
The goal of this project is to produce an Apple push notifications server written in pure C that is efficient, has a small footprint and small demands on the system running it, and is easy to use.

Features of this server will include:
* Queue to hold messages to be sent to the APNS server
* Message templates
* Single persistent connection to Apple's servers

### Support
The code should run without much issues on any system that C can be compiled for, and has support for sockets. OpenSSL is required to be available (both headers to build and library to link against) for the SSL communications with Apple's servers. The code has been developed on Mac OS X 10.8, but should also be able to be compiled on most other *nix-like systems.


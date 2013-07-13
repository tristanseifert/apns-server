APNSServer
===========
The goal of this project is to produce an Apple push notifications server written in pure C that is efficient, has a small footprint and small demands on the system running it, and is easy to use.

Features of this server will include:
* Queue to hold messages to be sent to the APNS server
* Message templates
* Single persistent connection to Apple's servers

### Compatibility
The code should run without much issues on any system that C can be compiled for, and has support for sockets. OpenSSL is required to be available (both headers to build and library to link against) for the SSL communications with Apple's servers. The code has been developed on Mac OS X 10.8, but should also be able to be compiled on most other *nix-like systems.

It helps to compile this code with the `-O3` compiler setting if binary size is not an issue.

### Source Code License ###
The source code is released under the [Simplified BSD License](http://opensource.org/licenses/bsd-license.php):

Copyright (c) 2013, Squee! Apps
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

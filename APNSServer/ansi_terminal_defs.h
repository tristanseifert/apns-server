//
//  ansi_terminal_defs.h
//  APNSServer
//
//  Created by Tristan Seifert on 25.05.13.
//  Copyright (c) 2013 Squee! Apps. All rights reserved.
//

#ifndef APNSServer_ansi_terminal_defs_h
#define APNSServer_ansi_terminal_defs_h

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ANSI_BGCOLOR_WHITE   "\x1b[47m"
#define ANSI_BGCOLOR_RED     "\x1b[41m"
#define ANSI_BGCOLOR_GREEN   "\x1b[42m"
#define ANSI_BGCOLOR_YELLOW  "\x1b[43m"
#define ANSI_BGCOLOR_BLUE    "\x1b[44m"
#define ANSI_BGCOLOR_MAGENTA "\x1b[45m"
#define ANSI_BGCOLOR_CYAN    "\x1b[46m"
#define ANSI_BGCOLOR_RESET   "\x1b[49m"

#define ANSI_BOLD			 "\x1b[1m"
#define ANSI_ITALIC		     "\x1b[3m"
#define ANSI_UNDERLINE	   	 "\x1b[4m"
#define ANSI_STRIKETHRU		 "\x1b[9m"
#define ANSI_BOLDOFF    	 "\x1b[22m"
#define ANSI_ITALICOFF		 "\x1b[23m"
#define ANSI_UNDERLINEOFF    "\x1b[24m"
#define ANSI_STRIKEOFF    	 "\x1b[29m"
#define ANSI_RESET   		 "\x1b[0m"

#endif

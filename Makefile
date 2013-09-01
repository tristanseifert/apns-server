CC=gcc

# Silence "unused variable" warnings, enable C99
CFLAGS=-c -Wall -Wno-unused-variable --std=gnu99

all: jansson apnsserver

apnsserver: apnsserver.o 
	$(CC) -L/usr/lib -lssl -pthread *.o jansson/*.o -o apnsserver

apnsserver.o:
	$(CC) $(CFLAGS) -Ijansson/ -IAPNSServer/ APNSServer/*.c

jansson: jansson/dump.c jansson/error.c jansson/hashtable.c jansson/load.c jansson/memory.c jansson/pack_unpack.c jansson/strbuffer.c jansson/strconv.c jansson/utf.c jansson/value.c 
	cd jansson
	$(CC) $(CFLAGS) *.c
	cd ..
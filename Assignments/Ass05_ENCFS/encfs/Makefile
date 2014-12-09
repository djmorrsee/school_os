# File: Makefile
# By: Andy Sayler <www.andysayler.com>
# Adopted from work by: Chris Wailes <chris.wailes@gmail.com>
# Project: CSCI 3753 Programming Assignment 5
# Creation Date: 2010/04/06
# Modififed Date: 2012/04/12
# Description:
#	This is the Makefile for PA5.


CC           = gcc

CFLAGSFUSE   = -D_FILE_OFFSET_BITS=64 `pkg-config fuse --cflags`
LLIBSFUSE    = `pkg-config fuse --libs`
LLIBSOPENSSL = -lcrypto

CFLAGS = -c -g -Wall -Wextra
LFLAGS = -g -Wall -Wextra

.PHONY: all clean

all: encfs

encfs: encfs.o djxattr.o dj-crypt.o aes-crypt.o
	$(CC) $(LFLAGS) $^ -o $@ $(LLIBSFUSE) $(LLIBSOPENSSL)
	
encfs.o: encfs.c 
	$(CC) $(CFLAGS) $(CFLAGSFUSE) $^

djxattr.o: djxattr.c 
	$(CC) $(CFLAGS) $^
	
dj-crypt.o: dj-crypt.c 
	$(CC) $(CFLAGS) $^

aes-crypt.o: aes-crypt.c aes-crypt.h
	$(CC) $(CFLAGS) $^

clean:
	rm -f encfs
	rm -f *.o
	rm -f *~
	rm -f handout/*~
	rm -f handout/*.log
	rm -f handout/*.aux
	rm -f handout/*.out




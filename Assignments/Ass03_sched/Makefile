CC = gcc
CFLAGS = -c -g -Wall -Wextra
LFLAGS = -g -Wall -Wextra

.PHONY: all clean

all: mix cpu io

clean:
	rm -f mix 
	rm -f cpu
	rm -f io
	rm -f *.o
	rm -f *.txt
	rm -f test/*

mix:mix.c
	$(CC) $(LFLAGS) $^ -o $@ -lm

cpu:cpu_bound.c
	$(CC) $(LFLAGS) $^ -o $@ -lm

io:io_bound.c
	$(CC) $(LFLAGS) $^ -o $@ -lm

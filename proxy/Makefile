#Sample Makefile. You can make changes to this file according to your need
# The executable must be named proxy

CC = gcc
CFLAGS = -Wall -g 
LDFLAGS = -lpthread

TOBJS = multithread-proxy.o csapp.o
POBJS = multiprocess-proxy.o csapp.o

all: multithread-proxy multiprocess-proxy

multithread-proxy: $(TOBJS)

multiprocess-proxy: $(POBJS)

csapp.o: csapp.c
	$(CC) $(CFLAGS) -c csapp.c

multithread-proxy.o: multithread-proxy.c
	$(CC) $(CFLAGS) -c multithread-proxy.c

multiprocess-proxy.o: multiprocess-proxy.c
	$(CC) $(CFLAGS) -c multiprocess-proxy.c
clean:
	rm -f *~ *.o multiprocess-proxy multithread-proxy 

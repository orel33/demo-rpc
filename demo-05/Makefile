# Makefile

CC=gcc
CFLAGS=-std=c99 -Wall -g
CPPFLAGS=-I /usr/include/tirpc/
LDFLAGS=
LDLIBS= -ltirpc

ALL: server client

server.o: server.c
client.o: client.c

server: server.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

client: client.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f *.o server client

# EOF
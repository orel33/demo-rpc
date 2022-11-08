# Makefile

CC=gcc
CFLAGS=-std=c99 -Wall
CPPFLAGS=-I /usr/include/tirpc/
LDFLAGS=
LDLIBS= -ltirpc

ALL: server client lightclient

server.o: server.c
client.o: client.c
lightclient.o: lightclient.c

server: server.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

client: client.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

lightclient: lightclient.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f *.o server client lightclient

# EOF
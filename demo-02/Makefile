# Makefile

CC=gcc
CFLAGS=-std=c99
CPPFLAGS=-I /usr/include/tirpc/
LDFLAGS=
LDLIBS= -ltirpc

ALL: server client

add.h: add.x
	rpcgen -h -N -o add.h add.x

server.o: server.c add.h
client.o: client.c add.h

server: server.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

client: client.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f *.o server client add.h

# EOF

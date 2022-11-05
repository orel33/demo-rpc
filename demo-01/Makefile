# Makefile

CC=gcc
CFLAGS=-std=c99
CPPFLAGS=-I /usr/include/tirpc/
LDFLAGS=
LDLIBS= -ltirpc

ALL: server client

add_svc.c add.h: add.x
	rpcgen -N -C add.x

add_xdr.o: add_xdr.c add.h
add_svc.o: add_svc.c add.h
add_clnt.o: add_clnt.c add.h
server.o: server.c add.h
client.o: client.c add.h

server: server.o add_svc.o add_xdr.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

client: client.o add_clnt.o add_xdr.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f *.o add.h add_clnt.c add.h add_svc.c add_xdr.c server client

# EOF
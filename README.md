# Demo RPC

## Installation

```bash

# install rpcbing & tirpc (transport-independant RPC library)
$ sudo apt install rpcbind rpcsvc-proto libtirpc-dev libtirpc3

$ rpcinfo -p
   program vers proto   port  service
    100000    4   tcp    111  portmapper
    100000    3   tcp    111  portmapper
    100000    2   tcp    111  portmapper
    100000    4   udp    111  portmapper
    100000    3   udp    111  portmapper
    100000    2   udp    111  portmapper
```

## Demo 00

Given the interface [add.x](demo-00/add.x), we can generate all the files including
sample code for client and server side by using `rpcgen`.

```bash
$ rpcgen -a -N add.x
```

According to manual, the option `-N` is for the newstyle of rpcgen. This allows
procedures to have  multiple arguments. It also uses the style of parameter
passing that closely resembles C. So, when passing an argument to a remote
procedure you do not have to pass a pointer to the argument but  the  argument
it‐self.  This behaviour is different from the oldstyle of rpcgen generated
code. The newstyle is not the default case because of backward compatibility.

In this demo, we provide no implementation for the `add` program, but just a
`Makefile` to compile the sample code with library `tirpc`.

## Demo 01

This demo shows the implementation of the `add` program, by using client &
server stubs that are generated by `rpcgen`.

## Demo 02

In this demo, we do some refactoring compared to the previous demo, and we only
use `rpcgen` to generate the header `add.h`.

## Demo 03

This demo illustrates how to implement client and server directly without using
client & server stubs that are generated by `rpcgen`. This code is more simple
to understand than the previous one. It illustrates how to use directly RPC API.

**Nota Bene** : The `add` program number defined in `add.x` is
`0x20000199=536871321`.

Let's try to launch this demo...

```bash
# compilation
$ cd demo-03
$ make
  gcc -std=c99 -I /usr/include/tirpc/  -c -o server.o server.c
  gcc  server.o -o server -ltirpc
  gcc -std=c99 -I /usr/include/tirpc/  -c -o client.o client.c
  gcc  client.o -o client -ltirpc
# launch server for add program
$ ./server
# list registered services (in another terminal)
$ rpcinfo -p
   program vers proto   port  service
    100000    4   tcp    111  portmapper
    100000    3   tcp    111  portmapper
    100000    2   tcp    111  portmapper
    100000    4   udp    111  portmapper
    100000    3   udp    111  portmapper
    100000    2   udp    111  portmapper
 536871321    1   tcp  52466               # <-- our service for tcp
 536871321    1   udp  57331               # <-- our service for udp
# launch client (in another terminal)
$ ./client localhost
  result: 10 + 20 = 30
```

## Demo 04

As the previous one, this demo illustrates how to implement client and server
directly with RPC API. It provides a bit more complex program
[test.x](demo-04/test.x) with more procedures. Besides, the server can manage
its own socket and can optionally bind it to a given port, instead of choosing
one randomly.

**Nota Bene** : The `test` program number defined in `test.x` is `0x20000001=536870913`.

Let's try to launch this demo... Here, the client uses the UDP transport layer
to call both `add` and `echo` procedures.

```bash
# compilation
$ cd demo-04
$ make
gcc -std=c99 -Wall -g -I /usr/include/tirpc/  -c -o server.o server.c
gcc  server.o -o server -ltirpc
gcc -std=c99 -Wall -g -I /usr/include/tirpc/  -c -o client.o client.c
gcc  client.o -o client -ltirpc
# launch server for test program (listening on port 7777)
$ ./server 7777
# list registered services (in another terminal)
$ rpcinfo -p
   program vers proto   port  service
    100000    4   tcp    111  portmapper
    100000    3   tcp    111  portmapper
    100000    2   tcp    111  portmapper
    100000    4   udp    111  portmapper
    100000    3   udp    111  portmapper
    100000    2   udp    111  portmapper
 536870913    1   tcp   7777               # <-- our service for tcp
 536870913    1   udp   7777               # <-- our service for udp
# launch client (in another terminal)
$ ./client localhost
  add result: 10 + 20 = 30
  echo result: hello
```

## Demo 05

Here, we have the same demo as the previous one, but we remove the use of
`rpcbind` for both client and server. So that, the client is directly connected
to the RPC server. This code is only available for UDP transport layer.

```bash
# compilation
$ cd demo-05
$ make
# launch server for test program (listening on port 7777)
$ ./server 7777
# launch client (in another terminal)
$ ./client localhost 7777
  add result: 10 + 20 = 30
  echo result: hello
```

## About Wireshark

If wireshark doesn't know the program number, it doesn't do the RPC decode,
*unless* "Dissect unknown RPC program numbers" is checked in the preferences.
After this, you can see both the request and replies.

Here are the capture of the RPC demo with both UDP & TCP transport layers :

* [rpc-demo-udp.pcap](rpc-demo-udp.pcap)
* [rpc-demo-tcp.pcap](rpc-demo-tcp.pcap)

## Biblio

* ONC/SUN RPC: <http://en.wikipedia.org/wiki/ONC_RPC>
* RFC 1831 RPC: [Remote Procedure Call Protocol Version 2](https://www.ietf.org/rfc/rfc1831.txt)
* RFC 1832 XDR: [External Data Representation Standard](https://www.ietf.org/rfc/rfc1832.txt)
* RFC 1833 Port Mapper: [Binding Protocols for ONC RPC Version 2](https://www.ietf.org/rfc/rfc1833.txt)

---
<aurelien.esnard@u-bordeaux.fr>

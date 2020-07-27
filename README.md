## This is a simple multi-threaded chat server implemented on Berkeley sockets (UNIX) server with can handle multiple client connections.

### Requirements:
 - gcc version 8.0 > or higher
 - make

### build&run
To build and run server run the folowing commands:
```
cd <Project Directory>/server
make
./server 127.0.0.1 8080
```

To build and run client run the folowing commands:
```
cd <Project Directory>/client
make
./client 127.0.0.1 8080
```

### Note
There is bug with long messages (messages that are higher than 1024 symbols will be cut)

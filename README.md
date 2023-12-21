# client/server apps

This repository provides source code for the client and server apps. All you need is a Linux machine. 

## Build

1. Navigate to the `client-server-CPP-implementation` directory.
2. Run `make` to build client and server.

## Run Client and Server

### Server

- Type `./server <port>`. If you don't specify a port, the default port 8080 will be used.

### Client

- Type `./client`.

## Usage

The client supports the following commands:

- `CONNECT <port> <client name>`: Connects the client to the server on the specified port with a given name.
- `DISCONNECT`: Disconnects the client from the server.
- `PUBLISH <topic name> <data>`: Publishes data to a specific topic.
- `SUBSCRIBE <topic name>`: Subscribes to a specific topic.
- `UNSUBSCRIBE <topic name>`: Unsubscribes from a specific topic.

Example of running a client:
- `./client`
- `connect 8080 client_name`
- `subscribe /topic`
- `publish /topic some_data`
- `unsubscribe /topic`
- `disconnect`


## Description

Both client and server apps print various messages of what is going on at the time.

### Client

The client uses a command observer pattern to handle all the commands (Commands.h and Commands.cpp).
It uses `poll` to check if there is any input and then deals with the input appropriately. If any message is received, it is printed out. 
When DISCONNECT command is entered, the client disconnects and the app closes.

**Important:** If clients are not on the same network as the server, change IP in Commands.h to servers ip. 

### Server

The server initializes by creating a socket, binding it, and starting to listen. It then enters a loop to handle client interactions.
In each iteration, it checks for new client connections and reads data from existing clients.
It processes client commands such as publish, subscribe, and unsubscribe.
When a client disconnects, it is removed from the list of clients and unsubscribed from topics.
The server continues running until there are no more clients connected.

Here we have an observer design pattern in use. Class Topic (Topic.h, Topic.cpp) manages the observers (ClientObserver.h, ClientObserver.cpp) subscribed to a specific topic.


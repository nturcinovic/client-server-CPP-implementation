# client/server apps

This repository provides source code for the client and server apps.

## Build

1. Navigate to the `client-server-CPP-implementation` directory.
2. Run `make` to build client and server.

## Run Client and Server

### Server

- Type `./server <port>`. If you don't specify a port, the default port 8080 will be used.

### Client

- Type `./client`.

## Usage

The system supports the following commands:

- `CONNECT <port> <client name>`: Connects the client to the server on the specified port with a given name.
- `DISCONNECT`: Disconnects the client from the server.
- `PUBLISH <topic name> <data>`: Publishes data to a specific topic.
- `SUBSCRIBE <topic name>`: Subscribes to a specific topic.
- `UNSUBSCRIBE <topic name>`: Unsubscribes from a specific topic.

## Description

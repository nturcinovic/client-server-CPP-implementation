#pragma once

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "ClientObserver.h"
#include "Topic.h"

#define DEFAULT_PORT 8080

class Server {
private:
    int serverfd;
    int port;
    std::vector<ClientObserver> clients;
    std::vector<Topic> topics;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char buffer[BUFFER_SIZE];

    bool initializeServer();
    void init_client(ClientObserver *client, int clientSockFd, std::string buffer);
    void close_client(ClientObserver &client);
    bool pollClients();
    bool topicNameMatches(const Topic& topic, const std::string& name) const;
public:
    Server(int serverPort);
    ~Server();
    void run();
};
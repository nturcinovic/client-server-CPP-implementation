#pragma once

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <mutex>
#include "ClientObserver.h"
#include "Topic.h"

#define DEFAULT_PORT 8080
#define MAX_CLIENTS  30
#define MAX_TOPICS   20

class Server {
private:
    int serverfd;
    int port;
    std::vector<ClientObserver> clients;
    std::vector<Topic> topics;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char buffer[BUFFER_SIZE];
    Message newMessage;

    bool initializeServer();
    void initClient(int clientSockFd, std::string name);
    void closeClient(ClientObserver &client);
    bool pollClients();
    void addTopic(Topic& topicTmp, ClientObserver& client);
    void removeTopic(const std::string& topicName, ClientObserver& client);
public:
    Server(int serverPort);
    ~Server();
    void run();
};
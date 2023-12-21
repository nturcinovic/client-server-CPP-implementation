
#include "server.h"
#include <cstring>
#include <algorithm>
#include <arpa/inet.h>
#include <sstream>

Server::Server(int serverPort) : serverfd(-1), port(serverPort) {
    memset(&addr, 0, sizeof(addr));
    addrlen = sizeof(addr);
    //newMessage("", "");
    clients.reserve(MAX_CLIENTS);
    topics.reserve(MAX_TOPICS);
}

Server::~Server() {
    close(serverfd);
}

void Server::run() {
    if (!initializeServer()) {
        std::cerr << "Failed to initialize the server." << std::endl;
        return;
    }
    std::cout << "Server initialized" << std::endl;

    while(1) {
        if(!pollClients()) {
            break;
        }
    }

    std::cout << "Server closed." << std::endl;
}

bool Server::initializeServer() {
    int opt = 1;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd < 0) 
    {
        perror("socket");
        return false;
    }

    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
        perror("setsockopt");
        return false;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) 
    {
        perror("bind");
        return false;
    }

    if (listen(serverfd, 5) < 0) 
    {
        perror("listen");
        return false;
    }

    return true;
}

void Server::initClient(int clientSockFd, std::string buffer) {
    ClientObserver tmpClient; 
    tmpClient.sock = clientSockFd;
    tmpClient.port = ntohs(addr.sin_port);
    tmpClient.ip = inet_ntoa(addr.sin_addr);
    memset(tmpClient.buffer, 0, sizeof(tmpClient.buffer)); 
    tmpClient.name = buffer;
    clients.push_back(tmpClient);
}

void Server::closeClient(ClientObserver &client) {
    auto it = std::find_if(clients.begin(), clients.end(), [&client](const ClientObserver& c) {
        return &c == &client;
    });

    if (it != clients.end()) {
        // Unsubscribe the client from all topics
        for (auto& topic : topics) {
            client.unsubscribe(topic.getName());
            topic.removeObserver(&client);
        }

        // Remove the client from the list of clients
        clients.erase(it);
    }
}

bool Server::pollClients() {
    fd_set readfds;
    int activity, valread, sd, max_sd;
    int IsThereClients = 0;
    std::string topicName, topicData;

    while (1) 
    {
        FD_ZERO(&readfds);

        FD_SET(serverfd, &readfds);
        max_sd = serverfd;

        for (auto &client : clients) {
            sd = client.getSock();
            // If valid socket descriptor, then add to read list
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }

            // Highest file descriptor number, needed for the select function
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // Wait for an activity on one of the sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) 
        {
            perror("select");
            return false;
        }

        // Client is connecting
        if (FD_ISSET(serverfd, &readfds)) 
        {
            int clientSockFd = accept(serverfd, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
            if (clientSockFd < 0) 
            {
                perror("accept");
                return false;
            }

            IsThereClients++;

            memset(buffer, 0, sizeof(buffer));
            valread = read(clientSockFd, buffer, sizeof(buffer));
            initClient(clientSockFd, buffer);

            std::cout << "New connection, socket fd is [" << clientSockFd <<"], ip is: ["<< inet_ntoa(addr.sin_addr)
                    << "], port: [" << ntohs(addr.sin_port) << "], name: [" << buffer << "]" << std::endl;
        }

        // Check other sockets
        for (auto &client : clients) {
            sd = client.getSock();

            if (FD_ISSET(sd, &readfds)) 
            {
                memset(buffer, 0, sizeof(buffer));
                valread = read(sd, buffer, sizeof(buffer));

                if (valread == 0) 
                {
                    // Client disconnected
                    closeClient(client);
                    sd = 0;
                    IsThereClients--;

                    std::cout << "Client disconnected, ip [" << client.ip << "], port [" << client.port 
                            << "], name [" << client.name << "]" << std::endl;
                } 
                else 
                {
                    buffer[valread] = '\0';

                    std::istringstream iss(buffer);
                    std::string commandType;
                    iss >> commandType >> topicName >> topicData;
                
                    // Convert commandType to uppercase for case-insensitive comparison
                    std::transform(commandType.begin(), commandType.end(), commandType.begin(), ::toupper);

                    newMessage.topic = topicName;
                    newMessage.data = topicData;
                    Topic tempTopic(newMessage);

                    // Compare with the known command types
                    if (commandType == "PUBLISH") {
                        for (auto &topic : topics) {
                            if (topic.getName() == newMessage.topic) {
                                topic.publishMessage(newMessage);
                                std::cout << "Client [" << client.name << "] published data [" << newMessage.data  << "] to topic [" << newMessage.topic  <<"]" << std::endl; 
                            }
                        }

                    } else if (commandType == "SUBSCRIBE") {
                        addTopic(tempTopic, client);

                    } else if (commandType == "UNSUBSCRIBE") {
                        removeTopic(newMessage.topic, client);
                    }
                }
            }
        }

        if(IsThereClients == 0)
        {
            std::cout << "No more clients connected, shuting down the server!" << std::endl;
            topics.clear();
            break;
        }
    }

    return false;
}

void Server::addTopic(Topic& topicTmp, ClientObserver& client) {
    client.subscribe(topicTmp.getName());

    auto it = std::find_if(topics.begin(), topics.end(), [topicTmp](const Topic& topic) {
        return topic.getName() == topicTmp.getName();
    });

    if (it != topics.end()) {
        it->addObserver(&client);
    } else {
        topicTmp.addObserver(&client);
        topics.push_back(topicTmp);
    }
}

void Server::removeTopic(const std::string& topicName, ClientObserver& client) {
    client.unsubscribe(topicName);

    auto it = std::find_if(topics.begin(), topics.end(), [topicName](const Topic& topic) {
        return topic.getName() == topicName;
    });

    // If the topic is found, remove the client as an observer
    if (it != topics.end()) {
        it->removeObserver(&client);
    }
}

int main(int argc, char* argv[]) {
    int serverPort = (argc > 1) ? std::stoi(argv[1]) : DEFAULT_PORT;
    Server server(serverPort);
    server.run();

    return 0;
}
#include "Commands.h"
#include <arpa/inet.h>
#include <sstream>
#include <cstring>
#include <unistd.h>


ConnectCommand::ConnectCommand(int &clientfd, int &port, std::string &clientName, char *buffer)
    : clientfd(clientfd), port(port), clientName(clientName), buffer(buffer) {}

bool ConnectCommand::connectToServer() {
    if (port == 0) {
        std::istringstream iss(buffer);
        std::string tmp;
        iss >> tmp >> port >> clientName;

        if(clientName.empty())
        {
            std::cout << "Missing clients name" << std::endl;
            return false;
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        // If both server and clients are not on the same network, change IP to servers IP
        if (inet_pton(AF_INET, IP, &addr.sin_addr) <= 0) {
            std::cout << "Invalid address - Address not supported" << std::endl;
            return false;
        }

        if (connect(clientfd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0 &&
            errno != EINPROGRESS) {
            perror("connect error");
            return false;
        }
        strcpy(buffer, clientName.c_str());
        if (send(clientfd, buffer, strlen(buffer), 0) != static_cast<ssize_t>(strlen(buffer))) {
            perror("send");
            return false;
        }

        std::cout << "port number [" << port << "], client name [" << clientName << "]" << std::endl;
    } else {
        std::cout << "Client already connected!" << std::endl;
    }
    return true;
}

bool ConnectCommand::execute() { return connectToServer() ? true : false; }


DisconnectCommand::DisconnectCommand(int &clientfd, bool &running)
    : clientfd(clientfd), running(running){}

bool DisconnectCommand::disconnectFromServer() {
    std::cout << "disconnecting from server" << std::endl;
    if (clientfd != -1) {
        close(clientfd);
        clientfd = -1;
    }
    running = false;
    return true;
}

bool DisconnectCommand::execute() { return disconnectFromServer(); }


PublishCommand::PublishCommand(int &clientfd, char *buffer)
    : clientfd(clientfd), buffer(buffer) {}

bool PublishCommand::publishData() {
    if (clientfd != -1) {
        std::istringstream iss(buffer);
        std::string tmp, topicName, topicData;
        iss >> tmp >> topicName >> topicData;

        memset(buffer, 0, BUFFER_SIZE);
        strcpy(buffer, tmp.c_str());
        buffer[tmp.length()] = ' ';
        strcpy(&buffer[tmp.length() + 1], topicName.c_str());
        buffer[tmp.length() + 1 + topicName.length()] = ' ';
        strcpy(&buffer[tmp.length() + 1 + topicName.length() + 1], topicData.c_str());

        if (send(clientfd, buffer, strlen(buffer), 0) != static_cast<ssize_t>(strlen(buffer))) {
            perror("send");
            return false;
        }
        std::cout << "publishing data [" << topicData << "] to topic [" << topicName << "]" << std::endl;
    } else {
        std::cout << "Not connected to a server." << std::endl;
    }
    return true;
}

bool PublishCommand::execute() { return publishData() ? true : false; }


SubscribeCommand::SubscribeCommand(int &clientfd, char *buffer)
    : clientfd(clientfd), buffer(buffer) {}

bool SubscribeCommand::subscribeToTopic() {
    if (clientfd != -1) {
        std::istringstream iss(buffer);
        std::string tmp, topicName;
        iss >> tmp >> topicName;

        memset(buffer, 0, BUFFER_SIZE);
        strcpy(buffer, tmp.c_str());
        buffer[tmp.length()] = ' ';
        strcpy(&buffer[tmp.length() + 1], topicName.c_str());

        if (send(clientfd, buffer, strlen(buffer), 0) != static_cast<ssize_t>(strlen(buffer))) {
            perror("send");
            return false;
        }
        std::cout << "subscribing to topic [" << topicName << "]" << std::endl;
    } else {
        std::cout << "Not connected to a server." << std::endl;
    }
    return true;
}

bool SubscribeCommand::execute() { return subscribeToTopic() ? true : false; }


UnsubscribeCommand::UnsubscribeCommand(int &clientfd, char *buffer)
    : clientfd(clientfd), buffer(buffer) {}

bool UnsubscribeCommand::unsubscribeFromTopic() {
    if (clientfd != -1) {
        std::istringstream iss(buffer);
        std::string tmp, topicName;
        iss >> tmp >> topicName;

        memset(buffer, 0, BUFFER_SIZE);
        strcpy(buffer, tmp.c_str());
        buffer[tmp.length()] = ' ';
        strcpy(&buffer[tmp.length() + 1], topicName.c_str());

        if (send(clientfd, buffer, strlen(buffer), 0) != static_cast<ssize_t>(strlen(buffer))) {
            perror("send");
            return false;
        }
        std::cout << "unsubscribing from topic [" << topicName << "]" << std::endl;
    } else {
        std::cout << "Not connected to a server." << std::endl;
    }
    return true;
}

bool UnsubscribeCommand::execute() { return unsubscribeFromTopic() ? true : false; }
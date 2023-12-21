#include "ClientObserver.h"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <sys/socket.h>
#include "Topic.h"

ClientObserver::ClientObserver(int socket, int clientPort, const std::string& clientIp, const std::string& clientName)
    : sock(socket), port(clientPort), ip(clientIp), name(clientName) {
        memset(buffer, 0, sizeof(buffer));
}

ClientObserver::ClientObserver() : sock(-1), port(-1) {
    memset(buffer, 0, sizeof(buffer));
}

void ClientObserver::notify(const Message& message) {
    auto it = is_subscribed(message.topic);
    if(sock > 0 && it != topicsSubscribed.end()) {
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, message.topic.c_str());
        buffer[strlen(message.topic.c_str())] = DELIM_CHAR; 
        strcpy(&buffer[strlen(message.topic.c_str()) + 1], message.data.c_str());
        if(send(sock, buffer, strlen(buffer), 0) != static_cast<ssize_t>(strlen(buffer)))   
        {   
            perror("publish send");   
        }
        std::cout << "Notifying client [" << name << "] about message: " << message.data << " on topic: " << message.topic << std::endl; 
    }
}

int ClientObserver::getSock() const {
    return sock;
}

std::set<std::string>::iterator ClientObserver::is_subscribed(std::string topicName)
{
    return std::find(topicsSubscribed.begin(), topicsSubscribed.end(), topicName);
}

void ClientObserver::subscribe(std::string topicName) {
    auto it = is_subscribed(topicName);
    if(it != topicsSubscribed.end()) {
        std::cout << "client [" << name << "] already subscribed to topic [" << topicName << "]" << std::endl;
    } else {
        topicsSubscribed.insert(topicName);
        std::cout << "client [" << name <<"] subscribed to topic [" << topicName << "]" << std::endl;
    }
}

void ClientObserver::unsubscribe(std::string topicName) {
    auto it = is_subscribed(topicName);
    if (it != topicsSubscribed.end()) {
        topicsSubscribed.erase(it);
        std::cout << "Client [" << name << "] unsubscribed from topic [" << topicName << "]\n";
    } else {
        std::cout << "client [" << name << "] not subscribed to topic [" << topicName << "]" << std::endl;
    }
}

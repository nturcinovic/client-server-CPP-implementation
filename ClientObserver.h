#pragma once

#include <iostream>
#include <set>
#include "Message.h"

#define BUFFER_SIZE 1024
#define DELIM_CHAR  ' '


class TopicObserver {
public:
    virtual ~TopicObserver() = default;
    virtual void notify(const Message& message) = 0;
};

class ClientObserver : public TopicObserver {
public:
    int sock;
    int port;
    std::string ip;
    std::string name;
    std::set<std::string> topicsSubscribed;
    char buffer[BUFFER_SIZE];

    ClientObserver(int socket, int clientPort, const std::string& clientIp, const std::string& clientName);
    ClientObserver();
    void notify(const Message& message) override;
    int getSock() const;
    std::set<std::string>::iterator is_subscribed(std::string topicName);
    void subscribe(std::string topicName);
    void unsubscribe(std::string topicName);
};
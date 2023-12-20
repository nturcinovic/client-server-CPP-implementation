#pragma once

#include <iostream>
#include <vector>
#include "Message.h"
#include "ClientObserver.h"

class Topic {
private:
    std::string name;
    std::string data;
    std::vector<ClientObserver*> observers;

    void notifyObservers(const Message& message);
public:
    Topic(const Message &newMessage);
    void addObserver(ClientObserver* observer);
    void removeObserver(ClientObserver* observer);
    void publishMessage(const Message& message);
    std::string getName() const; 
};
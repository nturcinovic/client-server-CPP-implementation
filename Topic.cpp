#include "Topic.h"
#include <algorithm>

Topic::Topic(const Message &newMessage) : msg(newMessage) {}

void Topic::addObserver(ClientObserver* observer) {
    observers.push_back(observer);
}

void Topic::removeObserver(ClientObserver* observer) {
    auto it = std::find(observers.begin(), observers.end(), observer);
    if (it != observers.end()) {
        observers.erase(it);
    }
}

void Topic::publishMessage(const Message& message) {
    notifyObservers(message);
}

std::string Topic::getName() const {
    return msg.topic;
}

void Topic::notifyObservers(const Message& message) {
    for (auto &observer : observers) { 
        if (observer) {
            observer->notify(message);
        }
    }
}

#include "Topic.h"
#include <algorithm>

Topic::Topic(const Message &newMessage) : name(newMessage.topic), data(newMessage.data) {}

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
    return name;
}

void Topic::notifyObservers(const Message& message) {
    for (auto observer : observers) {
        observer->notify(message);
    }
}

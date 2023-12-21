#pragma once

#include <string>

class Message {
public:
    std::string topic;
    std::string data;

    Message(std::string topic, std::string data) : topic(topic), data(data) {}
    Message() : topic(""), data("") {}
};
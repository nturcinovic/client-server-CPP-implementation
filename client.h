#pragma once

#include "Commands.h"
#include <vector>

#define BUFFER_SIZE     1024

// Receiver: Client
class Client {
private:
    int clientfd;
    int port;
    std::string clientName;
    char buffer[BUFFER_SIZE];
    bool running;
    std::vector<Command *> commands;

    bool pollInput();
    bool processCommand();
    bool commandMatches(Command *command, const std::string &cmd);

public:
    Client();
    ~Client();
    void run();
};
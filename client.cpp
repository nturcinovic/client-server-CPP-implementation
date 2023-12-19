
#include <arpa/inet.h>
#include <sstream>
#include <cstring>
#include <poll.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <fcntl.h>
#include "Commands.h"
#include "client.h"


Client::Client() : clientfd(-1), port(0), running(true) {
    memset(buffer, 0, BUFFER_SIZE);

    // Initialize commands
    commands.push_back(new ConnectCommand(clientfd, port, clientName, buffer));
    commands.push_back(new DisconnectCommand(clientfd, running));
    commands.push_back(new PublishCommand(clientfd, buffer));
    commands.push_back(new SubscribeCommand(clientfd, buffer));
    commands.push_back(new UnsubscribeCommand(clientfd, buffer));
}

Client::~Client() {
    for (auto command : commands) {
        delete command;
    }
    commands.clear();
}

void Client::run() {
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("socket error");
        return;
    }

    /* get socket's flags */
    int flags = fcntl(clientfd, F_GETFL, 0);   
    if (flags < 0) 
    {
        perror("fcntl get");
        return;
    } 

    /* Apply the new flags to the socket */
    if (fcntl(clientfd, F_SETFL, flags | O_NONBLOCK) < 0) 
    {
        perror("fcntl set");
        return;
    }

    std::cout << "Client running." << std::endl;

    while (running) {
        if (pollInput()) {
            if(!processCommand()) {
                std::cout << "Error while processing command." << std::endl;
                break;
            }
        }

        memset(buffer, 0, BUFFER_SIZE);
        int received = recv(clientfd, buffer, sizeof(buffer), 0);
        if (received > 0) {
            std::string topicName, topicData;
            std::istringstream iss(buffer);
            iss >> topicName >> topicData;
            std::cout << "[Message] Topic: " << topicName << " Data: " << topicData << std::endl;
        }
    }

    std::cout << "Client closed." << std::endl;
}


bool Client::pollInput() {
    struct pollfd mypoll = {0, POLLIN};
    return poll(&mypoll, 1, 0);
}

bool Client::processCommand() {
    std::string cmd;
    std::getline(std::cin, cmd);
    bool unknownComand = true;

    // Remove the newline character if present
    size_t len = cmd.length();
    if (len > 0 && cmd[len - 1] == '\n') {
        cmd.pop_back();
    }

    for (auto command : commands) {
        if (commandMatches(command, cmd)) {
            strcpy(buffer, cmd.c_str());
            if(!command->execute())
            {
                return false;
            }
            unknownComand = false;
            break;
        }
    }
    if(unknownComand) {
        std::cout << "Unknown command" << std::endl;
    }
    return true;
}

bool Client::commandMatches(Command *command, const std::string &cmd) {
    std::istringstream iss(cmd);
    std::string commandType;
    iss >> commandType;
    
    // Convert commandType to uppercase for case-insensitive comparison
    std::transform(commandType.begin(), commandType.end(), commandType.begin(), ::toupper);

    // Compare with the known command types
    if (commandType == "CONNECT" && dynamic_cast<ConnectCommand *>(command)) {
        return true;
    } else if (commandType == "DISCONNECT" && dynamic_cast<DisconnectCommand *>(command)) {
        return true;
    } else if (commandType == "PUBLISH" && dynamic_cast<PublishCommand *>(command)) {
        return true;
    } else if (commandType == "SUBSCRIBE" && dynamic_cast<SubscribeCommand *>(command)) {
        return true;
    } else if (commandType == "UNSUBSCRIBE" && dynamic_cast<UnsubscribeCommand *>(command)) {
        return true;
    } 

    return false;
}

int main()
{
	Client client;
    client.run();

	return 0;
}

#pragma once

#include <iostream>

#define BUFFER_SIZE     1024

class Client;

// Command interface
class Command {
public:
    virtual ~Command() = default;
	virtual bool execute() = 0;
};

// Concrete Command: Connect
class ConnectCommand : public Command {
private:
    int &clientfd;
    int &port;
    std::string &clientName;
    char *buffer;

    bool connectToServer();
public:
    ConnectCommand(int &clientfd, int &port, std::string &clientName, char *buffer);
    bool execute() override;
};

// Concrete Command: Disconnect
class DisconnectCommand : public Command {
private:
	int &clientfd;
    bool &running;

    void disconnectFromServer();
public:
    DisconnectCommand(int &clientfd, bool &running);
	bool execute() override;
};

// Concrete Command: Publish
class PublishCommand : public Command {
private:
    int &clientfd;
    char *buffer;

    bool publishData();
public:
	PublishCommand(int &clientfd, char *buffer);
	bool execute() override;
};

// Concrete Command: Subscribe
class SubscribeCommand : public Command {
private:
    int &clientfd;
    char *buffer;

    bool subscribeToTopic();
public:
	SubscribeCommand(int &clientfd, char *buffer);
	bool execute() override;
};

// Concrete Command: Unsubscribe
class UnsubscribeCommand : public Command {
private:
    int &clientfd;
    char *buffer;

    bool unsubscribeFromTopic();
public:
	UnsubscribeCommand(int &clientfd, char *buffer);
	bool execute() override;
};
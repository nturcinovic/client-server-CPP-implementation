#pragma once

#include <iostream>

#define BUFFER_SIZE     1024
#define IP              "127.0.0.1"

class Client;

class Command {
public:
    virtual ~Command() = default;
	virtual bool execute() = 0;
};

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

class DisconnectCommand : public Command {
private:
	int &clientfd;
    bool &running;

    bool disconnectFromServer();
public:
    DisconnectCommand(int &clientfd, bool &running);
	bool execute() override;
};

class PublishCommand : public Command {
private:
    int &clientfd;
    char *buffer;

    bool publishData();
public:
	PublishCommand(int &clientfd, char *buffer);
	bool execute() override;
};

class SubscribeCommand : public Command {
private:
    int &clientfd;
    char *buffer;

    bool subscribeToTopic();
public:
	SubscribeCommand(int &clientfd, char *buffer);
	bool execute() override;
};

class UnsubscribeCommand : public Command {
private:
    int &clientfd;
    char *buffer;

    bool unsubscribeFromTopic();
public:
	UnsubscribeCommand(int &clientfd, char *buffer);
	bool execute() override;
};
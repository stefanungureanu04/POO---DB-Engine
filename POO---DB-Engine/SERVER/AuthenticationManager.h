#pragma once
#include "SocketLib.h"
#include "AuthenticationHandler.h"

class AuthenticationManager{
private:
    Socket serverSocket;
    AuthenticationHandler handler;

public:
    AuthenticationManager(int port);
    virtual ~AuthenticationManager() {};
    void run();
};


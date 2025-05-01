#pragma once
#include "SocketLib.h"
#include "AppHandler.h"

class AppManager{
private:
    Socket serverSocket;
    AppHandler handler;

public:
    AppManager(int port);
    virtual ~AppManager() {};
    void run();
};


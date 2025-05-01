#include "AppManager.h"
#include "AppLog.h"
#include <fstream>
#include <iostream>

AppManager::AppManager(int port) : serverSocket(Socket::Protocol::TCP)
{
    AppLog appLog;
	serverSocket.bindAndListen(port);
	appLog.write("[SERVER] Listening on port " + std::to_string(port));
}

void AppManager::run()
{
    AppLog appLog;

    while (true) {
        Socket* client = serverSocket.acceptConnection();
        appLog.write("[SERVER] Client connected.");

        std::string request = client->receiveData(1024);
        appLog.write("[SERVER] Received: " + request);

        std::string response = handler.handle(request);
        appLog.write("[SERVER] Sending: " + response);

        client->sendData(response);
        delete client;
    }
}

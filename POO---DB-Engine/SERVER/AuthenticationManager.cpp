#include "AuthenticationManager.h"
#include <fstream>
#include <iostream>

AuthenticationManager::AuthenticationManager(int port) : serverSocket(Socket::Protocol::TCP)
{
	serverSocket.bindAndListen(port);
	std::cout << "[SERVER] Listening on port " << port << "...\n";
}

void AuthenticationManager::run()
{
    while (true) {
        Socket* client = serverSocket.acceptConnection();
        std::cout << "[SERVER] Client connected.\n";

        std::string request = client->receiveData(1024);
        std::cout << "[SERVER] Received: " << request << "\n";

        std::string response = handler.handle(request);
        std::cout << "[SERVER] Sending: " << response << "\n";

        client->sendData(response);
        delete client;
    }
}

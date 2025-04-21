#include <iostream>
#include "SocketLib.h"
#include "AuthentificationHandler.h"
#include "UserCredentialsDatabase.h"

int main()
{
	// lasat pentru verificare functionalitate DLL
	SocketException exception("test", 100);

	std::cout << exception << std::endl;

    try {
        Socket serverSocket(Socket::Protocol::TCP);
        serverSocket.bindAndListen(12345);
        std::cout << "[SERVER] Listening on port 12345...\n";

        AuthentificationHandler handler;

        while (true) {
            Socket* client = serverSocket.acceptConnection();
            std::cout << "[SERVER] Client connected.\n";

            std::string request = client->receiveData(1024);
            std::cout << "[SERVER] Received: " << request << "\n";

            std::string response = handler.handle(request);
            std::cout << "[SERVER] Sending: " << response << "\n";

            client->sendData(response);
            delete client;  // Clean up client socket
        }

       // UserCredentialsDatabase::destroyInstance();  // Optional, if app ever ends
    }
    catch (const SocketException& e) {
    }

    return 0;


	std::cout << "hello from server" << std::endl;
	return 0;
}
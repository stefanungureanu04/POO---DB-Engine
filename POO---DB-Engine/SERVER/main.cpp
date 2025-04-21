#include <iostream>
#include "SocketLib.h"

int main()
{
	// lasat pentru verificare functionalitate DLL
	SocketException exception("test", 100);

	std::cout << exception;

	// testare socket
    Socket serverSocket(Socket::Protocol::TCP);
    serverSocket.bindAndListen(12345);

    std::cout << "Server listening on port 12345...\n";

    while (true) {
        Socket* client = serverSocket.acceptConnection();
        std::cout << "Client connected.\n";

        // Receive and display data
        std::string message = client->receiveData(1024);
        std::cout << "Received from client: " << message << "\n";

        // Send a fixed reply
        client->sendData("OK");

        delete client;
        std::cout << "Client disconnected.\n\n";
    }

	std::cout << "hello from server";
	return 0;
}
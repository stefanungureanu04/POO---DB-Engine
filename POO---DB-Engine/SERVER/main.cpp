#include <iostream>
#include "SocketLib.h"
#include "AuthenticationManager.h"

int main()
{

    try {
        AuthenticationManager manager(12345);
        manager.run();
    }
    catch (const SocketException& e) {
        std::cerr << "[SERVER] SocketException: " << e << "\n";
    }
	return 0;
}
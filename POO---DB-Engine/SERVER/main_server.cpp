#include <iostream>
#include "SocketLib.h"
#include "AppManager.h"

int main()
{
    try {
        AppManager manager(12345);
        manager.run();
    }
    catch (const SocketException& e) {
        std::cerr << "[SERVER] SocketException: " << e << "\n";
    }

	return 0;
}
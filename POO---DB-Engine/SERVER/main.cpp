#include <iostream>
#include "SocketLib.h"

int main()
{
	// lasat pentru verificare functionalitate DLL
	SocketException exception("test", 100);

	std::cout << exception;

	std::cout << "hello from server";
	return 0;
}
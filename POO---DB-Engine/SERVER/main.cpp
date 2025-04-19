#include <iostream>
#include "SocketLib.h"

int main()
{
	SocketException exception("test", 100);

	std::cout << exception;

	std::cout << "hello from server";
	return 0;
}
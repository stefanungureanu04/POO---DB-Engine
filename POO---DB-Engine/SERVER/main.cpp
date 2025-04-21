#include <iostream>
#include "SocketLib.h"
#include "UserCredentialsDatabase.h"

int main()
{
	// lasat pentru verificare functionalitate DLL
	SocketException exception("test", 100);

	std::cout << exception << std::endl;

	UserCredentialsDatabase& userCredentialsDatabase = UserCredentialsDatabase::getInstance();

	userCredentialsDatabase.showDatabase();


	std::cout << "hello from server" << std::endl;
	return 0;
}
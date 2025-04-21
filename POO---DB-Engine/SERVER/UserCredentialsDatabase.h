#pragma once
#include <string>
#include <fstream>
#include <vector>

class UserCredentialsDatabase{
private:
	std::vector <std::pair<std::string, std::string>> credentials;
	
	static const std::string filename;
	static UserCredentialsDatabase* instance;

public:
	static UserCredentialsDatabase& getInstance();
	static void destroyInstance();
	void loadDatabase();
	void eraseDatabase();
	const int size() const;
	void addCredentials(const std::string& username, const std::string& password);
	void removeCredentials(const std::string& username, const std::string& password);
	const bool foundCredentials(const std::string& username, const std::string& password) const;
	void showDatabase() const;

private:
	UserCredentialsDatabase();
	virtual ~UserCredentialsDatabase() {};
	UserCredentialsDatabase(const UserCredentialsDatabase& other) {};
};


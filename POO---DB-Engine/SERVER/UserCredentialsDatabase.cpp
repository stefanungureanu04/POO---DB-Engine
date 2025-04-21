#include "UserCredentialsDatabase.h"
#include <iostream>

const std::string UserCredentialsDatabase::filename = "user_credentials_db.txt";

UserCredentialsDatabase* UserCredentialsDatabase::instance = nullptr;

UserCredentialsDatabase::UserCredentialsDatabase()
{
	this->loadDatabase();
}

UserCredentialsDatabase& UserCredentialsDatabase::getInstance()
{
	if (UserCredentialsDatabase::instance == nullptr) {
		UserCredentialsDatabase::instance = new UserCredentialsDatabase();
	}

	return (*UserCredentialsDatabase::instance);
}

void UserCredentialsDatabase::destroyInstance()
{
	if (UserCredentialsDatabase::instance != nullptr) {
		delete UserCredentialsDatabase::instance;
	}

	UserCredentialsDatabase::instance = nullptr;
}

void UserCredentialsDatabase::loadDatabase()
{
	this->credentials.clear();

	std::ifstream file(UserCredentialsDatabase::filename);

	std::string username{};
	std::string password{};

	while (file >> username >> password) {
		this->credentials.emplace_back(username, password);
	}
}

void UserCredentialsDatabase::eraseDatabase()
{
	this->credentials.clear();
	std::ofstream file(UserCredentialsDatabase::filename, std::ios::trunc);
}

const int UserCredentialsDatabase::size() const
{
	return this->credentials.size();
}

void UserCredentialsDatabase::addCredentials(const std::string& username, const std::string& password)
{
	if (this->foundUsername(username)) {
		return;
	}

	this->credentials.emplace_back(username, password);

	std::ofstream file(UserCredentialsDatabase::filename, std::ios::app);
	if (file.is_open()) {
		file << username << " " << password << "\n";
	}
}

void UserCredentialsDatabase::removeCredentials(const std::string& username, const std::string& password)
{
	const auto originalSize = credentials.size();

	this->credentials.erase(std::remove_if(credentials.begin(), credentials.end(),
			[&](const auto& pair) {
				return pair.first == username && pair.second == password;
			}),
		credentials.end()
	);

	if (this->credentials.size() != originalSize){
		std::ofstream file(UserCredentialsDatabase::filename, std::ios::trunc);

		for (const auto& credential : this->credentials) {
			file << credential.first << " " << credential.second << "\n";
		}
	}
}

const bool UserCredentialsDatabase::foundCredentials(const std::string& username, const std::string& password) const
{
	if (this->credentials.size() == 0) {
		return false;
	}

	for (const auto& pair : this->credentials) {
		if (pair.first == username && pair.second == password)
			return true;
	}

	return false;
}

const bool UserCredentialsDatabase::foundUsername(const std::string& username) const
{
	if (this->credentials.size() == 0) {
		return false;
	}

	for (const auto& pair : this->credentials) {
		if (pair.first == username) {
			return true;
		}
	}

	return false;
}

void UserCredentialsDatabase::showDatabase() const
{
	if (this->size() == 0) {
		std::cout << "EMPTY" << std::endl;
		return;
	}

	for (const auto& pair : this->credentials) {
		std::cout << "username: " << pair.first << "\t\t" << "password: " << pair.second;
		std::cout << std::endl;
	}
}

#include "AuthenticationManager.h"
#include "UserCredentialsDatabase.h"

AuthenticationManager::AuthenticationManager(const std::string& request)
{
	this->request = request;
}

const std::string AuthenticationManager::processAuthentication()
{
    if (request.rfind("LOGIN:", 0) == 0) {
        std::string data = request.substr(6);
        size_t sep = data.find(':');
        if (sep == std::string::npos) return "INVALID_FORMAT";

        std::string username = data.substr(0, sep);
        std::string password = data.substr(sep + 1);

        if (UserCredentialsDatabase::getInstance().foundCredentials(username, password)) {
            return "LOGIN_SUCCESS";
        }
        else if (UserCredentialsDatabase::getInstance().foundUsername(username)) {
            return "WRONG_PASSWORD";
        }
        else {
            return "LOGIN_FAIL";
        }
    }

    else if (request.rfind("REGISTER:", 0) == 0) {
        std::string data = request.substr(9);
        size_t sep = data.find(':');
        if (sep == std::string::npos) return "INVALID_FORMAT";

        std::string username = data.substr(0, sep);
        std::string password = data.substr(sep + 1);

        if (UserCredentialsDatabase::getInstance().foundUsername(username)) {
            return "USERNAME_TAKEN";
        }

        UserCredentialsDatabase::getInstance().addCredentials(username, password);
        return "REGISTER_SUCCESS";
    }
}

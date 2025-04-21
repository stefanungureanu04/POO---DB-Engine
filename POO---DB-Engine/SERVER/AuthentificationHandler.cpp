#include "AuthentificationHandler.h"

std::string AuthentificationHandler::handle(const std::string& request)
{
    if (request.rfind("LOGIN:", 0) == 0) {
        std::string data = request.substr(6);
        size_t sep = data.find(':');
        if (sep == std::string::npos) return "INVALID_FORMAT";

        std::string username = data.substr(0, sep);
        std::string password = data.substr(sep + 1);

        return UserCredentialsDatabase::getInstance().foundCredentials(username, password)
            ? "LOGIN_SUCCESS"
            : "LOGIN_FAIL";
    }
    else if (request.rfind("REGISTER:", 0) == 0) {
        std::string data = request.substr(9);
        size_t sep = data.find(':');
        if (sep == std::string::npos) return "INVALID_FORMAT";

        std::string username = data.substr(0, sep);
        std::string password = data.substr(sep + 1);

        if (UserCredentialsDatabase::getInstance().foundCredentials(username, password)) {
            return "USERNAME_TAKEN";
        }

        UserCredentialsDatabase::getInstance().addCredentials(username, password);
        return "REGISTER_SUCCESS";
    }

    return "UNKNOWN_COMMAND";
}

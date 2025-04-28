#include "AuthenticationHandler.h"
#include "AppLog.h"
#include <filesystem>
#include<iostream>

std::string AuthenticationHandler::handle(const std::string& request)
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

    //sectiune care proceseaza comanda GET_DATABASES ca urmare a apasarii butonului CurrentDatabase
    //din CLIENT (pentru a putea afisa bazele de date ale utilizatorului intr-o fereastra dedicata 
    //si sa-si aleaga de acolo in care sa lucreze)
    else if (request.rfind("GET_DATABASES:", 0) == 0) {
        std::string username = request.substr(14);
        //std::string dbFilePath = "user_databases/" + username + ".txt";
        std::string db_filename = username + ".txt";
        std::ifstream file(db_filename);

        if (!file.is_open()) {
            AppLog log;
            log.write("[SERVER] Could not open database list file: " + db_filename);
            return "DBLIST:";  // trimite răspuns gol
        }


        if (!file.is_open()) {
            return "DBLIST:";  // Nu există baze de date
        }

        std::string dbName;
        std::string response = "DBLIST:";
        bool first = true;

        while (std::getline(file, dbName)) {
            if (!first) {
                response += ";";
            }
            response += dbName;
            first = false;
        }

        return response;
    }

    return "UNKNOWN_COMMAND";
}

#include "User.h"
#include <ctype.h>

User::User(const std::string& username, const std::string& password)
{
	this->username = username;
	this->password = password;
}

const std::string& User::getUsername() const
{
	return this->username;
}

const std::string& User::getPassword() const
{
	return this->password;
}

const bool User::username_isValid() const
{
    if (this->username.empty()) {
        return false;
    }

    bool hasDot = false;

    for (char ch : this->username) {
        if (ch == '.') {
            hasDot = true;
        }
        else if (ch < 'a' || ch > 'z') {
            return false; 
        }
    }

    return hasDot;
}

const bool User::password_isValid() const
{
    if (this->password.empty()) {
        return false;
    }

    if (this->password.find(username) != std::string::npos || this->password.find(' ')!=std::string::npos) {
        return false;
    }

    bool hasDigit = false;
    bool hasSpecial = false;

    for (const char character : this->password) {
        if (isdigit(character)) {
            hasDigit = true;
        }
        else if (!isalpha(character)) {
            hasSpecial = true;
        }
    }

    return hasDigit && hasSpecial;
}



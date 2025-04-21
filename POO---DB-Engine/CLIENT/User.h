#include <string>

class User {
private:
	std::string username;
	std::string password;

public:
	User(const std::string& username, const std::string& password);
	virtual ~User() {};
	const std::string& getUsername() const;
	const std::string& getPassword() const;
	const bool username_isValid() const;
	const bool password_isValid() const;
};
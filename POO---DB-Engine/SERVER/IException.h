#pragma once
#include <string>
#include <ostream>

class IException{
protected:
	std::string m_Message;
	int m_Code;

public:
	virtual ~IException() = 0 {};
	virtual const std::string& getMessage() const = 0;
	virtual const int& getCode() const = 0;
	virtual const std::string type() const = 0;
	virtual void print() const = 0;
	friend std::ostream& operator << (std::ostream& out, IException& e);
	friend std::ostream& operator << (std::ostream& out, IException* e);
};


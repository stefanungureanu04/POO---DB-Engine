#pragma once
#include "IException.h"
#include <iostream>

class CException : public IException {
public:
	virtual ~CException() {};
	CException(const std::string& message, const int& code);
	const std::string& getMessage() const override;
	const int& getCode() const override;
	void print() const override;
};


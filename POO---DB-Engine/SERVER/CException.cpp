#include "CException.h"

CException::CException(const std::string& message, const int& code)
{
	m_Message = message;
	m_Code = code;
}

const std::string& CException::getMessage() const
{
	return m_Message;
}

const int& CException::getCode() const
{
	return m_Code;
}

void CException::print() const
{
	std::cout << "Exception::" + this->type() + ": " + this->getMessage() + " (error #" + std::to_string(this->getCode()) + ")";
}

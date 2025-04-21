#pragma once
#include "ILog.h"
#include <fstream>
#include <iostream>

class CLog : public ILog{
protected:
	std::string filename;

public:
	CLog(const std::string& filename);
	CLog() {};
	void showLog() const override;
	void erase() override;
};


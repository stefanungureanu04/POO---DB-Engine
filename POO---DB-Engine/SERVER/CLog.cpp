#include "CLog.h"

CLog::CLog(const std::string& filename)
{
    this->filename = filename;
}

void CLog::showLog() const
{
    std::ifstream file(this->filename);

    int numRow = 0;
    std::string line;

    while (std::getline(file, line)) {
        std::cout <<++numRow<<". "<<line << std::endl;
    }
}

void CLog::erase()
{
    std::ofstream file(this->filename, std::ios::trunc);
}

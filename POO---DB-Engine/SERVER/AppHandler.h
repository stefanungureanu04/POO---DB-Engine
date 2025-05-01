#pragma once
#include <string>

class AppHandler {
public:
    std::string handle(const std::string& request);

private:
   bool beginsWith(const std::string& request, const std::string& prefix);
};

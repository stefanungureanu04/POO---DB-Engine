#include "IException.h"

std::ostream& operator<<(std::ostream& out, IException& e)
{
    std::string stringify{};
    
    stringify += "Exception::" + e.type() + ": " + e.getMessage() + " (error #" + std::to_string(e.getCode()) + ")";

    out << stringify;

    return out;
}

std::ostream& operator<<(std::ostream& out, IException* e)
{
    std::string stringify{};

    stringify += "Exception::" + e->type() + ": " + e->getMessage() + " (error #" + std::to_string(e->getCode()) + ")";

    out << stringify;

    return out;
}

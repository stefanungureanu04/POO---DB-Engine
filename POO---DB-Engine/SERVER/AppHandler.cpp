#include "AppHandler.h"
#include "AuthenticationManager.h"
#include "DatabaseSelectManager.h"
#include "LogProviderManager.h"
#include "QueryManager.h"
#include "CommandManager.h"
#include "AppLog.h"

std::string AppHandler::handle(const std::string& request)
{
    if (request.rfind("LOGIN:", 0) == 0 || request.rfind("REGISTER:", 0) == 0) {
        AuthenticationManager authenticationManager(request);
        std::string response = authenticationManager.processAuthentication();
        return response;
    }
    else if (request.rfind("GET_DATABASES:", 0) == 0 || request.rfind("CREATE_DATABASE:", 0) == 0 || request.rfind("DELETE_DATABASE:",0) == 0) {
        DatabaseSelectManager databaseSelectManager(request);
        std::string response = databaseSelectManager.processDatabaseRequest();
        return response;
    }
    else if (request.rfind("GET_LOGS:", 0) == 0) {
        LogProviderManager logProviderManager(request);
        std::string response = logProviderManager.processLogRequests();
        return response;
    }
    else if (request.rfind("SAVE_QUERY:", 0) == 0 || request.rfind("LIST_QUERIES:", 0) == 0 || request.rfind("LOAD_QUERY:", 0) == 0) {
        QueryManager queryManager(request);
        std::string response = queryManager.processQueryRequest();
        return response;
    }
    else  if (request.rfind("EXECUTE_CODE:", 0) == 0) {
        CommandManager commandManager(request);
        std::string response = commandManager.processCommand();
        return response;
    }

    return "UNKNOWN_COMMAND";
}

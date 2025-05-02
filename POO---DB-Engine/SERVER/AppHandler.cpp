#include "AppHandler.h"
#include "AuthenticationManager.h"
#include "DatabaseSelectManager.h"
#include "LogProviderManager.h"
#include "QueryManager.h"
#include "CommandManager.h"
#include "AppLog.h"
#include"DatabaseLoaderManager.h"
#include"TableListManager.h"

std::string AppHandler::handle(const std::string& request)
{
    if (beginsWith(request,"LOGIN:") || beginsWith(request,"REGISTER:")) {
        AuthenticationManager authenticationManager(request);
        std::string response = authenticationManager.processAuthentication();
        return response;
    }
    else if (beginsWith(request,"GET_DATABASES:") || beginsWith(request,"CREATE_DATABASE:") || beginsWith(request, "DELETE_DATABASE:")) {
        DatabaseSelectManager databaseSelectManager(request);
        std::string response = databaseSelectManager.processDatabaseRequest();
        return response;
    }
    else if (beginsWith(request, "GET_LOGS:")) {
        LogProviderManager logProviderManager(request);
        std::string response = logProviderManager.processLogRequests();
        return response;
    }
    else if (beginsWith(request,"SAVE_QUERY:") || beginsWith(request,"LIST_QUERIES:") || beginsWith(request,"LOAD_QUERY:")) {
        QueryManager queryManager(request);
        std::string response = queryManager.processQueryRequest();
        return response;
    }
    else if (beginsWith(request,"EXECUTE_CODE:")) {
        CommandManager commandManager(request);
        std::string response = commandManager.processCommand();
        return response;
    }
    else if (beginsWith(request, "LOAD_DATABASE:")) {
        DatabaseLoaderManager loader(request);
        return loader.process();
    }
    else if (request.rfind("GET_TABLES:", 0) == 0) {
        TableListManager manager(request);
        return manager.process();
    }

    return "UNKNOWN_COMMAND";
}

bool AppHandler::beginsWith(const std::string& request, const std::string& prefix)
{
    return request.rfind(prefix, 0) == 0;
}

#pragma once
#include "AuthenticationWindow.h"
#include "EnvironmentWindow.h"

class AppController : public QObject {

    Q_OBJECT

private:
    AuthenticationWindow* authenticationWindow = nullptr;
    EnvironmentWindow* environmentWindow = nullptr;

public:
    void run();
    void showAuthentication();
    void showEnvironment(const QString& username);
};


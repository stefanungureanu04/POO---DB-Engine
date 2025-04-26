#pragma once
#include "AuthenticationWindow.h"
#include "EnvironmentWindow.h"

class AppController : public QObject {

    Q_OBJECT

private:
    QWidget* currentWindow;

public:
    void run();
    void setCurrentWindow(QWidget* window);
    void showAuthentication();
    void showEnvironment(const QString& username);
};


#include "AppController.h"
#include "EnvironmentWindow.h"
#include <qobject.h>

void AppController::run()
{
    showAuthentication();
}

void AppController::showAuthentication()
{
    AuthenticationWindow* authenticationWindow = new AuthenticationWindow();

    QObject::connect(authenticationWindow, &AuthenticationWindow::loginSuccess, this, [this, authenticationWindow](const QString& username) {
            this->showEnvironment(username);
            authenticationWindow->close();
        });

    authenticationWindow->setAttribute(Qt::WA_DeleteOnClose);
    authenticationWindow->show();

    this->setCurrentWindow(authenticationWindow);
}

void AppController::showEnvironment(const QString& username)
{
    EnvironmentWindow* ide = new EnvironmentWindow(username);

    QObject::connect(ide, &EnvironmentWindow::logoutRequested, this, [this, ide]() {
            this->showAuthentication();
            ide->close();
        });

    ide->setAttribute(Qt::WA_DeleteOnClose);
    ide->show();

   this -> setCurrentWindow(ide);
}

void AppController::setCurrentWindow(QWidget* window)
{
    if (this->currentWindow) {
        this->currentWindow->deleteLater();
    }

    this->currentWindow = window;
}

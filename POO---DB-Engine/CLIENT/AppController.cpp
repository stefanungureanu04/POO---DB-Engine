#include "AppController.h"
#include "EnvironmentWindow.h"
#include <qobject.h>

void AppController::run()
{
    showAuthentication();
}

void AppController::showAuthentication()
{
    if (authenticationWindow != nullptr) {
        authenticationWindow->close();
        authenticationWindow->deleteLater();
        authenticationWindow = nullptr;
    }

     authenticationWindow = new AuthenticationWindow(nullptr);
     
     QObject::connect(authenticationWindow, &AuthenticationWindow::loginSuccess, this, [this](const QString& username) {
         
         this->showEnvironment(username);

         if (authenticationWindow != nullptr) {
             authenticationWindow->accept();
             authenticationWindow->deleteLater();
             authenticationWindow = nullptr;
         }
      });

     QObject::connect(authenticationWindow, &AuthenticationWindow::loginFailed, this, [this]() {

         if (authenticationWindow != nullptr) {
             authenticationWindow->close();
             authenticationWindow->deleteLater();
             authenticationWindow = nullptr;
         }

         this->showAuthentication();
      });

     QObject::connect(authenticationWindow, &AuthenticationWindow::signupCompleted, this, [this]() {

         if (authenticationWindow != nullptr) {
             authenticationWindow->close();
             authenticationWindow->deleteLater();
             authenticationWindow = nullptr;
         }

        this->showAuthentication(); 
    });

     authenticationWindow->setAttribute(Qt::WA_DeleteOnClose);
     authenticationWindow->show();
}

void AppController::showEnvironment(const QString& username)
{
    if (environmentWindow!=nullptr) {
        environmentWindow->close();
        environmentWindow->deleteLater();
        environmentWindow = nullptr;
    }

    environmentWindow = new EnvironmentWindow(username);

    QObject::connect(environmentWindow, &EnvironmentWindow::logoutRequested, this, [this]() {

        this->showAuthentication();
        environmentWindow->close();

        if (environmentWindow != nullptr) {
            environmentWindow->close();
            environmentWindow->deleteLater();
            environmentWindow = nullptr;
        }
     });

    environmentWindow->setAttribute(Qt::WA_DeleteOnClose);
    environmentWindow->show();

    if (authenticationWindow != nullptr) {
        authenticationWindow->close();
        authenticationWindow->deleteLater();
        authenticationWindow = nullptr;
    }
}
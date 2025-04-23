#include "AppController.h"
#include "EnvironmentWindow.h"
#include <qobject.h>

void AppController::run()
{

    AuthenticationWindow* authWindow = new AuthenticationWindow();
    QObject::connect(authWindow, &AuthenticationWindow::loginSuccess, [authWindow](const QString& username) {
        EnvironmentWindow* ide = new EnvironmentWindow(username);
        ide->setAttribute(Qt::WA_DeleteOnClose);
        ide->show();
        authWindow->close();
        });
    authWindow->show();
}

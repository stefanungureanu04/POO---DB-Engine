#include "CLIENT.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <QDebug>
#include "AuthenticationWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AuthenticationWindow authenticationWindow;
    authenticationWindow.show();

    return app.exec();
}

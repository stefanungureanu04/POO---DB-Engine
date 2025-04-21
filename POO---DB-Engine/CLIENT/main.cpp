#include "CLIENT.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <QDebug>
#include "AuthentificationWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AuthentificationWindow authentificationWindow;
    authentificationWindow.show();

    return app.exec();
}

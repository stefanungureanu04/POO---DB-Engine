#include "CLIENT.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <QDebug>

int main(int argc, char *argv[])
{
    qDebug() << "Hello";
    QApplication a(argc, argv);
    CLIENT w;
    w.show();
    return a.exec();
}

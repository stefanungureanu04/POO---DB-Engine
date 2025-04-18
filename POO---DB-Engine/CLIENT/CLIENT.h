#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CLIENT.h"

class CLIENT : public QMainWindow
{
    Q_OBJECT

public:
    CLIENT(QWidget *parent = nullptr);
    ~CLIENT();

private:
    Ui::CLIENTClass ui;
};

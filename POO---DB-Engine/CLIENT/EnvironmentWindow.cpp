#include "EnvironmentWindow.h"
#include "ui_EnvironmentWindow.h"


EnvironmentWindow::EnvironmentWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::EnvironmentWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("IDE");
    this->resize(1200,700);
}

EnvironmentWindow::~EnvironmentWindow()
{
    delete ui;
}


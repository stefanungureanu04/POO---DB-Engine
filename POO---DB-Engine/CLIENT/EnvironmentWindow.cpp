#include "EnvironmentWindow.h"
#include "ui_EnvironmentWindow.h"
#include "OptionsDialog.h"


EnvironmentWindow::EnvironmentWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::EnvironmentWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("IDE");
    this->resize(1200,700);
}

void EnvironmentWindow::on_optionsButton_clicked()
{
    OptionsDialog dialog(this);
    dialog.exec();                      //se deschide fereastra de dialog: OPTIUNI
}

EnvironmentWindow::~EnvironmentWindow()
{
    delete ui;
}


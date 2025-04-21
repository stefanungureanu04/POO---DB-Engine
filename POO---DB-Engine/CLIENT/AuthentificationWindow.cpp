#include "AuthentificationWindow.h"
#include "ui_AuthentificationWindow.h" 
#include <QMessageBox>


AuthentificationWindow::AuthentificationWindow(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::AuthentificationWindow)
{
    ui->setupUi(this);
}

AuthentificationWindow::~AuthentificationWindow()
{
    delete ui;
}

void AuthentificationWindow::on_okButton_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if (username == "admin" && password == "1234") {
        accept(); // Login success
    }
    else {
        QMessageBox::warning(this, "Login Failed", "Incorrect username or password.");
    }
}

void AuthentificationWindow::on_cancelButton_clicked()
{
    reject(); // Close dialog
}


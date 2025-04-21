#include "AuthentificationWindow.h"
#include "ui_AuthentificationWindow.h" 
#include <QMessageBox>
#include "SocketLib.h"
#include "SignUpWindow.h"


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

void AuthentificationWindow::on_signupButton_clicked()
{
    SignUpWindow signUp(this);
    int result = signUp.exec();

    if (result == QDialog::Accepted) {
        // Optional: refill the fields or show a success message
        QMessageBox::information(this, "Success", "You can now log in with your new credentials.");
        ui->usernameEdit->clear();
        ui->passwordEdit->clear();
        this->show();
    }
}

void AuthentificationWindow::on_loginButton_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill in both fields.");
        return;
    }

    // Regardless of credentials — always show a result
    QMessageBox::information(this, "Login Attempt",
        "Login attempted with:\nUsername: " + username + "\nPassword: " + password);

    // Clear the form so user can try again
    ui->usernameEdit->clear();
    ui->passwordEdit->clear();

    this->show();
}




#include "SignUpWindow.h"
#include "ui_SignUpWindow.h"
#include "SocketLib.h"
#include "User.h"
#include <QMessageBox>

SignUpWindow::SignUpWindow(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SignUpWindow)
{
    ui->setupUi(this);
}

SignUpWindow::~SignUpWindow()
{
    delete ui;
}

void SignUpWindow::on_registerButton_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill in both fields.");
        return;
    }

    User user(username.toStdString(), password.toStdString());

    if (!user.username_isValid()) {
        QMessageBox::warning(this, "Invalid Username",
            "Username must only contain lowercase letters and at least one dot.");
        return;
    }

    if (!user.password_isValid()) {
        QMessageBox::warning(this, "Invalid Password",
            "Password must contain at least one digit and one special character, "
            "cannot contain spaces or the username.");
        return;
    }

    // TODO: Send to server via socket OR write to file
    QMessageBox::information(this, "Success", "Registration successful. Please log in.");

    this->accept(); //  Close dialog and return to login form
}


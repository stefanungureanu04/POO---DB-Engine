#include "AuthenticationWindow.h"
#include "ui_AuthentificationWindow.h" 
#include <QMessageBox>
#include <qcheckbox.h>
#include "SocketLib.h"
#include "SignUpWindow.h"


AuthenticationWindow::AuthenticationWindow(QWidget* parent) : QDialog(parent), ui(new Ui::AuthenticationWindow)
{
    ui->setupUi(this);

    connect(ui->showPasswordCheckbox, &QCheckBox::toggled, this, &AuthenticationWindow::onShowPasswordToggled);
}

AuthenticationWindow::~AuthenticationWindow()
{
    delete ui;
}

void AuthenticationWindow::onShowPasswordToggled(bool checked)
{
    if (checked) {
        ui->passwordEdit->setEchoMode(QLineEdit::Normal);
    }
    else {
        ui->passwordEdit->setEchoMode(QLineEdit::Password);
    }
}

void AuthenticationWindow::on_signupButton_clicked()
{
    SignUpWindow signUp(this);
    int result = signUp.exec();

    if (result == QDialog::Accepted) {
        QMessageBox::information(this, "Success", "You can now log in with your new credentials.");
        emit signupCompleted();
    }
}

void AuthenticationWindow::on_loginButton_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill in both fields.");
        return;
    }
 
    try {
        Socket socket(Socket::Protocol::TCP);

        if (!socket.connectToServer("127.0.0.1", 12345)) {
            QMessageBox::critical(this, "Error", "Cannot connect to the server.");
            return;
        }

        std::string message = "LOGIN:" + username.toStdString() + ":" + password.toStdString();
        socket.sendData(message);
        std::string response = socket.receiveData(1024);

        if (response == "LOGIN_SUCCESS") {         
            QMessageBox::information(this, "Login Successful", "Welcome, " + username + "!");
            emit loginSuccess(username);
        }

        else if (response == "WRONG_PASSWORD") {
            QMessageBox::warning(this, "Login Failed", "Incorrect password. Please try again.");   
            emit loginFailed();
        }
        else {
            QMessageBox::warning(this, "Login Failed", "Account not found. Redirecting to sign up...");
            on_signupButton_clicked();
        }

    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Socket Error", e.what());
    }
}




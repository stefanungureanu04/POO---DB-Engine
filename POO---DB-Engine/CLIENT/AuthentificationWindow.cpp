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
            ui->usernameEdit->clear();
            ui->passwordEdit->clear();
            // Optionally call main page or keep form open
        }
        else {
            QMessageBox::warning(this, "Login Failed", "Account not found. Redirecting to sign up...");
            on_signupButton_clicked();  // open sign-up dialog
        }

    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Socket Error", e.what());
    }
}




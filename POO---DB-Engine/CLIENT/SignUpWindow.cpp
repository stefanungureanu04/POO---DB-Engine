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
        QMessageBox::warning(this, "Invalid Username", "Username must contain only lowercase letters and a dot.");
        return;
    }

    if (!user.password_isValid()) {
        QMessageBox::warning(this, "Invalid Password", "Password must contain at least one digit, one special character, no spaces, and not include the username.");
        return;
    }

    try {
        Socket socket(Socket::Protocol::TCP);
        if (!socket.connectToServer("127.0.0.1", 12345)) {
            QMessageBox::critical(this, "Error", "Cannot connect to the server.");
            return;
        }

        std::string message = "REGISTER:" + username.toStdString() + ":" + password.toStdString();
        socket.sendData(message);
        std::string response = socket.receiveData(1024);

        if (response == "REGISTER_SUCCESS") {
            QMessageBox::information(this, "Registration Complete", "Account created. Please log in.");
            this->accept();  // close the sign-up dialog
        }
        else if (response == "USERNAME_TAKEN") {
            QMessageBox::warning(this, "Username Exists", "This username is already registered.");
        }
        else {
            QMessageBox::warning(this, "Server Error", QString::fromStdString(response));
        }

    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Socket Error", e.what());
    }
}


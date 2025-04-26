#include "EnvironmentWindow.h"
#include "ui_EnvironmentWindow.h"
#include "OptionsDialog.h"
#include "AuthenticationWindow.h"
#include "DatabaseSelect.h"
#include "SocketLib.h"
#include <QMessageBox>



EnvironmentWindow::EnvironmentWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::EnvironmentWindow)
{
    ui->setupUi(this);          // trebuie sa fie prezent aici, altfel conexiunea nu are loc

    // other initialization here
}

EnvironmentWindow::EnvironmentWindow(const QString& username, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::EnvironmentWindow)
{
    ui->setupUi(this);
    ui->labelUsername->setText("-> " + username + " <- MANAGER");   //apare utilizatorul curent in stanga cuvantului MANAGER
}

void EnvironmentWindow::on_optionsButton_clicked()
{

    OptionsDialog* dialog = new OptionsDialog(this);

    connect(dialog, &OptionsDialog::textSizeChanged,
        this, &EnvironmentWindow::updateEditorFontSize);


    dialog->exec();
}

void EnvironmentWindow::updateEditorFontSize(int size) {
    QFont font = ui->EditorText->font();  // replace `EditorText` with your actual QPlainTextEdit name
    font.setPointSize(size);
    ui->EditorText->setFont(font);
}

void EnvironmentWindow::setCurrentUsername(const QString& username) 
{
    this->currentUsername = username;
    ui->labelUsername->setText("-> " + username + " <- MANAGER");
}

void EnvironmentWindow::on_currentDatabaseButton_clicked()
{
    QStringList userDatabases;

    QMessageBox::information(this, "DEBUG", "Current username: " + currentUsername);

    Socket socket(Socket::Protocol::TCP);
    if (!socket.connectToServer("127.0.0.1", 12345)) {
        QMessageBox::critical(this, "Error", "Could not connect to server.");
        return;
    }

    std::string request = "GET_DATABASES:" + currentUsername.toStdString();
    socket.sendData(request);

    std::string response = socket.receiveData(2048);
    QString qResponse = QString::fromStdString(response);

    if (qResponse.startsWith("DBLIST:")) {
        QString dbList = qResponse.mid(7);
        if (!dbList.isEmpty()) {
            userDatabases = dbList.split(';');
        }
    }

    DatabaseSelect dialog(this);

    if (userDatabases.isEmpty()) {
        dialog.getNoDatabaseLabel()->setVisible(true);
        dialog.getDatabaseListWidget()->setVisible(false);
    }
    else {
        dialog.getNoDatabaseLabel()->setVisible(false);
        dialog.getDatabaseListWidget()->setVisible(true);
        for (const QString& db : userDatabases) {
            dialog.getDatabaseListWidget()->addItem(db);
        }
    }

    dialog.exec();
}


EnvironmentWindow::~EnvironmentWindow()
{
    delete ui;
}


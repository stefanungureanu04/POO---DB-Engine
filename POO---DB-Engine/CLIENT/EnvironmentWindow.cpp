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

void EnvironmentWindow::on_currentDatabaseButton_clicked()
{
    QStringList userDatabases;

    //1. Creez socket și trimit cererea
    Socket socket(Socket::Protocol::TCP);
    if (!socket.connectToServer("127.0.0.1", 12345)) {
        QMessageBox::critical(this, "Error", "Could not connect to server.");
        return;
    }

    QString username = this->currentUsername;  // Asigură-te că ai salvat username-ul după login
    std::string request = "GET_DATABASES:" + username.toStdString();
    socket.sendData(request);

    std::string response = socket.receiveData(2048);
    QString qResponse = QString::fromStdString(response);

    //2. Parsăm răspunsul
    if (qResponse.startsWith("DBLIST:")) {
        QString dbList = qResponse.mid(7);
        if (!dbList.isEmpty()) {
            userDatabases = dbList.split(';');
        }
    }

    //3. Deschide dialogul și setează lista
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


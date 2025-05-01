#include "DatabaseSelect.h"
#include "ui_DatabaseSelect.h"
#include <qinputdialog.h>
#include <SocketLib.h>
#include <QMessageBox>

DatabaseSelect::DatabaseSelect(const QString& username, QWidget* parent) : QDialog(parent), ui(new Ui::DatabaseSelect)
{
    this->currentUsername = username;
    ui->setupUi(this);
    connect(ui->databaseListWidget, &QListWidget::itemDoubleClicked, this, &DatabaseSelect::onDatabaseItemDoubleClicked);
}

DatabaseSelect::~DatabaseSelect()
{
    delete ui;
}

QLabel* DatabaseSelect::getNoDatabaseLabel()
{
    return ui->noDatabaseLabel;
}

QListWidget* DatabaseSelect::getDatabaseListWidget()
{
    return ui->databaseListWidget;
}

void DatabaseSelect::refreshDatabaseList()
{
    QStringList userDatabases;

    try {
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
        setDatabaseList(userDatabases);
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Socket Error", e.what());
    }
}

void DatabaseSelect::on_createNewDatabaseButton_clicked()
{
    bool ok;
    QString newDbName = QInputDialog::getText(this, "Create New Database", "Enter database name:", QLineEdit::Normal, "", &ok);

    if (ok == false || newDbName.isEmpty()) {
        return; 
    }

    try {
        Socket socket(Socket::Protocol::TCP);
        if (!socket.connectToServer("127.0.0.1", 12345)) {
            QMessageBox::critical(this, "Error", "Could not connect to server.");
            return;
        }

        std::string request = "CREATE_DATABASE:" + currentUsername.toStdString() + ":" + newDbName.toStdString();
        socket.sendData(request);
        std::string response = socket.receiveData(1024);

        if (response == "CREATE_DB_SUCCESS") {
            QMessageBox::information(this, "Database Created", "Database created successfully.");
            refreshDatabaseList(); 
        }
        else {
            QMessageBox::warning(this, "Error", QString::fromStdString(response));
        }
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Socket Error", e.what());
    }
}

void DatabaseSelect::setDatabaseList(const QStringList& databases)
{
    ui->databaseListWidget->clear();

    if (databases.isEmpty()) {
        ui->noDatabaseLabel->setVisible(true);
        ui->databaseListWidget->setVisible(false);
    }
    else {
        ui->noDatabaseLabel->setVisible(false);
        ui->databaseListWidget->setVisible(true);
        ui->databaseListWidget->addItems(databases);
    }
}

void DatabaseSelect::onDatabaseItemDoubleClicked(QListWidgetItem* item)
{
    if (item) {
        QString selectedDatabase = item->text();
        emit databaseSelected(selectedDatabase);
        accept();  
    }
}
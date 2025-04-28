#include "EnvironmentWindow.h"
#include "ui_EnvironmentWindow.h"
#include "OptionsDialog.h"
#include "AuthenticationWindow.h"
#include "DatabaseSelect.h"
#include "SocketLib.h"
#include <QMessageBox>

EnvironmentWindow::EnvironmentWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::EnvironmentWindow)
{
    ui->setupUi(this);                                     
}

EnvironmentWindow::EnvironmentWindow(const QString& username, QWidget* parent) : QMainWindow(parent), ui(new Ui::EnvironmentWindow)
{
    this->currentUsername = username;

    ui->setupUi(this);
    ui->labelUsername->setText(username + " - MANAGER");
}

void EnvironmentWindow::onDatabaseChosen(const QString& dbName)
{
    selectedDatabase = dbName;  // salvez baza selectată

    QString newLabelText = currentUsername + " - MANAGER\n" + selectedDatabase;
    ui->labelUsername->setText(newLabelText);
}

void EnvironmentWindow::updateEditorFontSize(int size)
{
    this->fontSize = size; 

    QFont font = ui->EditorText->font();
    font.setPointSize(size);
    ui->EditorText->setFont(font);
}

void EnvironmentWindow::toggleSyntaxHighlighter(bool enabled)
{
    syntaxHighlightingEnabled = enabled;

    if (enabled) {
        if (!highlighter) {
            highlighter = new SyntaxHighlighter(ui->EditorText->document());
        }

        highlighter->rehighlight();
    }
    else {
        if (highlighter) {
            delete highlighter;
            highlighter = nullptr;
        }

        QTextCursor cursor(ui->EditorText->document());
        cursor.select(QTextCursor::Document);

        QTextCharFormat format;
        format.setForeground(Qt::black);
        cursor.mergeCharFormat(format);
    }
}

void EnvironmentWindow::toggleExecutionTime(bool enabled)
{
    executionTimeEnabled = enabled;
    // You can handle what to do when execution time is toggled
}

void EnvironmentWindow::toggleHistoryCleanup(bool enabled)
{
    // You can handle what to do when the button is pressed (for now, just a debug pop-up message)
    if (enabled) {
        // Only when the box is checked
        QMessageBox::information(this, "HISTORY CLEANUP", "HISTORY CLEANUP");

        // Uncheck the checkbox manually

        // We need a pointer to the OptionsDialog that emitted the signal.
        OptionsDialog* dialog = qobject_cast<OptionsDialog*>(sender());
        if (dialog) {
            dialog->uncheckHistoryCleanup();
        }
    }
}

void EnvironmentWindow::deleteCurrentDatabase()
{
    // You can handle what to do when the button is pressed
    QMessageBox::information(this, "DELETE CURRENT DATABASE", "DELETE CURRENT DATABASE");
}

void EnvironmentWindow::on_runButton_clicked()
{
    // You can handle what to do when the button is pressed (for now, just a debug pop-up message)
    QMessageBox::information(this, "RUN", "RUN");
}

void EnvironmentWindow::on_importButton_clicked()
{
    // You can handle what to do when the button is pressed (for now, just a debug pop-up message)
    QMessageBox::information(this, "IMPORT", "IMPORT");
}

void EnvironmentWindow::on_logButton_clicked()
{
    // You can handle what to do when the button is pressed (for now, just a debug pop-up message)
    QMessageBox::information(this, "LOG", "LOG");
}

void EnvironmentWindow::on_downloadButton_clicked()
{
    // You can handle what to do when the button is pressed (for now, just a debug pop-up message)
    QMessageBox::information(this, "DOWNLOAD", "DOWNLOAD");
}

void EnvironmentWindow::on_optionsButton_clicked()
{
    OptionsDialog* dialog = new OptionsDialog(this);

    connect(dialog, &OptionsDialog::textSizeChanged, this, &EnvironmentWindow::updateEditorFontSize);
    connect(dialog, &OptionsDialog::syntaxHighlightingToggled, this, &EnvironmentWindow::toggleSyntaxHighlighter);
    connect(dialog, &OptionsDialog::executionTimeToggled, this, &EnvironmentWindow::toggleExecutionTime);
    connect(dialog, &OptionsDialog::historyCleanupToggled, this, &EnvironmentWindow::toggleHistoryCleanup);
    connect(dialog, &OptionsDialog::deleteCurrentDatabaseRequested, this, &EnvironmentWindow::deleteCurrentDatabase);

    dialog->setSyntaxHighlightingEnabled(syntaxHighlightingEnabled);
    dialog->setExecutionTimeEnabled(executionTimeEnabled);
    dialog->setFontSize(fontSize);

    dialog->exec();
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
    connect(dialog, &DatabaseSelect::databaseSelected, this, &EnvironmentWindow::onDatabaseChosen);

   
}





void EnvironmentWindow::on_logoutButton_clicked()
{
    emit logoutRequested();
}


////---> apasarea butonului run si rularea codului curent scris in editorul text
//void EnvironmentWindow::on_runButton_clicked()
//{
//    QString userCode = ui->EditorText->toPlainText();
//
//    if (userCode.isEmpty()) {
//        QMessageBox::warning(this, "Empty Code", "Please write something to run.");
//        return;
//    }
//
//    try {
//        Socket socket(Socket::Protocol::TCP);
//        if (!socket.connectToServer("127.0.0.1", 12345)) {
//            QMessageBox::critical(this, "Error", "Cannot connect to server.");
//            return;
//        }
//
//        // Serializez: comanda + utilizator + baza de date + cod   -> selectedDatabase este baza de date selectata din meniul de selectie al bazei de date
//       // std::string request = "EXECUTE_CODE:" + currentUsername.toStdString() + ":" + selectedDatabase.toStdString() + ":" + userCode.toStdString();
//        
//       // socket.sendData(request);
//
//        std::string response = socket.receiveData(4096); // răspunsul de la server
//        QMessageBox::information(this, "Server Response", QString::fromStdString(response));
//    }
//    catch (const std::exception& e) {
//        QMessageBox::critical(this, "Error", e.what());
//    }
//}





EnvironmentWindow::~EnvironmentWindow()
{
    delete ui;
}


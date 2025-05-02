#include "EnvironmentWindow.h"
#include "ui_EnvironmentWindow.h"
#include "OptionsDialog.h"
#include "AuthenticationWindow.h"
#include "DatabaseSelect.h"
#include "SocketLib.h"
#include <QMessageBox>
#include <qinputdialog.h>

EnvironmentWindow::EnvironmentWindow(const QString& username, QWidget* parent) : QMainWindow(parent), ui(new Ui::EnvironmentWindow)
{
    ui->setupUi(this);
    currentUsername = username;
    
    updateUsernameLabel();
    setupPanelSwitching();
}

void EnvironmentWindow::updateEditorFontSize(int size)
{
    fontSize = size;

    QFont font = ui->EditorText->font();
    font.setPointSize(size);
    ui->EditorText->setFont(font);
}

void EnvironmentWindow::toggleSyntaxHighlighter(bool enabled)
{
    syntaxHighlightingEnabled = enabled;

    if (enabled) {
        enableSyntaxHighlighting();
    }
    else {
        disableSyntaxHighlighting();

        QTextCursor cursor(ui->EditorText->document());
        cursor.select(QTextCursor::Document);

        QTextCharFormat format;
        format.setForeground(Qt::black);
        cursor.mergeCharFormat(format);
    }
}

void EnvironmentWindow::toggleExecutionTime(bool enabled)
{
    // You can handle what to do when execution time is toggled
    executionTimeEnabled = enabled;
}

void EnvironmentWindow::toggleHistoryCleanup(bool enabled)
{
    if (enabled) {
        commandHistoryBuffer.clear();
        QMessageBox::information(this, "HISTORY CLEANUP", "Cleaning up history...");
        OptionsDialog* dialog = qobject_cast<OptionsDialog*>(sender());

        if (dialog) {
            dialog->uncheckHistoryCleanup();
        }
    }
}

void EnvironmentWindow::deleteCurrentDatabase()
{
    if (selectedDatabase.isEmpty()) {
        QMessageBox::warning(this, "Delete", "No database selected.");
        return;
    }

    try {
        Socket socket(Socket::Protocol::TCP);
        if (!socket.connectToServer("127.0.0.1", 12345)) {
            QMessageBox::critical(this, "Connection Error", "Could not connect to server.");
            return;
        }

        std::string request = "DELETE_DATABASE:" + currentUsername.toStdString() + ":" + selectedDatabase.toStdString();
        socket.sendData(request);
        std::string response = socket.receiveData(1024);

        if (response == "DELETE_DB_SUCCESS") {
            QMessageBox::information(this, "Success", "Database deleted successfully.");
            selectedDatabase.clear();
            updateUsernameLabel();
        }
        else {
            QMessageBox::warning(this, "Error", QString::fromStdString(response));
        }
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Socket Error", e.what());
    }
}

void EnvironmentWindow::on_currentDatabaseButton_clicked()
{
    QStringList userDatabases;

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

    DatabaseSelect* dialog = new DatabaseSelect(currentUsername, this);

    if (userDatabases.isEmpty()) {
        dialog->getNoDatabaseLabel()->setVisible(true);
        dialog->getDatabaseListWidget()->setVisible(false);
    }
    else {
        dialog->getNoDatabaseLabel()->setVisible(false);
        dialog->getDatabaseListWidget()->setVisible(true);
        for (const QString& db : userDatabases) {
            dialog->getDatabaseListWidget()->addItem(db);
        }
    }

    QObject::connect(dialog, SIGNAL(databaseSelected(QString)), this, SLOT(onDatabaseChosen(QString)));

    dialog->exec();
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

//---> apasarea butonului run si rularea codului curent scris in editorul text
void EnvironmentWindow::on_runButton_clicked()
{
    QString currentPanel = getCurrentPanel();

    if (currentPanel != "Query") {
        QMessageBox::warning(this, "Run Blocked", "Switch to 'Query Editor' to run queries.");
        return;
    }

    QString userCode = ui->EditorText->toPlainText();

    if (userCode.isEmpty()) {
        QMessageBox::warning(this, "Empty Code", "Please write something to run.");
        return;
    }
    else {
        if (commandHistoryBuffer.isEmpty() || commandHistoryBuffer.last() != userCode) {
            commandHistoryBuffer.append(userCode);
        }
    }

    try {
        Socket socket(Socket::Protocol::TCP);
        if (!socket.connectToServer("127.0.0.1", 12345)) {
            QMessageBox::critical(this, "Error", "Cannot connect to server.");
            return;
        }

        // Serializez: comanda + utilizator + baza de date + cod   -> selectedDatabase este baza de date selectata din meniul de selectie al bazei de date
        std::string request = "EXECUTE_CODE:" + currentUsername.toStdString() + ":" + selectedDatabase.toStdString() + ":" + userCode.toStdString();

        socket.sendData(request);  // trimit cererea la server

        std::string response = socket.receiveData(4096); // răspunsul de la server
        QMessageBox::information(this, "Server Response", QString::fromStdString(response));
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Socket Error", e.what());
    }
}

void EnvironmentWindow::on_importButton_clicked()
{
    if (getCurrentPanel() != "Query") {
        QMessageBox::warning(this, "Import", "You can only import queries inside the Query panel.");
        return;
    }

    try {

        Socket listSocket(Socket::Protocol::TCP);
        if (!listSocket.connectToServer("127.0.0.1", 12345)) {
            QMessageBox::critical(this, "Connection Error", "Could not connect to server.");
            return;
        }

        std::string listRequest = "LIST_QUERIES:" + currentUsername.toStdString();
        listSocket.sendData(listRequest);
        std::string listResponse = listSocket.receiveData(4096);

        QString qListResponse = QString::fromStdString(listResponse).trimmed();

        if (!qListResponse.startsWith("QUERYLIST:")) {
            QMessageBox::warning(this, "Import Error", "Unexpected server response.");
            return;
        }

        QStringList queryNames = qListResponse.mid(10).split("|||");

        if (queryNames.isEmpty() || (queryNames.size() == 1 && queryNames[0].isEmpty())) {
            QMessageBox::information(this, "IMPORT", "No saved queries available.");
            return;
        }

        bool ok;
        QString selectedQuery = QInputDialog::getItem(this, "IMPORT", "Choose a query:", queryNames, 0, false, &ok);

        if (ok == false || selectedQuery.isEmpty()) {
            return;
        }

        Socket loadSocket(Socket::Protocol::TCP);
        if (!loadSocket.connectToServer("127.0.0.1", 12345)) {
            QMessageBox::critical(this, "Connection Error", "Could not connect to server (LOAD_QUERY).");
            return;
        }

        std::string loadRequest = "LOAD_QUERY:" + currentUsername.toStdString() + ":" + selectedQuery.toStdString();
        loadSocket.sendData(loadRequest);
        std::string loadResponse = loadSocket.receiveData(8192);

        QString qLoadResponse = QString::fromStdString(loadResponse).trimmed();

        if (qLoadResponse == "LOAD_FAIL") {
            QMessageBox::warning(this, "Load Error", "Query file not found on server.");
            return;
        }
        else if (!qLoadResponse.startsWith("QUERYDATA:")) {
            QMessageBox::warning(this, "Load Error", "Unexpected server response (LOAD_QUERY).");
            return;
        }

        QString queryContent = qLoadResponse.mid(10);
        ui->EditorText->setPlainText(queryContent);

        QMessageBox::information(this, "IMPORT", "Query imported successfully.");
    }

    catch (const std::exception& e) {
        QMessageBox::critical(this, "Socket Error", e.what());
    }
}

void EnvironmentWindow::on_logButton_clicked()
{
    if (logDisplayActive) {
        ui->EditorText->setPlainText(logBackup);
        ui->EditorText->setReadOnly(false);
        enableSyntaxHighlighting();
        logDisplayActive = false;
        return;
    }

    try {
        Socket socket(Socket::Protocol::TCP);
        if (!socket.connectToServer("127.0.0.1", 12345)) {
            QMessageBox::critical(this, "Connection Error", "Could not connect to server.");
            return;
        }

        std::string request = "GET_LOGS:" + currentUsername.toStdString();
        socket.sendData(request);

        std::string response, chunk;
        int count = 0;
        do {
            chunk = socket.receiveData(1024);
            response += chunk;
            count++;
        } while (response.find("END_OF_LOGS") == std::string::npos && count < 100);

        if (!response._Starts_with("LOGDATA:")) {
            QMessageBox::warning(this, "Log Error", "Unexpected server response.");
            return;
        }

        response = response.substr(0, response.find("END_OF_LOGS"));
        QStringList logLines = QString::fromStdString(response).mid(8).split("|||");

        logBackup = ui->EditorText->toPlainText();
        disableSyntaxHighlighting();
        ui->EditorText->setPlainText(logLines.join("\n"));
        ui->EditorText->setReadOnly(true);
        logDisplayActive = true;
    }

    catch (const std::exception& e) {
        QMessageBox::critical(this, "Socket Error", e.what());
    }
}

void EnvironmentWindow::on_downloadButton_clicked()
{
    if (getCurrentPanel() != "Query") {
        QMessageBox::warning(this, "Download", "You can only save queries inside the Query panel.");
        return;
    }

    bool ok;
    QString queryName = QInputDialog::getText(this, "Save Query", "Enter query name:", QLineEdit::Normal, "", &ok);

    if (ok == false || queryName.isEmpty()) {
        return;
    }

    try {
        Socket socket(Socket::Protocol::TCP);
        if (!socket.connectToServer("127.0.0.1", 12345)) {
            QMessageBox::critical(this, "Connection Error", "Could not connect to server.");
            return;
        }

        std::string request = "SAVE_QUERY:" + currentUsername.toStdString() + ":" + queryName.toStdString() + ":";
        request += ui->EditorText->toPlainText().toStdString();
        socket.sendData(request);

        std::string response = socket.receiveData(1024);
        if (response == "SAVE_SUCCESS") {
            QMessageBox::information(this, "Download", "Query saved on server.");
        }
        else {
            QMessageBox::warning(this, "Save Failed", QString::fromStdString(response));
        }
    }

    catch (const std::exception& e) {
        QMessageBox::critical(this, "Socket Error", e.what());
    }
}

//acesta este butonul pentru a deschide fereastra de vizualizare a tabelelor
//void EnvironmentWindow::on_tabesButton_clicked()
//{
//    if (selectedDatabase.isEmpty()) {
//        QMessageBox::warning(this, "No DB", "Select a database first.");
//        return;
//    }
//
//    TablesViewerWindow* viewer = new TablesViewerWindow(currentUsername, selectedDatabase, this);
//    viewer->exec();
//}

void EnvironmentWindow::on_logoutButton_clicked()
{
    emit logoutRequested();
}

void EnvironmentWindow::onDatabaseChosen(const QString& databaseName)
{
    QMessageBox::information(this, "Database Select", "Selected database: " + databaseName); 
    
    selectedDatabase = databaseName;
    updateUsernameLabel();

	//aici furnizez instructiunea de incarcare a bazei de date pe server
    Socket socket(Socket::Protocol::TCP);
    if (socket.connectToServer("127.0.0.1", 12345)) {
        std::string request = "LOAD_DATABASE:" + currentUsername.toStdString() + ":" + selectedDatabase.toStdString();
        socket.sendData(request);
        std::string response = socket.receiveData(1024);
        if (response != "LOAD_DB_SUCCESS") {
            QMessageBox::warning(this, "Error", "Could not load database on server.");
        }
    }
}



void EnvironmentWindow::updateUsernameLabel()
{
    QString usernameLabel = currentUsername + " - MANAGER";

    if (!selectedDatabase.isEmpty()) {
        usernameLabel += "\n" + selectedDatabase;
    }

    ui->labelUsername->setText(usernameLabel);
}

EnvironmentWindow::~EnvironmentWindow()
{
    delete ui;
}

void EnvironmentWindow::enableSyntaxHighlighting()
{
    if (highlighter == nullptr) {
        highlighter = new SyntaxHighlighter(ui->EditorText->document());
        highlighter->rehighlight();
    }
}

void EnvironmentWindow::disableSyntaxHighlighting()
{
    if (highlighter != nullptr) {
        delete highlighter;
        highlighter = nullptr;
    }
}

void EnvironmentWindow::setupPanelSwitching()
{
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, [this](int row) {
     
        QString selectedItem = getPanelAtRow(row);

        if (selectedItem == "Query") {
            switchToQuery();
        }
        else if (selectedItem == "Command History") {
            switchToCommandHistory();
        }

		//afisarea tuturor tabelelor bazei de date selectate
        else  if (selectedItem == "Tables") {
            displayTables();  // definim mai jos
        }
        else {
            switchToOtherPanel();
        }
     });

    setDefaultWidgetRow("Query");
}

void EnvironmentWindow::displayTables() {
    if (selectedDatabase.isEmpty()) {
        QMessageBox::warning(this, "No Database", "Please select a database first.");
        return;
    }

    try {
        Socket socket(Socket::Protocol::TCP);
        if (!socket.connectToServer("127.0.0.1", 12345)) {
            QMessageBox::critical(this, "Connection Error", "Could not connect to server.");
            return;
        }

        std::string request = "GET_TABLES:" + currentUsername.toStdString() + ":" + selectedDatabase.toStdString();
        socket.sendData(request);

        std::string response = socket.receiveData(8192);
        const std::string prefix = "TABLEDUMP:";
        if (response.rfind(prefix, 0) == 0) { // începe cu "TABLEDUMP:"
            std::string payload = response.substr(prefix.length());
            QString qPayload = QString::fromStdString(payload);
            ui->EditorText->setPlainText(qPayload);
        }
        else {
            QMessageBox::warning(this, "Error", "Unexpected server response.");
        }
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

QString EnvironmentWindow::getCurrentPanel() const
{
    QListWidgetItem* currentItem = ui->listWidget->currentItem();
    
    if (currentItem) {
        return currentItem->text();
    }

    return QString(); 
}

QString EnvironmentWindow::getPanelAtRow(int row) const
{
    if (row < 0 || row >= ui->listWidget->count()) {
        return QString(); 
    }

    return ui->listWidget->item(row)->text();
}

void EnvironmentWindow::setDefaultWidgetRow(const std::string& name)
{
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        if (ui->listWidget->item(i)->text() == name) {
            ui->listWidget->setCurrentRow(i);
            break;
        }
    }
}

void EnvironmentWindow::switchToQuery()
{
    ui->EditorText->setPlainText(editorQueryContent);
    ui->EditorText->setReadOnly(false);

    if (syntaxHighlightingEnabled) {
        enableSyntaxHighlighting();
    }
}

void EnvironmentWindow::switchToCommandHistory()
{
    editorQueryContent = ui->EditorText->toPlainText();

    QString combinedHistory = commandHistoryBuffer.join("\n\n");
    ui->EditorText->setPlainText(combinedHistory);
    ui->EditorText->setReadOnly(true);
}

void EnvironmentWindow::switchToOtherPanel()
{
    if (!ui->EditorText->isReadOnly()) {
        editorQueryContent = ui->EditorText->toPlainText();
    }

    ui->EditorText->clear();
    ui->EditorText->setReadOnly(true);

    if (highlighter) {
        delete highlighter;
        highlighter = nullptr;
    }
}


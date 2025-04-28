#pragma once
#include <QMainWindow>
#include "SyntaxHighlighter.h"

QT_BEGIN_NAMESPACE
namespace Ui { 
    class EnvironmentWindow; 
}
QT_END_NAMESPACE

class EnvironmentWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EnvironmentWindow(QWidget* parent = nullptr);
    explicit EnvironmentWindow(const QString& username, QWidget* parent = nullptr);
    ~EnvironmentWindow();
 
signals:
    void logoutRequested();

private slots:
    void updateEditorFontSize(int size);
    void toggleSyntaxHighlighter(bool enabled);
    void toggleExecutionTime(bool enabled);
    void toggleHistoryCleanup(bool enabled);
    void deleteCurrentDatabase();

    void on_runButton_clicked();
    void on_importButton_clicked();
    void on_logButton_clicked();
    void on_downloadButton_clicked();

    void on_optionsButton_clicked();
    void on_currentDatabaseButton_clicked();
    void on_logoutButton_clicked();

    void onDatabaseChosen(const QString& databaseName);
	void updateUsernameLabel();

private:
    QString currentUsername;
    QString selectedDatabase;           //numele baze de date curente (obtinuta prin dublu ckick pe lista de baze de date afisata in meniul CurrentDatabase)
    Ui::EnvironmentWindow* ui;
    QStringList userDatabases;                                      // definesc bazele de date ale utilizatorului
    SyntaxHighlighter* highlighter = nullptr;

    // Settings storage:
    int fontSize = 12;
    bool syntaxHighlightingEnabled = false;
    bool executionTimeEnabled = false;
};



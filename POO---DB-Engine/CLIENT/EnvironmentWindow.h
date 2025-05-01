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
    void enableSyntaxHighlighting();
    void disableSyntaxHighlighting();
    void setupPanelSwitching();
    QString getCurrentPanel() const;
    QString getPanelAtRow(int row) const;
    void setDefaultWidgetRow(const std::string& name);
    void switchToQuery();
    void switchToCommandHistory();
    void switchToOtherPanel();

private:
    QString currentUsername;
    QString selectedDatabase;                            //numele bazei de date curente 
    Ui::EnvironmentWindow* ui;
    QStringList userDatabases;                           // definesc bazele de date ale utilizatorului
    QStringList commandHistoryBuffer;                    // stores past commands
    QString editorBackup;                               // used when switching in/out of Command History
    QString editorQueryContent;
    SyntaxHighlighter* highlighter = nullptr;

    QString logBackup;                                  // stores the content of the query editor before showing the log, so you can restore it later.
    bool logDisplayActive = false;                      // A flag to track whether logs are currently shown in the editor.

    // Settings storage:
    int fontSize = 12;
    bool syntaxHighlightingEnabled = false;
    bool executionTimeEnabled = false;
};



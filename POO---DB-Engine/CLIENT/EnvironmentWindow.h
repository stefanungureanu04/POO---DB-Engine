#pragma once
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class EnvironmentWindow; }
QT_END_NAMESPACE

class EnvironmentWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EnvironmentWindow(QWidget* parent = nullptr);
    explicit EnvironmentWindow(const QString& username, QWidget* parent = nullptr);
    void setCurrentUsername(const QString& username);
    ~EnvironmentWindow();
   

private slots:
    void on_optionsButton_clicked();
    void updateEditorFontSize(int size);
    void on_currentDatabaseButton_clicked();

private:
    QString currentUsername;
    Ui::EnvironmentWindow* ui;
    QStringList userDatabases;              // definesc bazele de date ale utilizatorului


};



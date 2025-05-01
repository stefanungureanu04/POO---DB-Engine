#pragma once
#include <QDialog>
#include <QStringList>
#include <QLabel>
#include <QListWidget>

namespace Ui {
    class DatabaseSelect;
}

class DatabaseSelect : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseSelect(const QString& username, QWidget* parent = nullptr);
    ~DatabaseSelect();

    void setDatabaseList(const QStringList& databases);
    QLabel* getNoDatabaseLabel();
    QListWidget* getDatabaseListWidget();

signals:
    void databaseSelected(const QString& databaseName);

public slots:
    void refreshDatabaseList();

private slots:
    void onDatabaseItemDoubleClicked(QListWidgetItem* item);
    void on_createNewDatabaseButton_clicked();

private:
    Ui::DatabaseSelect* ui;
    QString currentUsername;
};

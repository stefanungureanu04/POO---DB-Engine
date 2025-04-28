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
    explicit DatabaseSelect(QWidget* parent = nullptr);
    ~DatabaseSelect();

    void setDatabaseList(const QStringList& databases);
    QLabel* getNoDatabaseLabel();
    QListWidget* getDatabaseListWidget();

signals:
    void databaseSelected(const QString& databaseName);

private slots:
    void onDatabaseItemDoubleClicked(QListWidgetItem* item);

private:
    Ui::DatabaseSelect* ui;


};

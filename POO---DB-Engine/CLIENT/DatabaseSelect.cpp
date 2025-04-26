#include "DatabaseSelect.h"
#include "ui_DatabaseSelect.h"

DatabaseSelect::DatabaseSelect(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DatabaseSelect)
{
    ui->setupUi(this);
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

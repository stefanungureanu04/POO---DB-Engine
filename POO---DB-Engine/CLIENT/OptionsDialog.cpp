#include "OptionsDialog.h"
#include "ui_OptionsDialog.h"
#include <qmessagebox.h>

OptionsDialog::OptionsDialog(QWidget* parent) : QDialog(parent), ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);

    updateUI();
}

void OptionsDialog::updateUI()
{
    ui->slider->setValue(fontSize);
    ui->executionTimeCheckBox->setChecked(executionTime);
    ui->syntaxHighlightingCheckBox->setChecked(syntaxHighlighting);
    ui->historyCleanupCheckBox->setChecked(historyCleanup);
}

void OptionsDialog::setFontSize(int size)
{
    this->fontSize = size;

    if (ui != nullptr) {
        ui->slider->setValue(size);
    }
}

void OptionsDialog::setExecutionTimeEnabled(bool enabled)
{
    this->executionTime = enabled;

    if (ui != nullptr) {
        ui->executionTimeCheckBox->setChecked(enabled);
    }
}

void OptionsDialog::setSyntaxHighlightingEnabled(bool enabled)
{
    this->syntaxHighlighting = enabled;

    if (ui != nullptr) {
        ui->syntaxHighlightingCheckBox->setChecked(enabled);
    }
}

void OptionsDialog::uncheckHistoryCleanup()
{
    if (ui != nullptr) {
        ui->historyCleanupCheckBox->setChecked(false);
    }
}

void OptionsDialog::on_slider_valueChanged(int newSize) 
{
    ui->label3->setText(QString::number(newSize));

    emit textSizeChanged(newSize);                              
}

void OptionsDialog::on_syntaxHighlightingCheckBox_stateChanged(int state)
{
    syntaxHighlighting = (state == Qt::Checked);

    emit syntaxHighlightingToggled(syntaxHighlighting);
}

void OptionsDialog::on_executionTimeCheckBox_stateChanged(int state)
{
    emit executionTimeToggled(state == Qt::Checked);
}

void OptionsDialog::on_historyCleanupCheckBox_stateChanged(int state)
{
    emit historyCleanupToggled(state == Qt::Checked);
}

void OptionsDialog::on_deleteCurrentDatabaseButton_clicked()
{
    QMessageBox::StandardButton reply;

    reply = QMessageBox::question(this, "CONFIRM", "Are you sure you want to delete the current database?", QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        emit deleteCurrentDatabaseRequested();
    }
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}
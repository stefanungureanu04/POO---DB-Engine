#include "OptionsDialog.h"
#include "ui_OptionsDialog.h"

OptionsDialog::OptionsDialog(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::OptionsDialog)
{

    ui->setupUi(this);
}

void OptionsDialog::on_slider_valueChanged(int value) {
    ui->label3->setText(QString::number(value));
    emit textSizeChanged(value); // trimite valoarea oricui e conectat
}




OptionsDialog::~OptionsDialog()
{
    delete ui;
}
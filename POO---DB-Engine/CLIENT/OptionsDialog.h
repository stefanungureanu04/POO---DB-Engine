#pragma once

#include <QDialog>

namespace Ui {
    class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget* parent = nullptr);
    ~OptionsDialog();

private slots:
    void on_slider_valueChanged(int value);
signals:
    void textSizeChanged(int newSize);

private:
    Ui::OptionsDialog* ui;
};

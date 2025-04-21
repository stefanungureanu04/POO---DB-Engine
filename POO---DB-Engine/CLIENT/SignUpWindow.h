#pragma once

#include <QDialog>

namespace Ui {
    class SignUpWindow;
}

class SignUpWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SignUpWindow(QWidget* parent = nullptr);
    ~SignUpWindow();

private slots:
    void on_registerButton_clicked();

private:
    Ui::SignUpWindow* ui;
};



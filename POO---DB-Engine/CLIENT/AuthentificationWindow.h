#ifndef AUTHENTIFICATIONWINDOW_H
#define AUTHENTIFICATIONWINDOW_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class AuthentificationWindow; }
QT_END_NAMESPACE

class AuthentificationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AuthentificationWindow(QWidget* parent = nullptr);
    ~AuthentificationWindow();

private slots:
    void on_signupButton_clicked();
    void on_loginButton_clicked();

private:
    Ui::AuthentificationWindow* ui;
};

#endif // AUTHENTIFICATIONWINDOW_H

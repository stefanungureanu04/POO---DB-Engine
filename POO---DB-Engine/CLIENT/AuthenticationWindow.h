#ifndef AUTHENTICATIONWINDOW_H
#define AUTHENTICATIONWINDOW_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class AuthenticationWindow; }
QT_END_NAMESPACE

class AuthenticationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AuthenticationWindow(QWidget* parent = nullptr);
    ~AuthenticationWindow();

signals:
    void loginSuccess(const QString& username);

private slots:
    void on_signupButton_clicked();
    void on_loginButton_clicked();

private:
    Ui::AuthenticationWindow* ui;
};

#endif

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
    ~EnvironmentWindow();


private slots:
    void on_optionsButton_clicked();
private slots:
    void updateEditorFontSize(int size);

private:
    QString currentUsername;
    Ui::EnvironmentWindow* ui;

};



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
    ~EnvironmentWindow();

private:
    Ui::EnvironmentWindow* ui;
};



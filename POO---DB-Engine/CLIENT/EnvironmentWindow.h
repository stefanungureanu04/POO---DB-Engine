#pragma once
#include <QMainWindow>

class EnvironmentWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EnvironmentWindow(QWidget* parent = nullptr);
    ~EnvironmentWindow();
};

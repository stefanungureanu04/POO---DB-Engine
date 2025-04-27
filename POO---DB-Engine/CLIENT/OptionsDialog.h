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

    void setFontSize(int size);
    void setExecutionTimeEnabled(bool enabled);
    void setSyntaxHighlightingEnabled(bool enabled);
    void uncheckHistoryCleanup();

signals:
    void textSizeChanged(int newSize);
    void syntaxHighlightingToggled(bool enabled);
    void executionTimeToggled(bool enabled);
    void historyCleanupToggled(bool enabled);
    void deleteCurrentDatabaseRequested();

private slots:
    void on_slider_valueChanged(int value);
    void on_syntaxHighlightingCheckBox_stateChanged(int state);
    void on_executionTimeCheckBox_stateChanged(int state);
    void on_historyCleanupCheckBox_stateChanged(int state);
    void on_deleteCurrentDatabaseButton_clicked();

private:
    int fontSize = 12;
    bool executionTime = false;
    bool syntaxHighlighting = false;
    bool historyCleanup = false;

    void updateUI(); 

    Ui::OptionsDialog* ui;
};

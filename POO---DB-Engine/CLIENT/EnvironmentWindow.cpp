#include "EnvironmentWindow.h"
#include "ui_EnvironmentWindow.h"
#include "OptionsDialog.h"
#include "AuthenticationWindow.h"



EnvironmentWindow::EnvironmentWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::EnvironmentWindow)
{
    ui->setupUi(this);
    // other initialization here
}

EnvironmentWindow::EnvironmentWindow(const QString& username, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::EnvironmentWindow)
{
    ui->setupUi(this);
    ui->labelUsername->setText("-> " + username + " <- MANAGER");  // assuming you have a QLabel named 'label'
}

void EnvironmentWindow::on_optionsButton_clicked()
{

    OptionsDialog* dialog = new OptionsDialog(this);

    connect(dialog, &OptionsDialog::textSizeChanged,
        this, &EnvironmentWindow::updateEditorFontSize);


    dialog->exec();
}

void EnvironmentWindow::updateEditorFontSize(int size) {
    QFont font = ui->EditorText->font();  // replace `EditorText` with your actual QPlainTextEdit name
    font.setPointSize(size);
    ui->EditorText->setFont(font);
}

EnvironmentWindow::~EnvironmentWindow()
{
    delete ui;
}


#include "popupwindow.h"
#include "ui_popupwindow.h"

PopUpWindow::PopUpWindow(QWidget *parent, QString message)
    : QDialog(parent)
    , ui(new Ui::PopUpWindow)
{
    ui->setupUi(this);
    auto errorBox = findChild<QLabel *>("errorMessage");
    errorBox->setText(message);
}
PopUpWindow::~PopUpWindow()
{
    delete ui;
}

void PopUpWindow::on_pushButton_clicked()
{
    close();
}

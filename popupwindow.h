#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QDialog>

namespace Ui {
class PopUpWindow;
}

class PopUpWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PopUpWindow(QWidget *parent = nullptr, QString message = "");
    ~PopUpWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::PopUpWindow *ui;
};

#endif // POPUPWINDOW_H

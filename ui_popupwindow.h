/********************************************************************************
** Form generated from reading UI file 'popupwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POPUPWINDOW_H
#define UI_POPUPWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_PopUpWindow
{
public:
    QPushButton *pushButton;
    QLabel *errorMessage;

    void setupUi(QDialog *PopUpWindow)
    {
        if (PopUpWindow->objectName().isEmpty())
            PopUpWindow->setObjectName(QString::fromUtf8("PopUpWindow"));
        PopUpWindow->resize(513, 173);
        pushButton = new QPushButton(PopUpWindow);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(210, 130, 91, 31));
        errorMessage = new QLabel(PopUpWindow);
        errorMessage->setObjectName(QString::fromUtf8("errorMessage"));
        errorMessage->setGeometry(QRect(40, 40, 431, 61));
        QFont font;
        font.setFamily(QString::fromUtf8("MS Shell Dlg 2"));
        font.setPointSize(12);
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        errorMessage->setFont(font);
        errorMessage->setStyleSheet(QString::fromUtf8(""));
        errorMessage->setTextFormat(Qt::AutoText);
        errorMessage->setAlignment(Qt::AlignCenter);
        errorMessage->setWordWrap(true);

        retranslateUi(PopUpWindow);

        QMetaObject::connectSlotsByName(PopUpWindow);
    } // setupUi

    void retranslateUi(QDialog *PopUpWindow)
    {
        PopUpWindow->setWindowTitle(QCoreApplication::translate("PopUpWindow", "Error", nullptr));
        pushButton->setText(QCoreApplication::translate("PopUpWindow", "OK", nullptr));
        errorMessage->setText(QCoreApplication::translate("PopUpWindow", "TextLabel", nullptr));
    } // retranslateUi
};

namespace Ui {
class PopUpWindow : public Ui_PopUpWindow
{};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POPUPWINDOW_H

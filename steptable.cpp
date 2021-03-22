#include "steptable.h"

#include <QGridLayout>
#include <QHeaderView>
#include <QDebug>



StepTable::StepTable(QWidget *parent) : QWidget(parent)
{
    m_table = new QTableWidget(12, 2, this);

    m_table->setHorizontalHeaderLabels(QStringList() << "Left" << "Right");

    styleHeader();

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(m_table, 0, 0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
}

void StepTable::insertRow(int row)
{
    qDebug() << "inserting row";
    m_table->insertRow(row);
    auto tmp = new QTableWidgetItem(QString::number(row));
    m_table->setItem(0, 1, std::move(tmp));
}

void StepTable::styleHeader()
{
    // Work-around to style header:
    // https://stackoverflow.com/questions/38554640/add-border-under-column-headers-in-qtablewidget
    m_table->horizontalHeader()->setStyleSheet( "QHeaderView::section{"
            "border-top:0px solid #D8D8D8;"
            "border-left:0px solid #D8D8D8;"
            "border-right:1px solid #D8D8D8;"
            "border-bottom: 1px solid #D8D8D8;"
            "background-color:white;"
            "padding:4px;"
        "}"
        "QTableCornerButton::section{"
            "border-top:0px solid #D8D8D8;"
            "border-left:0px solid #D8D8D8;"
            "border-right:1px solid #D8D8D8;"
            "border-bottom: 1px solid #D8D8D8;"
            "background-color:white;"
        "}" );
}

#include "filetable.h"

#include <QGridLayout>
#include <QHeaderView>
#include <QDebug>

FileTable::FileTable(QWidget *parent) : QWidget(parent)
{
    m_table = new QTableWidget(1, static_cast<qint64>(FileTableRowName::COUNT), this);
    m_table->setHorizontalHeaderLabels(QStringList() << "File Name" << "Have Exported Steps?"); // TODO: use FileTableRowName enum to assign header labels

    // Fix the style of the header to be consistent with rest of the table
    styleHeader();
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch );

    // Set layout for this widget
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(m_table, 0, 0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);

}

void FileTable::styleHeader()
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

#include "filetable.h"

#include <QGridLayout>
#include <QHeaderView>
#include <QDebug>
#include <QDir>

FileTable::FileTable(QWidget *parent) : QWidget(parent), m_lastOccupiedPosition{}
{
    m_table = new QTableWidget(1, static_cast<qint64>(FileTableRowName::COUNT), this);
    m_table->setHorizontalHeaderLabels(QStringList() << "File Name" << "Have Exported Steps?"); // TODO: use FileTableRowName enum to assign header labels

    // Fix the style of the header to be consistent with rest of the table
    styleHeader();
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch );
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Set layout for this widget
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(m_table, 0, 0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
    connect(m_table, &QTableWidget::itemDoubleClicked, this, &FileTable::handleItemDoubleClicked);

}

void FileTable::fillTableWithFiles(QFileInfoList files, QString footfallFolder, QDir videoFolder, QString stepFormat)
{
    // Remove all contents before trying to add
    m_lastOccupiedPosition = 0;
    m_table->setRowCount(0);

    m_rootFolder = videoFolder;
    m_stepFormat = stepFormat;
    // Add new items
    auto columnToInsertAt = static_cast<qint16>(FileTableRowName::FileName);

    for (auto &file : files) {
         auto curFileName = file.fileName();
         auto new_item = new QTableWidgetItem(curFileName);

         if (m_lastOccupiedPosition == m_table->rowCount())
             m_table->insertRow(m_lastOccupiedPosition);

         m_table->setItem(m_lastOccupiedPosition, columnToInsertAt, std::move(new_item));

         // Update label status
         setLabelStatus(m_lastOccupiedPosition, footfallFolder, stepFormat);
         m_lastOccupiedPosition++;
    }
}

void FileTable::handleItemDoubleClicked(QTableWidgetItem *item)
{
    // If a video name was selected, get the full file name and
    // emit signal to play the selected one.
    auto fileColumn = static_cast<qint16>(FileTableRowName::FileName);
    if (item->column() != fileColumn)
        return;

    qDebug() << item->column() <<  fileColumn <<"handleItemDoubleClicked";
    QString localPath = item->data(Qt::EditRole).toString();
    QString videoName = m_rootFolder.filePath(localPath);
    emit playVideoByName(videoName);
}

void FileTable::setLabelStatus(qint64 rowToInsertAt, QString footfallFolder, QString stepFormat)
{
    auto testIcon = this->style()->standardIcon(QStyle::SP_DialogCancelButton);

    auto fileCol = static_cast<qint16>(FileTableRowName::FileName);
    auto statusCol = static_cast<qint16>(FileTableRowName::StepStatus);

    auto curFileName = m_table->item(rowToInsertAt, fileCol)->data(Qt::EditRole);
    auto footfallFileName = QDir(footfallFolder).filePath(curFileName.toString() + stepFormat);


    if (QFile::exists(footfallFileName))
        testIcon = this->style()->standardIcon(QStyle::SP_DialogApplyButton);

    auto new_item = new QTableWidgetItem(testIcon, "");
    m_table->setItem(rowToInsertAt, statusCol, std::move(new_item));

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

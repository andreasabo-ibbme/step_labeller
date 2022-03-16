#include "filetable.h"

#include <algorithm>
#include <QDebug>
#include <QDir>
#include <QGridLayout>
#include <QHeaderView>

FileTable::FileTable(QWidget *parent, const QString stepFormat)
    : QWidget(parent)
    , m_lastOccupiedPosition{}
    , m_acceptableFormats{"avi", "mov", "mp4"}
    , m_stepFormat{stepFormat}
{
    m_table = new QTableWidget(1, static_cast<qint64>(FileTableRowName::COUNT), this);
    m_table->setHorizontalHeaderLabels(QStringList() << "File Name"
                                                     << "Have Exported Steps?");

    // Fix the style of the header to be consistent with rest of the table
    styleHeader();
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Set layout for this widget
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(m_table, 0, 0);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    connect(m_table, &QTableWidget::itemDoubleClicked, this, &FileTable::handleItemDoubleClicked);
}

bool FileTable::isValidVideo(const QString &file)
{
    auto suffix = QFileInfo(file).suffix().toLower();
    if (std::find(m_acceptableFormats.begin(), m_acceptableFormats.end(), suffix)
        == m_acceptableFormats.end()) {
        return false;
    }
    return true;
}

void FileTable::fillTableWithFiles(QFileInfoList files, QString footfallFolder, QDir videoFolder)
{
    // Remove all contents before trying to add
    m_lastOccupiedPosition = 0;
    m_table->setRowCount(0);
    m_footfallFolder = footfallFolder;
    m_rootFolder = videoFolder;

    // Add new items
    auto columnToInsertAt = static_cast<qint64>(FileTableRowName::FileName);

    for (auto &file : files) {
        auto curFileName = file.fileName();

        // Only add allowed videos
        if (!isValidVideo(curFileName)) {
            continue;
        }

        auto new_item = new QTableWidgetItem(curFileName);

        if (m_lastOccupiedPosition == m_table->rowCount())
            m_table->insertRow(m_lastOccupiedPosition);

        m_table->setItem(m_lastOccupiedPosition, columnToInsertAt, std::move(new_item));

        // Update label status
        setLabelStatus(m_lastOccupiedPosition);
        m_lastOccupiedPosition++;
    }
}

void FileTable::playFirstVideo()
{
    if (m_lastOccupiedPosition < 1) {
        emit failedToPlayVideo("The selected folder does not contain any playable videos");
    } else {
        playVideoFromTable(m_table->itemAt(0, 0));
    }
}

void FileTable::updateFileLabelStatus()
{
    for (auto i = 0; i < m_lastOccupiedPosition; ++i) {
        setLabelStatus(i);
    }
}

void FileTable::handleItemDoubleClicked(QTableWidgetItem *item)
{
    // If a video name was selected, get the full file name and
    // emit signal to play the selected one.
    auto fileColumn = static_cast<qint64>(FileTableRowName::FileName);
    if (item->column() != fileColumn)
        return;

    playVideoFromTable(item);
}

void FileTable::setLabelStatus(qint64 rowToInsertAt)
{
    auto testIcon = this->style()->standardIcon(QStyle::SP_DialogCancelButton);

    auto fileCol = static_cast<qint64>(FileTableRowName::FileName);
    auto statusCol = static_cast<qint64>(FileTableRowName::StepStatus);

    auto curFileName = m_table->item(rowToInsertAt, fileCol)->data(Qt::DisplayRole);
    QFileInfo footfallFileInfo = QFileInfo(QDir(m_footfallFolder),
                                           QFileInfo(curFileName.toString()).completeBaseName()
                                               + m_stepFormat);

    if (footfallFileInfo.exists()) {
        testIcon = this->style()->standardIcon(QStyle::SP_DialogApplyButton);
    }

    auto new_item = new QTableWidgetItem(testIcon, "");
    m_table->setItem(rowToInsertAt, statusCol, std::move(new_item));
}

void FileTable::playVideoFromTable(const QTableWidgetItem *item)
{
    QString localPath = item->data(Qt::DisplayRole).toString();
    QString nextVideo = m_rootFolder.filePath(localPath);

    // Note: the slot in resetForNext in StepTable deals with the logic whether to
    // play next video.
    emit sendFootfallOutputMetaData(m_rootFolder.filePath(m_footfallFolder), localPath, nextVideo);
}

void FileTable::styleHeader()
{
    // Work-around to style header:
    // https://stackoverflow.com/questions/38554640/add-border-under-column-headers-in-qtablewidget
    m_table->horizontalHeader()->setStyleSheet("QHeaderView::section{"
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
                                               "}");
}

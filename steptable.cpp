#include "steptable.h"

#include <QGridLayout>
#include <QHeaderView>
#include <QDebug>


StepTable::StepTable(QWidget *parent) : QWidget(parent)
{

    // Initially the table is empty so initialize occupied position to reflect that
    for (size_t i = 0; i < static_cast<qint64>(BodySide::COUNT); ++i){
        m_lastOccupiedPosition.push_back(0);
        m_heelStrikeList.push_back(QVector<qint64>());
    }

    m_table = new QTableWidget(1, static_cast<qint64>(BodySide::COUNT), this);
    m_table->setHorizontalHeaderLabels(QStringList() << "Left" << "Right"); // TODO: use BodySide enum to assign header labels

    styleHeader();

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(m_table, 0, 0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
}

void StepTable::insertRow(qint64 row)
{
    qDebug() << "inserting row";
    m_table->insertRow(row);
}

void StepTable::insertNewRightStep(qint64 frameNum)
{
    addStep(frameNum, BodySide::Right);
}

void StepTable::insertNewLeftStep(qint64 frameNum)
{
    addStep(frameNum, BodySide::Left);
}

void StepTable::addStep(qint64 frameNum, BodySide side) {
    auto columnToInsertAt = static_cast<qint16>(side);
    auto rowToInsertAt = m_lastOccupiedPosition[columnToInsertAt];

    if (rowToInsertAt == m_table->rowCount())
        insertRow(rowToInsertAt);

    // This allows the underlying QVariant in QTableWidgetItem to keep track
    // of the datatype rather than forcing a cast to QString.
    // This allows us to use the default sorting
    auto item = new QTableWidgetItem;
    item->setData(Qt::EditRole, frameNum);
    m_table->setItem(rowToInsertAt, columnToInsertAt, std::move(item));

    m_heelStrikeList[columnToInsertAt].push_back(frameNum);

    // Resort the column in a way that is independent from the others
    std::sort(m_heelStrikeList[columnToInsertAt].begin(), m_heelStrikeList[columnToInsertAt].end());
    for (auto row = 0; row <= rowToInsertAt; row++)
    {
        auto curItem = m_table->item(row, columnToInsertAt);
        curItem->setData(Qt::EditRole, m_heelStrikeList[columnToInsertAt][row]);
    }

    m_lastOccupiedPosition[static_cast<qint16>(side)]++;
    // TODO: make sure we don't insert duplicates

    // TODO: Keep track of next location to insert at for each side.
    // This will correct the current error when the difference in number of left/right steps is > 1
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

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
    connect(m_table, &QTableWidget::itemChanged, this, &StepTable::handleCellChanged);
}

StepTable::~StepTable()
{
    delete m_table;
}

void StepTable::insertRow(qint16 row)
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

void StepTable::handleCellChanged(QTableWidgetItem *item)
{
    qDebug()<< "handleCellChanged";

    auto row = item->row();
    auto col = item->column();
    auto data = item->data(Qt::EditRole);

    // Inserting a new entry
    if (row > m_heelStrikeList[col].size()){
        qDebug()<< "Added new entry";
    }
    else { // Modifying existing entry

    }


}

void StepTable::addStep(qint64 frameNum, BodySide side) {
    auto columnToInsertAt = static_cast<qint16>(side);
    auto rowToInsertAt = m_lastOccupiedPosition[columnToInsertAt];

    // TODO: make sure we don't insert duplicates
    if (alreadyInColumn(columnToInsertAt, frameNum)){
        qDebug() << "Already have " << frameNum << " in table";
        return;
    }

    if (rowToInsertAt == m_table->rowCount())
        insertRow(rowToInsertAt);

    // This allows the underlying QVariant in QTableWidgetItem to keep track
    // of the datatype rather than forcing a cast to QString.
    auto item = new QTableWidgetItem;
    item->setData(Qt::EditRole, frameNum);
    m_table->setItem(rowToInsertAt, columnToInsertAt, std::move(item));

    // Resort the current column independently from others
    m_heelStrikeList[columnToInsertAt].push_back(frameNum);
    sortColumn(columnToInsertAt);

    m_lastOccupiedPosition[columnToInsertAt]++;

}

void StepTable::sortColumn(qint16 col)
{
    // Resort the column in a way that is independent from the others
    std::sort(m_heelStrikeList[col].begin(), m_heelStrikeList[col].end());
    for (auto row = 0; row < m_heelStrikeList[col].size(); row++)
    {
        auto curItem = m_table->item(row, col);
        curItem->setData(Qt::EditRole, m_heelStrikeList[col][row]);
    }

}

bool StepTable::alreadyInColumn(qint16 col, qint64 frameNum)
{
    for (auto &a : m_heelStrikeList[col]) {
        if (a == frameNum)
            return true;
    }
    return false;
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

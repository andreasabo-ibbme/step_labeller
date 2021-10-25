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
    // Dynamically setting column names
    QStringList colLabels;
    for (auto& side: m_sides) {
        colLabels << side;
    }
    m_table->setHorizontalHeaderLabels(colLabels);
    // Fix the style of the header to be consistent with rest of the table
    styleHeader();
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch );
//    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive );
    // Set layout for this widget
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(m_table, 0, 0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);

    connect(m_table, &QTableWidget::itemChanged, this, &StepTable::handleCellChanged);
}

StepTable::~StepTable()
{
    delete m_table; // Not needed because we set parent this this when creating the table
}

void StepTable::insertRow(qint16 row)
{
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
    // This avoids infinite loop when the value in the table is changed
    // without direct user intervention.
    if (m_algorithmicStepAdd) {
        return;
    }
    auto row = item->row();
    auto col = item->column();
    auto frame_num = item->data(Qt::EditRole);

    // If the frame_num is empty, we want to delete the contents
    if (item->text() == ""){
        removeStep(row, col);
        return;
    }

    if (!frame_num.toInt()) {
        qDebug() << "Entry was not an integer";
        // Remove the contents of the cell
        item->setData(Qt::EditRole, "");
        sortColumn(col);
        return;
    }

    // Inserting a new entry
    if (row >= m_heelStrikeList[col].size()){
        addStep(frame_num.toInt(), BodySide(col));
    }
    else { // Modifying existing entry
        if (alreadyInColumn(col, frame_num.toInt())){
            qDebug() << "Already have " << frame_num << " in table";
            return;
        }

        m_heelStrikeList[col][row] = frame_num.toInt();
        sortColumn(col);

    }

    // TODO: signal that can be accepted by mainwindow to change
    // focus back to the playback window
}

bool StepTable::saveFootfalls()
{
    // Make sure the target folder exists (keep here because this is a public function)
    if (!m_outputFolder.exists()) {
        m_outputFolder.mkdir(".");
    }

    qDebug() << "saving to file " << m_outputFile;
    return writeToCSV();


}

void StepTable::resetForNext(QDir output_dir, QString output_file)
{
    // TODO: save to file
    auto success = saveFootfalls();
    // TODO: How to handle failure

    // TODO: Reset footfall table

    // Extract the parts of the video name

    m_outputFile = output_file;
    m_outputFolder = output_dir;
//    qDebug() << "StepTable::resetForNext: m_outputFolder " << m_outputFolder;
//    qDebug() << "StepTable::resetForNext: m_outputFile " << m_outputFile;

    // TODO: Load footfalls if available
}

void StepTable::removeStep(qint16 row, qint16 col){
    // Can't delete if there isn't anything there to delete
    if (row >= m_lastOccupiedPosition[col])
        return;

    m_heelStrikeList[col].remove(row);
    m_lastOccupiedPosition[col]--;
    sortColumn(col);
}

void StepTable::addStep(qint64 frameNum, BodySide side) {
    m_algorithmicStepAdd = true;
    auto columnToInsertAt = static_cast<qint16>(side);
    auto rowToInsertAt = m_lastOccupiedPosition[columnToInsertAt];

    // TODO: make sure we don't insert duplicates
    if (alreadyInColumn(columnToInsertAt, frameNum)){
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

    m_algorithmicStepAdd = false;
}

void StepTable::sortColumn(qint16 col)
{
    qDebug() << "we are sorting: " << col;
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

bool StepTable::writeToCSV()
{
    try {
        // https://stackoverflow.com/questions/27353026/qtableview-output-save-as-csv-or-txt
        qDebug() << "Writing to CSV: " << m_outputFile;

        // Format the header
        QString outputData;
        for (auto& heading: m_sides) {
            outputData += heading + ",";
        }
        outputData.chop(1);
        outputData += "\n";


        // Format the data string
        auto maxRows = *std::max_element(m_lastOccupiedPosition.constBegin(), m_lastOccupiedPosition.constEnd());
        for (int row = 0; row < maxRows; row++) {
            for (int col = 0; col < m_heelStrikeList.size(); col++) {
                // Extract data if we have it, or just output empty cell

                if (m_heelStrikeList[col].size() > row) {
                    outputData += QString::number(m_heelStrikeList[col][row]) + ",";
                }
                else {
                    outputData += ",";
                }

            }
            outputData.chop(1);
            outputData += "\n";
        }

        // Write to file
        QString outputPath = m_outputFolder.filePath(m_outputFile);
        QFile csvFile(outputPath);
        if(csvFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {

            QTextStream out(&csvFile);
            out << outputData;

            csvFile.close();
        }
        return true;
    }
    catch (...) {
        // Failed to write to file
        return false;
    }
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

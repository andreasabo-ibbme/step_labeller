#include "steptable.h"

#include <numeric>
#include <QDebug>
#include <QGridLayout>
#include <QHeaderView>

StepTable::StepTable(QWidget *parent, const QString stepFormat)
    : QWidget(parent)
    , m_stepFormat(stepFormat)
{
    // Initially the table is empty so initialize occupied position to reflect
    // that
    for (size_t i = 0; i < static_cast<qint64>(BodySide::COUNT); ++i) {
        m_lastOccupiedPosition.push_back(0);
        m_heelStrikeList.push_back(QVector<qint64>());
    }

    m_table = new QTableWidget(1, static_cast<qint64>(BodySide::COUNT), this);
    setColumnNames();

    // Fix the style of the header to be consistent with rest of the table
    styleHeader();

    // Set layout for this widget
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(m_table, 0, 0);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    connect(m_table, &QTableWidget::itemChanged, this, &StepTable::handleCellChanged);
}

StepTable::~StepTable()
{
    delete m_table; // Not needed because we set parent to this when creating the
                    // table
}

void StepTable::insertRow(qint64 row)
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
    auto frame_num = item->data(Qt::DisplayRole);

    // If the frame_num is empty, we want to delete the contents
    if (item->text() == "") {
        removeStep(row, col);
        return;
    }

    // Non-integer input or duplicate entry
    if (!frame_num.toInt() || alreadyInColumn(col, frame_num.toInt())) {
        item->setData(Qt::DisplayRole, "");
        sortColumn(col);
        return;
    }

    // Inserting a new entry
    if (row >= m_heelStrikeList[col].size()) {
        addStep(frame_num.toInt(), BodySide(col));
    } else { // Modifying existing entry
        m_heelStrikeList[col][row] = frame_num.toInt();
        sortColumn(col);
    }
}

void StepTable::makeOutputFolder()
{
    if (!m_outputFolder.exists()) {
        m_outputFolder.mkdir(".");
    }
}

bool StepTable::saveFootfalls(bool forceSave)
{
    // Make sure the target folder exists (keep here because this is a public
    // function)
    makeOutputFolder();
    return writeToCSV(forceSave);
}

void StepTable::resetForNext(QDir output_dir, QString localPath, QString nextVideo)
{
    auto output_file = QFileInfo(localPath).completeBaseName() + m_stepFormat;
    auto successSave = saveFootfalls(false);

    if (!successSave) {
        emit failedToSaveFootfalls(QString("Failed to save footfalls for: " + localPath));
        return;
    }

    emit playVideoByName(nextVideo);

    // Extract the parts of the new video name
    m_outputFile = output_file;
    m_outputFolder = output_dir;

    // Load footfalls if available, otherwise just reset table
    readFromCSV();
}

void StepTable::removeStep(qint64 row, qint64 col)
{
    // Can't delete if there isn't anything there to delete
    if (row >= m_lastOccupiedPosition[col])
        return;

    m_heelStrikeList[col].remove(row);
    m_lastOccupiedPosition[col]--;
    sortColumn(col);
}

void StepTable::addStep(qint64 frameNum, BodySide side)
{
    m_algorithmicStepAdd = true;
    auto columnToInsertAt = static_cast<qint64>(side);
    auto rowToInsertAt = m_lastOccupiedPosition[columnToInsertAt];

    if (alreadyInColumn(columnToInsertAt, frameNum)) {
        return;
    }

    if (rowToInsertAt == m_table->rowCount()) {
        insertRow(rowToInsertAt);
    }

    // This allows the underlying QVariant in QTableWidgetItem to keep track
    // of the datatype rather than forcing a cast to QString.
    auto item = new QTableWidgetItem;
    item->setData(Qt::DisplayRole, frameNum);
    m_table->setItem(rowToInsertAt, columnToInsertAt, std::move(item));

    // Re-sort the current column independently from others
    m_heelStrikeList[columnToInsertAt].push_back(frameNum);
    m_lastOccupiedPosition[columnToInsertAt]++;
    sortColumn(columnToInsertAt);
    m_algorithmicStepAdd = false;
}

void StepTable::sortColumn(qint64 col)
{
    // Resort the column in a way that is independent from the others
    std::sort(m_heelStrikeList[col].begin(), m_heelStrikeList[col].end());
    for (auto row = 0; row < m_heelStrikeList[col].size(); row++) {
        auto curItem = m_table->item(row, col);
        curItem->setData(Qt::DisplayRole, m_heelStrikeList[col][row]);
    }

    // Insert extra empty row in the display to allow manual addition of steps
    m_table->setRowCount(getMaxRows() + 1);
}

bool StepTable::alreadyInColumn(qint64 col, qint64 frameNum)
{
    for (auto &a : m_heelStrikeList[col]) {
        if (a == frameNum)
            return true;
    }
    return false;
}

qint64 StepTable::getMaxRows()
{
    return *std::max_element(m_lastOccupiedPosition.cbegin(), m_lastOccupiedPosition.cend());
}

QVector<QString> StepTable::formatStepsForCSV()
{
    // Format the header
    QString headerData;
    for (auto &heading : m_sides) {
        headerData += heading + ",";
    }
    headerData.chop(1); // Remove the last comma and replace with a newline
    headerData += "\n";

    // Format the data string
    auto maxRows = getMaxRows();
    QVector<QString> outputVec;
    outputVec.reserve(maxRows + 1);
    outputVec.append(headerData);

    for (int row = 0; row < maxRows; row++) {
        QString curData;
        curData.reserve(sizeof(char) * 100);

        for (int col = 0; col < m_heelStrikeList.size(); col++) {
            // Extract data if we have it, or just output empty cell
            if (m_heelStrikeList[col].size() > row) {
                curData.append(QString::number(m_heelStrikeList[col][row]) + ",");
            } else {
                curData.append(",");
            }
        }
        curData.chop(1); // Remove trailing comma
        curData.append("\n");
        outputVec.append(curData);
    }

    return outputVec;
}

void StepTable::clearAllSteps()
{
    // Clear the internal record-keeping
    for (auto &item : m_heelStrikeList) {
        item.clear();
    }

    for (auto &item : m_lastOccupiedPosition) {
        item = 0;
    }

    // Clear the display
    m_table->setRowCount(0);

    // Reset to one empty row to allow for manual addition of steps
    m_table->setRowCount(1);
}

bool StepTable::writeToCSV(bool forceSave = false)
{
    if (!forceSave && !getMaxRows())
        return true;
    try {
        // https://stackoverflow.com/questions/27353026/qtableview-output-save-as-csv-or-txt
        qDebug() << "Writing to CSV: " << m_outputFile;
        auto outputVec = formatStepsForCSV();
        QString outputData;

        // Reformat from vector of strings to one string
        // https://www.qt.io/blog/efficient-qstring-concatenation-with-c17-fold-expressions
        auto output_size = std::accumulate(outputVec.cbegin(),
                                           outputVec.cend(),
                                           0,
                                           [](size_t acc, const QString &s) {
                                               return acc + s.length();
                                           });
        outputData.resize(output_size);
        std::accumulate(outputVec.cbegin(),
                        outputVec.cend(),
                        outputData.begin(),
                        [](const auto &dest, const QString &s) {
                            return std::copy(s.cbegin(), s.cend(), dest);
                        });

        // Write to file
        auto outputPath = m_outputFolder.filePath(m_outputFile);
        QFile csvFile(outputPath);
        if (csvFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QTextStream out(&csvFile);
            out << outputData;
            csvFile.close();
            emit updatedCSVFile();
            return true;
        }
        return false; // Weren't able to write to file
    } catch (...) {
        // Failed to write to file
        return false;
    }
}

bool StepTable::readFromCSV()
{
    // Make sure that the table is empty before we try to add to it
    clearAllSteps();

    auto outputPath = m_outputFolder.filePath(m_outputFile);
    QFile csvFile(outputPath);

    // Parse file into internal structs first
    if (csvFile.open(QIODevice::ReadOnly | QIODevice::Truncate)) {
        QTextStream dataStream(&csvFile);
        bool firstLine{true};
        while (!dataStream.atEnd()) {
            auto line = dataStream.readLine();
            // Skip the headings
            if (firstLine) {
                firstLine = false;
                continue;
            }
            auto fields = line.split(",");

            for (int i = 0; i < m_lastOccupiedPosition.size(); ++i) {
                if (fields[i].isEmpty())
                    continue;
                addStep(fields[i].toInt(), static_cast<BodySide>(i));
            }
        }
        csvFile.close();
        return true;
    }
    return false;
}

void StepTable::styleHeader()
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

void StepTable::setColumnNames()
{
    // Dynamically setting column names
    QStringList colLabels;
    for (auto &side : m_sides) {
        colLabels << side;
    }
    m_table->setHorizontalHeaderLabels(colLabels);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

#ifndef STEPTABLE_H
#define STEPTABLE_H

#include <QDir>
#include <QTableWidget>
#include <QWidget>

enum class BodySide {Left, Right, COUNT};

class StepTable : public QWidget
{
    Q_OBJECT
public:
    explicit StepTable(QWidget *parent = nullptr);
    ~StepTable();

public slots:
    void insertRow(qint16 row);
    void insertNewRightStep(qint64 frameNum);
    void insertNewLeftStep(qint64 frameNum);
    void handleCellChanged(QTableWidgetItem* item);
    void resetForNext(QDir m_rootFolder, QString outputFile);
    bool saveFootfalls();
    void clearAllSteps();

private:
    void styleHeader();
    void setColumnNames();
    void addStep(qint64 frameNum, BodySide side);
    void removeStep(qint16 row, qint16 col);
    void sortColumn(qint16 col);
    bool alreadyInColumn (qint16 col, qint64 frameNum);

    bool writeToCSV();
    bool readFromCSV();
    QVector<QString> formatStepsForCSV();

    QString m_outputFile;
    QDir m_outputFolder;
    QTableWidget *m_table;
    QVector<qint64> m_lastOccupiedPosition;
    QVector<QVector<qint64>> m_heelStrikeList;
    bool m_algorithmicStepAdd = false;
    QVector<QString> m_sides{"Left", "Right"};

};

#endif // STEPTABLE_H

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

signals:
    void updatedCSVFile();

public slots:
    void insertRow(qint64 row);
    void insertNewRightStep(qint64 frameNum);
    void insertNewLeftStep(qint64 frameNum);
    void handleCellChanged(QTableWidgetItem* item);
    void resetForNext(QDir m_rootFolder, QString outputFile);
    bool saveFootfalls(bool forceSave);
    void clearAllSteps();

private:
    void styleHeader();
    void setColumnNames();
    void addStep(qint64 frameNum, BodySide side);
    void removeStep(qint64 row, qint64 col);
    void sortColumn(qint64 col);
    void makeOutputFolder();
    bool alreadyInColumn (qint64 col, qint64 frameNum);
    qint64 getMaxRows();

    bool writeToCSV(bool forceSave);
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

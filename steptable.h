#ifndef STEPTABLE_H
#define STEPTABLE_H

#include <QWidget>
#include <QTableWidget>

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

private:
    void styleHeader();
    void addStep(qint64 frameNum, BodySide side);
    void sortColumn(qint16 col);
    bool alreadyInColumn (qint16 col, qint64 frameNum);

private:
    QTableWidget *m_table;
    QVector<qint64> m_lastOccupiedPosition;
    QVector<QVector<qint64>> m_heelStrikeList;
};

#endif // STEPTABLE_H

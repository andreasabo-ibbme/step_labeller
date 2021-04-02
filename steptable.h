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

public slots:
    void insertRow(qint64 row);
    void insertNewRightStep(qint64 frameNum);
    void insertNewLeftStep(qint64 frameNum);

private:
    void styleHeader();
    void addStep(qint64 frameNum, BodySide side);
private:
    QTableWidget *m_table;
    QVector<qint64> m_lastOccupiedPosition;
    QVector<QVector<qint64>> m_heelStrikeList;
};

#endif // STEPTABLE_H

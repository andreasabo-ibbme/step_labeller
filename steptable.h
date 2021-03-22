#ifndef STEPTABLE_H
#define STEPTABLE_H

#include <QWidget>
#include <QTableWidget>

class StepTable : public QWidget
{
    Q_OBJECT
public:
    explicit StepTable(QWidget *parent = nullptr);

public slots:
    void insertRow(int row);

private:
    void styleHeader();
private:
    QTableWidget *m_table;

};

#endif // STEPTABLE_H

#ifndef FILETABLE_H
#define FILETABLE_H

#include <QWidget>
#include <QTableWidget>
#include <QFileInfoList>

enum class FileTableRowName {FileName, StepStatus, COUNT};

class FileTable : public QWidget
{
    Q_OBJECT
public:
    explicit FileTable(QWidget *parent = nullptr);

public slots:
    void fillTableWithFiles(QFileInfoList files);

signals:


private:
    void styleHeader();
    QTableWidget *m_table;
    qint64 m_lastOccupiedPosition;
};

#endif // FILETABLE_H

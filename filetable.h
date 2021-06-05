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
    void fillTableWithFiles(QFileInfoList files, QString footfallFolder, QString stepFormat);

private:
    void setLabelStatus(qint64 rowToInsertAt, QString footfallFolder, QString stepFormat);

private:
    void styleHeader();
    QTableWidget *m_table;
    qint64 m_lastOccupiedPosition;
};

#endif // FILETABLE_H

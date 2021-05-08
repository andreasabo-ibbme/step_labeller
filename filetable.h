#ifndef FILETABLE_H
#define FILETABLE_H

#include <QWidget>
#include <QTableWidget>

enum class FileTableRowName {FileName, StepStatus, COUNT};


class FileTable : public QWidget
{
    Q_OBJECT
public:
    explicit FileTable(QWidget *parent = nullptr);

signals:


private:
    void styleHeader();
    QTableWidget *m_table;
};

#endif // FILETABLE_H

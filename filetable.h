#ifndef FILETABLE_H
#define FILETABLE_H

#include <QDir>
#include <QWidget>
#include <QTableWidget>
#include <QFileInfoList>

enum class FileTableRowName {FileName, StepStatus, COUNT};

class FileTable : public QWidget
{
    Q_OBJECT
public:
    explicit FileTable(QWidget *parent = nullptr);

signals:
    void playVideoByName(QString video);

public slots:
    void fillTableWithFiles(QFileInfoList files, QString footfallFolder, QDir videoFolder, QString stepFormat);

private slots:
    void handleItemDoubleClicked(QTableWidgetItem *item);

private:
    void setLabelStatus(qint64 rowToInsertAt, QString footfallFolder, QString stepFormat);

private:
    void styleHeader();
    QTableWidget *m_table;
    qint64 m_lastOccupiedPosition;
    QDir m_rootFolder;
    QString m_stepFormat;
};

#endif // FILETABLE_H

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
    void fillTableWithFiles(QFileInfoList files, QString footfallFolder, QDir videoFolder, QString stepFormat);
    void playFirstVideo();

signals:
    void playVideoByName(QString video);
    void sendFootfallOutputMetaData(QDir m_rootFolder, QString outputFile);

public slots:
    void updateFileLabelStatus();

private slots:
    void handleItemDoubleClicked(QTableWidgetItem *item);

private:
    void setLabelStatus(qint64 rowToInsertAt);
    void playVideoFromTable(const QTableWidgetItem *item);

private:
    void styleHeader();
    QTableWidget *m_table;
    qint64 m_lastOccupiedPosition;
    QDir m_rootFolder;
    QString m_footfall_folder;
    QString m_stepFormat;
};

#endif // FILETABLE_H

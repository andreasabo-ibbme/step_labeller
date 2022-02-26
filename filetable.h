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
    explicit FileTable(QWidget *parent = nullptr, const QString stepFormat = ".csv");
    void fillTableWithFiles(QFileInfoList files, QString footfallFolder, QDir videoFolder);
    void playFirstVideo();

signals:
    void sendFootfallOutputMetaData(QDir m_rootFolder, QString outputFile, QString nextVideo);
    void failedToPlayVideo(QString video);

public slots:
    void updateFileLabelStatus();

private slots:
    void handleItemDoubleClicked(QTableWidgetItem *item);

private:
    void setLabelStatus(qint64 rowToInsertAt);
    void playVideoFromTable(const QTableWidgetItem *item);
    bool isValidVideo(const QString& file);

private:
    void styleHeader();
    QTableWidget *m_table;
    qint64 m_lastOccupiedPosition;
    QDir m_rootFolder;
    QString m_footfall_folder;
    QVector<QString> m_acceptableFormats;
    QString m_stepFormat;
};

#endif // FILETABLE_H

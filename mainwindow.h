#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QCheckBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QLabel>
#include <QListView>
#include <QMainWindow>
#include <QMutex>
#include <QPushButton>
#include <QStatusBar>
#include <QTableWidget>
#include <QFileInfoList>

#include "capture_thread.h"
#include "filetable.h"
#include "playercontrols.h"
#include "steptable.h"

#include "opencv2/opencv.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void initUI();
    void createActions();
    void connectPlaybackControls();
    void connectCaptureControls();

private slots:
    void updateFrame(cv::Mat* frame, qint64 frameNum);
    void updateFPS(float fps);
    void findVideos();
    void openVideo(QString video);

private:
    // Menu items
    QMenu *fileMenu;
    QAction *exitAction;
    QAction *openVidsAction;

    // Buttons
    QPushButton *m_clearStepsButton;
    QPushButton *m_saveStepsButton;
    QStatusBar *m_mainStatusBar;
    QLabel *m_mainStatusLabel;

    // Video playback
    QGraphicsScene *m_imageScene;
    QGraphicsView *m_imageView;
    cv::Mat m_currentFrame;
    std::mutex *m_data_lock;

    // Sub components
    StepTable *m_table;
    FileTable *m_fileTable;
    CaptureThread *m_capturer;
    PlayerControls *m_controls;

    qint64 m_frameNum;
    QString m_outputStepFormat;
    QFont m_Font;
};
#endif // MAINWINDOW_H

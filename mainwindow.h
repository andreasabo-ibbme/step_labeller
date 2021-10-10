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

public slots:
    void changeVideo(QString video);

private slots:
    void showCameraInfo();
    void openCamera();
    void updateFrame(cv::Mat* frame, qint64 frameNum);
    void updateFPS(float fps);
    void findVideos();
    void openVideo(QString video);
    void exportSteps();


private:
    QMenu *fileMenu;
    QAction *cameraInfoAction;
    QAction *openCameraAction;
    QAction *exitAction;
    QAction *openVidsAction;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QCheckBox *monitorCheckBox;
    QPushButton *findVideosButton;
    QPushButton *m_startExportButton;

    QListView *saved_list;

    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;

    cv::Mat currentFrame;
    std::mutex *data_lock;
    CaptureThread *capturer;
    PlayerControls *controls;

    // To move to videolist widget
    QFileInfoList m_video_list;

    StepTable *m_table;
    FileTable *m_fileTable;
    qint64 m_frameNum;

    QString m_footfall_path;
    QString m_outputStepFormat;
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QCheckBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QListView>
#include <QMainWindow>
#include <QMutex>
#include <QPushButton>
#include <QStatusBar>
#include <QFileInfoList>

#include "capture_thread.h"
#include "playercontrols.h"

#include "opencv2/opencv.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initUI();
    void createActions();
    void connectPlaybackControls();

private slots:
    void showCameraInfo();
    void openCamera();
    void updateFrame(cv::Mat* frame, qint64 frameNum);
    void updateFPS(float fps);
    void findVideos();
    void openVideo(QString video);


private:
    QMenu *fileMenu;
    QAction *cameraInfoAction;
    QAction *openCameraAction;
    QAction *exitAction;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QCheckBox *monitorCheckBox;
    QPushButton *findVideosButton;

    QListView *saved_list;

    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;

    cv::Mat currentFrame;
    std::mutex *data_lock;
    CaptureThread *capturer;
    PlayerControls *controls;

    // To move to videolist widget
    QFileInfoList m_video_list;

};
#endif // MAINWINDOW_H

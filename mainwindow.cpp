#include "mainwindow.h"
#include "playercontrols.h"


#include <QCameraInfo>
#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QMenuBar>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initUI();
    data_lock = new std::mutex;
    capturer = nullptr;

}

MainWindow::~MainWindow()
{
}

void MainWindow::initUI()
{
    qDebug() << "HERE";
    this->resize(2000, 800);
    constexpr size_t default_fps = 30;

    // Set up playback controls
    controls = new PlayerControls(default_fps, this);

    // Set up the menubar
    fileMenu = menuBar()->addMenu("&File");

    // Set up the playback area
    QGridLayout *main_layout = new QGridLayout();
    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    main_layout->addWidget(imageView, 0, 0, 12, 1);



//    monitorCheckBox = new QCheckBox(this);
//    monitorCheckBox->setText("Monitor on/off");
//    tool_layout->addWidget(monitorCheckBox, 0, 0);


//    tool_layout->addWidget(new QLabel(this), 0, 2);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->addStretch(1);
    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);


    // Set up the tools layout
    QGridLayout *tool_layout = new QGridLayout();
    main_layout->addLayout(tool_layout, 12, 0, 1, -1);
    findVideosButton = new QPushButton(this);
    findVideosButton->setText("Open Videos");
    tool_layout->addWidget(findVideosButton, 0, 0, 1, 1);
    tool_layout->addLayout(controlLayout, 0, 4, 1, -1);

    // Set up list of saved videos
    saved_list = new QListView(this);
    main_layout->addWidget(saved_list, 14, 0, 4, 1);

    QWidget *layout_widget = new QWidget(this);
    layout_widget->setLayout(main_layout);
    setCentralWidget(layout_widget);


    // Set up the status bar
    mainStatusBar = statusBar();
    mainStatusLabel = new QLabel(mainStatusBar);
    mainStatusBar->addPermanentWidget(mainStatusLabel);
    mainStatusLabel->setText("Gazer is Ready");

    connect(findVideosButton, &QPushButton::pressed, this, &MainWindow::findVideos);

    createActions();

}

void MainWindow::createActions()
{
    cameraInfoAction = new QAction("Camera &Information", this);
    openCameraAction = new QAction("&Open Camera", this);
    exitAction = new QAction("E&xit", this);

    fileMenu->addAction(cameraInfoAction);
    fileMenu->addAction(openCameraAction);
    fileMenu->addAction(exitAction);

    // Set up the connections
    connect(exitAction, &QAction::triggered, this, &QApplication::quit);
    connect(cameraInfoAction, &QAction::triggered, this, &MainWindow::showCameraInfo);
    connect(openCameraAction, &QAction::triggered, this, &MainWindow::openCamera);


}

void MainWindow::connectPlaybackControls()
{
    connect(controls, &PlayerControls::play, capturer, &CaptureThread::play);
    connect(controls, &PlayerControls::pause, capturer, &CaptureThread::pause);
    connect(controls, &PlayerControls::next, capturer, &CaptureThread::next);
    connect(controls, &PlayerControls::previous, capturer, &CaptureThread::previous);
}

void MainWindow::showCameraInfo()
{
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    QString info = QString("Available Cameras: \n");

    foreach (const QCameraInfo &cameraInfo, cameras) {
       info += " - " + cameraInfo.deviceName() + ": ";
       info += cameraInfo.description() + "\n";
    }
    QMessageBox::information(this, "Cameras", info);
}





void MainWindow::updateFrame(cv::Mat* mat, qint64 frameNum)
{
    {
        // Lock while updating the frame for display
        std::lock_guard<std::mutex> lock(*data_lock);
        currentFrame = *mat;
    }
    QFont serifFont("Times", 16, QFont::Bold);

    QImage frame(
                currentFrame.data,
                currentFrame.cols,
                currentFrame.rows,
                currentFrame.step,
                QImage::Format_RGB888);

    // Resize the image so it fits within the imageView
    QSize viewrect_size = imageView->viewport()->size();
    frame = frame.scaled(viewrect_size, Qt::KeepAspectRatio);

    QPixmap image = QPixmap::fromImage(frame);

    imageScene->clear();
    imageView->resetTransform();
    imageScene->addPixmap(image);
    imageScene->addText(QString::number(frameNum), serifFont);

    imageScene->update();
    imageView->setSceneRect(image.rect());
}

void MainWindow::updateFPS(float fps)
{
    mainStatusLabel->setText(QString("FPS is %1").arg(fps));

}

void MainWindow::findVideos()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::Detail);

    QStringList dirName;
     if (dialog.exec())
     {
         dirName = dialog.selectedFiles();
         qDebug() << dirName;
         auto myDir = QDir(dirName.at(0));
         QFileInfoList list = myDir.entryInfoList(QDir::Files);
         qDebug() << list;
         m_video_list = list;
         openVideo((m_video_list[0]).absoluteFilePath());
     }
     else {
         // TODO: error handling if did not select dir correctly.
     }
}

void MainWindow::openCamera()
{
    if (capturer != nullptr) {
        // If a thread is already running, stop it and start adn new one
        capturer->setRunning(false);
        disconnect(capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
        connect(capturer, &CaptureThread::finished, capturer, &CaptureThread::deleteLater);
    }

    int camera_ind = 0;
    capturer = new CaptureThread(camera_ind, data_lock,  controls->playbackRate());
    connect(capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
    connect(capturer, &CaptureThread::fpsChanged, this, &MainWindow::updateFPS);
    connect(capturer, &CaptureThread::stateChanged, controls, &PlayerControls::setState);
    connect(controls, &PlayerControls::changeRate, capturer, &CaptureThread::rateChanged);

    connectPlaybackControls();
    capturer->start();
    capturer->startCalcFPS(true);
    mainStatusLabel->setText(QString("Capturing camera: %1").arg(camera_ind));
}

void MainWindow::openVideo(QString video)
{
    qDebug() << "Opening video: " << video;
    if (capturer != nullptr) {
        // If a thread is already running, stop it and start adn new one
        capturer->setRunning(false);
        disconnect(capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
        connect(capturer, &CaptureThread::finished, capturer, &CaptureThread::deleteLater);
    }

    capturer = new CaptureThread(video, data_lock, controls->playbackRate());
    connect(capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
    connect(capturer, &CaptureThread::fpsChanged, this, &MainWindow::updateFPS);
    connect(capturer, &CaptureThread::stateChanged, controls, &PlayerControls::setState);
    connect(controls, &PlayerControls::changeRate, capturer, &CaptureThread::rateChanged);

    connectPlaybackControls();
    capturer->start();
    capturer->startCalcFPS(true);
    mainStatusLabel->setText(QString("Playing video from: %1").arg(video));
}



















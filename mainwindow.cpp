#include "mainwindow.h"
#include "playercontrols.h"


#include <QCameraInfo>
#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QMenuBar>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), capturer(nullptr), m_frameNum(0)
{
    initUI();
    data_lock = new std::mutex;
    m_outputStepFormat = ".csv";
}

MainWindow::~MainWindow()
{
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!capturer)
        return;
    switch (event->key()){
    case Qt::Key_A:
        capturer->previous();
        break;
    case Qt::Key_D:
        capturer->next();
        break;
    case Qt::Key_Space:
        capturer->togglePlayPause();
        break;
    case Qt::Key_E:
        m_table->insertNewRightStep(m_frameNum);
        break;
    case Qt::Key_Q:
        m_table->insertNewLeftStep(m_frameNum);
        break;
    default:
        break;
    }

    qDebug() << event->key();
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

    QGridLayout *main_layout = new QGridLayout();


    // Set up the playback area
    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    imageView->setFocusPolicy(Qt::StrongFocus);
//    main_layout->addWidget(imageView, 0, 1, 12, 5);



    // Set up file table view
//    m_fileTable = new FileTable();
//    main_layout->addWidget(m_fileTable, 0, 0, 12, 1);

    // Set up step export
//    m_startExportButton = new QPushButton("Export steps", this);
//    main_layout->addWidget(m_startExportButton, 12, 1, 1, 2, Qt::AlignCenter);
//    connect(m_startExportButton, &QPushButton::clicked, this, &MainWindow::exportSteps);


    // Set up file table view
    m_fileTable = new FileTable();
    main_layout->addWidget(m_fileTable, 0, 0, 10, 2);


    // Set up playback window and controls
    main_layout->addWidget(imageView, 0, 2, 10, 5);
    main_layout->addWidget(controls, 10, 2, 1, 5);

    // Set up step table view
    m_table = new StepTable();
    main_layout->addWidget(m_table, 0, 7, 10, 2);



    // Set the layout for the main window
    QWidget *layout_widget = new QWidget(this);
    layout_widget->setLayout(main_layout);
    setCentralWidget(layout_widget);


    // Set up the tools layout
//    QGridLayout *tool_layout = new QGridLayout();
//    main_layout->addLayout(tool_layout, 12, 0, 1, 1);
//    tool_layout->addWidget(controls, 0, 0, 1, 1, Qt::AlignCenter);

//    main_layout->addLayout(videoPlaybackLayout, 0, 0);




    // Set up the status bar
    mainStatusBar = statusBar();
    mainStatusLabel = new QLabel(mainStatusBar);
    mainStatusBar->addPermanentWidget(mainStatusLabel);
    mainStatusLabel->setText("Labeller is Ready");

    createActions();

}

void MainWindow::createActions()
{
    cameraInfoAction = new QAction("Camera &Information", this);
    openCameraAction = new QAction("&Open Camera", this);
    exitAction = new QAction("E&xit", this);
    openVidsAction = new QAction("Open Videos", this);

//    fileMenu->addAction(cameraInfoAction);
//    fileMenu->addAction(openCameraAction);

    fileMenu->addAction(openVidsAction);
    fileMenu->addAction(exitAction);

    // Set up the connections    
    connect(openVidsAction, &QAction::triggered, this, &MainWindow::findVideos);
    connect(exitAction, &QAction::triggered, this, &QApplication::quit);
//    connect(cameraInfoAction, &QAction::triggered, this, &MainWindow::showCameraInfo);
//    connect(openCameraAction, &QAction::triggered, this, &MainWindow::openCamera);

    connect(m_fileTable, &FileTable::playVideoByName, this, &MainWindow::openVideo);
    connect(m_fileTable, &FileTable::sendFootfallOutputMetaData, m_table, &StepTable::resetForNext);
}

void MainWindow::connectPlaybackControls()
{
    connect(capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
    connect(capturer, &CaptureThread::fpsChanged, this, &MainWindow::updateFPS);
    connect(capturer, &CaptureThread::stateChanged, controls, &PlayerControls::setState);
    connect(controls, &PlayerControls::changeRate, capturer, &CaptureThread::rateChanged);
    connect(controls, &PlayerControls::changeFrame, capturer, &CaptureThread::frameChanged);


    connect(controls, &PlayerControls::play, capturer, &CaptureThread::play);
    connect(controls, &PlayerControls::pause, capturer, &CaptureThread::pause);
    connect(controls, &PlayerControls::next, capturer, &CaptureThread::next);
    connect(controls, &PlayerControls::previous, capturer, &CaptureThread::previous);
    connect(controls, &PlayerControls::stop, capturer, &CaptureThread::stop);
}

void MainWindow::changeVideo(QString video)
{

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
        m_frameNum = frameNum;
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
    QString default_footfall = "footfalls";
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::Detail);

    QStringList dirName;
     if (dialog.exec())
     {
         dirName = dialog.selectedFiles();
         auto myDir = QDir(dirName.at(0));

         m_video_list = myDir.entryInfoList(QDir::Files);

         // Set default footfall path
         m_footfall_path = myDir.filePath(default_footfall);
         m_fileTable->fillTableWithFiles(m_video_list, m_footfall_path, myDir, m_outputStepFormat);

         // Automatically start playing the first video in the list
         openVideo((m_video_list[0]).absoluteFilePath());
     }
     else {
         // TODO: error handling if did not select dir correctly. IE. non-playable files in folder
     }
     qDebug() << "done in findVideos";

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

    // Needs the newly created capture thread
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

    connectPlaybackControls();
    capturer->start();
    capturer->startCalcFPS(false);
    mainStatusLabel->setText(QString("Playing video from: %1").arg(video));
}

void MainWindow::exportSteps()
{
    // TODO:
    qDebug() << "ANDREA EXPORT STEPS";
}



















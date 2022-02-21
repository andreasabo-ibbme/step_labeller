#include "mainwindow.h"
#include "playercontrols.h"


#include <QCameraInfo>
#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QMenuBar>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_capturer(nullptr), m_frameNum(0)
{
    initUI();
    m_data_lock = new std::mutex;
    m_outputStepFormat = ".csv";
    m_Font = QFont("Times", 16, QFont::Bold);
}

MainWindow::~MainWindow()
{
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!m_capturer)
        return;
    switch (event->key()){
    case Qt::Key_A:
        m_capturer->previous();
        break;
    case Qt::Key_D:
        m_capturer->next();
        break;
    case Qt::Key_Space:
        m_capturer->togglePlayPause();
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
    qDebug() << "Initializing UI";
    this->resize(2000, 800);
    constexpr size_t default_fps = 30;

    // Set up playback m_controls
    m_controls = new PlayerControls(default_fps, this);

    // Set up the menubar
    fileMenu = menuBar()->addMenu("&File");
    auto *main_layout = new QGridLayout(this);


    // Set up the playback area
    m_imageScene = new QGraphicsScene(this);
    m_imageView = new QGraphicsView(m_imageScene);
    m_imageView->setFocusPolicy(Qt::StrongFocus);

    // Set up file table view
    m_fileTable = new FileTable(this);
    main_layout->addWidget(m_fileTable, 0, 0, 10, 2);

    // Set up playback window and m_controls
    main_layout->addWidget(m_imageView, 0, 2, 10, 5);
    main_layout->addWidget(m_controls, 10, 2, 1, 5);

    // Set up step table view
    m_table = new StepTable(this);
    main_layout->addWidget(m_table, 0, 7, 10, 2);

    // Clear step table button
    m_clearStepsButton = new QPushButton("Clear all steps", this);
    connect(m_clearStepsButton, &QPushButton::clicked, m_table, &StepTable::clearAllSteps);
    main_layout->addWidget(m_clearStepsButton, 10, 7, 1, 1, Qt::AlignCenter);

    // Save step table button
    m_saveStepsButton = new QPushButton("Save steps to CSV", this);
    connect(m_saveStepsButton, &QPushButton::clicked, m_table, &StepTable::saveFootfalls);
    main_layout->addWidget(m_saveStepsButton, 10, 8, 1, 1, Qt::AlignCenter);

    // Set the layout for the main window
    auto *layout_widget = new QWidget(this);
    layout_widget->setLayout(main_layout);
    setCentralWidget(layout_widget);


    // Set up the status bar
    m_mainStatusBar = statusBar();
    m_mainStatusLabel = new QLabel(m_mainStatusBar);
    m_mainStatusBar->addPermanentWidget(m_mainStatusLabel);
    m_mainStatusLabel->setText("Step labeller is ready. Open folder in top-left.");

    createActions();
    qDebug() << "Done initializing UI";
}

void MainWindow::createActions()
{
    exitAction = new QAction("E&xit", this);
    openVidsAction = new QAction("Open Videos", this);

    fileMenu->addAction(openVidsAction);
    fileMenu->addAction(exitAction);

    // Set up the connections for fileMenu
    connect(openVidsAction, &QAction::triggered, this, &MainWindow::findVideos);
    connect(exitAction, &QAction::triggered, this, &QApplication::quit);

    // Set up the connections for FileTable class
    connect(m_fileTable, &FileTable::playVideoByName, this, &MainWindow::openVideo);
    connect(m_fileTable, &FileTable::sendFootfallOutputMetaData, m_table, &StepTable::resetForNext);
}

void MainWindow::connectPlaybackControls()
{
    connect(m_capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
    connect(m_capturer, &CaptureThread::fpsChanged, this, &MainWindow::updateFPS);
    connect(m_capturer, &CaptureThread::stateChanged, m_controls, &PlayerControls::setState);
    connect(m_controls, &PlayerControls::changeRate, m_capturer, &CaptureThread::rateChanged);
    connect(m_controls, &PlayerControls::changeFrame, m_capturer, &CaptureThread::frameChanged);


    connect(m_controls, &PlayerControls::play, m_capturer, &CaptureThread::play);
    connect(m_controls, &PlayerControls::pause, m_capturer, &CaptureThread::pause);
    connect(m_controls, &PlayerControls::next, m_capturer, &CaptureThread::next);
    connect(m_controls, &PlayerControls::previous, m_capturer, &CaptureThread::previous);
    connect(m_controls, &PlayerControls::stop, m_capturer, &CaptureThread::stop);
}

void MainWindow::updateFrame(cv::Mat* mat, qint64 frameNum)
{
    {
        // Lock while updating the frame for display
        std::lock_guard<std::mutex> lock(*m_data_lock);
        m_currentFrame = *mat;
        m_frameNum = frameNum;
    }

    QImage frame(
                m_currentFrame.data,
                m_currentFrame.cols,
                m_currentFrame.rows,
                m_currentFrame.step,
                QImage::Format_RGB888);

    // Resize the image so it fits within the m_imageView
    auto viewrect_size = m_imageView->viewport()->size();
    frame = frame.scaled(viewrect_size, Qt::KeepAspectRatio);

    auto image = QPixmap::fromImage(frame);

    m_imageScene->clear();
    m_imageView->resetTransform();
    m_imageScene->addPixmap(image);
    m_imageScene->addText(QString::number(frameNum), m_Font);

    m_imageScene->update();
    m_imageView->setSceneRect(image.rect());
}

void MainWindow::updateFPS(float fps)
{
    m_mainStatusLabel->setText(QString("FPS is %1").arg(fps));
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
         auto video_list = myDir.entryInfoList(QDir::Files);
         auto footfallPath = myDir.filePath(default_footfall);

         m_fileTable->fillTableWithFiles(video_list, footfallPath, myDir, m_outputStepFormat);
         // Automatically start playing the first video in the list
         m_fileTable->playFirstVideo();
     }
     else {
         // TODO: error handling if did not select dir correctly. IE. non-playable files in folder
     }
}

void MainWindow::openVideo(QString video)
{
    qDebug() << "Opening video: " << video;
    if (m_capturer != nullptr) {
        // If a thread is already running, stop it and start a new one
        m_capturer->setRunning(false);
        disconnect(m_capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
        connect(m_capturer, &CaptureThread::finished, m_capturer, &CaptureThread::deleteLater);
    }

    m_capturer = new CaptureThread(video, m_data_lock, m_controls->playbackRate());

    connectPlaybackControls();
    m_capturer->start();
    m_capturer->startCalcFPS(false);
    m_mainStatusLabel->setText(QString("Playing video from: %1").arg(video));
}

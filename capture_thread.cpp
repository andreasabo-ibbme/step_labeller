#include "capture_thread.h"
#include <QElapsedTimer>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <mutex>
#include "opencv2/videoio.hpp"

CaptureThread::CaptureThread(int camera, std::mutex *lock, qreal playback_rate):
    m_running(false), m_cameraID(camera), m_videoPath(""), m_dataLock(lock), m_playbackFPS(playback_rate), m_fpsCalculating(false), m_fpsSum(0), m_curFPSSampleCount(0)
{
    m_timeSamples.resize(m_fpsSamples);
    m_delayMS = 1/m_playbackFPS * 1000;
}

CaptureThread::CaptureThread(QString videoPath, std::mutex *lock, qreal playback_rate):
    m_running(false), m_cameraID(-1), m_videoPath(videoPath), m_dataLock(lock), m_playbackFPS(playback_rate), m_fpsCalculating(false), m_fpsSum(0), m_curFPSSampleCount(0)
{
    m_timeSamples.resize(m_fpsSamples);
    m_delayMS = 1/m_playbackFPS * 1000;
}

CaptureThread::~CaptureThread()
{
    m_cap.release();
}

void CaptureThread::run()
{
    if (m_cameraID < 0) { // Video playback
        m_cap.open((m_videoPath.toStdString().c_str()));
    }
    else { // Webcam stream
        m_cap.open(m_cameraID);
    }
    m_running = true;

    exec();
}

void CaptureThread::videoPlayback(bool& haveMoreFrames)
{
    QElapsedTimer timer;

    while (m_running && haveMoreFrames)
    {
        timer.restart();

        if (m_state == QMediaPlayer::PlayingState){
            haveMoreFrames = readNextVideoFrame();
        }
        auto sleeptime = m_delayMS - timer.elapsed();
        sleeptime = sleeptime < 0 ? 0 : sleeptime;

        // Use sleep to pause the capture thread to simulate the correct FPS
        QThread::msleep(static_cast<unsigned long>(sleeptime));
    }
}

bool CaptureThread::readNextVideoFrame()
{
    m_cap >> m_tmpFrame;
    if (m_tmpFrame.empty()) {
        return false;
    }

    cv::cvtColor(m_tmpFrame, m_tmpFrame, cv::COLOR_BGR2RGB);

    // Only lock when updating the frame shared with the UI thread
    {
        std::lock_guard<std::mutex> lockGuard(*m_dataLock);
        m_frame = m_tmpFrame;
    }


    emit frameCaptured(&m_frame, m_cap.get(cv::CAP_PROP_POS_FRAMES));
    return true;
}

void CaptureThread::setState(QMediaPlayer::State state)
{
    m_state = state;
    emit stateChanged(m_state);
}

void CaptureThread::play()
{
    setState(QMediaPlayer::PlayingState);
}

void CaptureThread::pause()
{
    setState(QMediaPlayer::PausedState);
}

void CaptureThread::next()
{
    // Pause and then move to the next frame
    if (m_state != QMediaPlayer::PausedState)
        setState(QMediaPlayer::PausedState);
    readNextVideoFrame();
}

void CaptureThread::previous()
{
    // Pause and then move to the next frame
    if (m_state != QMediaPlayer::PausedState)
        setState(QMediaPlayer::PausedState);

    auto prev_frame_num = m_cap.get(cv::CAP_PROP_POS_FRAMES) - 2;

    if (prev_frame_num < 0)
        prev_frame_num = 0;

    m_cap.set(cv::CAP_PROP_POS_FRAMES, prev_frame_num);
    readNextVideoFrame();
}

void CaptureThread::stop()
{
    m_cap.set(cv::CAP_PROP_POS_FRAMES, 0);
    setState(QMediaPlayer::PausedState);
    readNextVideoFrame();
}

void CaptureThread::rateChanged(qreal new_rate)
{
    if (new_rate > 0)
        m_playbackFPS = new_rate;

    m_delayMS = 1/m_playbackFPS * 1000;
}

void CaptureThread::frameChanged(qint64 frame)
{
    auto num_frames = m_cap.get(cv::CAP_PROP_FRAME_COUNT);
    if ((frame < 0) | (frame > num_frames - 1))
        return;

    // Pause and then move to the frame
    if (m_state != QMediaPlayer::PausedState)
        setState(QMediaPlayer::PausedState);
    m_cap.set(cv::CAP_PROP_POS_FRAMES, frame - 1);
    readNextVideoFrame();
}

void CaptureThread::togglePlayPause()
{
    if (m_state == QMediaPlayer::PlayingState)
        pause();
    else if (m_state == QMediaPlayer::PausedState | m_state == QMediaPlayer::StoppedState)
        play();
}

void CaptureThread::startCalcFPS(bool start)
{
    m_fpsCalculating = start;
    m_fpsSum = 0;
    m_curFPSSampleCount = 0;
    m_timeSamples.clear();
}

void CaptureThread::playVideo()
{
    qDebug() << "CaptureThread::playVideo()";
    if (m_cameraID < 0) { // Video playback
        bool haveMoreFrames{true};
        videoPlayback(haveMoreFrames);
    }
    else { // Webcam stream - support for this has been removed
        setState(QMediaPlayer::StoppedState);
    }
}
int CaptureThread::exec()
{
    setState(QMediaPlayer::PlayingState);
    playVideo();
    m_state = QMediaPlayer::StoppedState;

    // Play the video once. Then pause for further instructions
    // in exec loop
    while (m_running) {
        if (m_state == QMediaPlayer::PlayingState) {
            playVideo();
        }

        else {
            msleep(m_delayMS);
        }
    }

    return 0;
}


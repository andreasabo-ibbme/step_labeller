#include "capture_thread.h"
#include <QElapsedTimer>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <mutex>
#include "opencv2/videoio.hpp"

CaptureThread::CaptureThread(int camera, std::mutex *lock, qreal playback_rate):
    running(false), cameraID(camera), videoPath(""), data_lock(lock), playback_fps(playback_rate), fps_calculating(false), fps_sum(0), cur_fps_sample_count(0)
{
    time_samples.resize(fps_samples);
    delay_ms = 1/playback_fps * 1000;
}

CaptureThread::CaptureThread(QString videoPath, std::mutex *lock, qreal playback_rate):
    running(false), cameraID(-1), videoPath(videoPath), data_lock(lock), playback_fps(playback_rate), fps_calculating(false), fps_sum(0), cur_fps_sample_count(0)
{
    time_samples.resize(fps_samples);
    delay_ms = 1/playback_fps * 1000;
}

CaptureThread::~CaptureThread()
{
    m_cap.release();
}

void CaptureThread::run()
{
    qDebug() << "CaptureThread::run()" << videoPath;
    if (cameraID < 0) { // Video playback
        m_cap.open((videoPath.toStdString().c_str()));
    }
    else { // Webcam stream
        m_cap.open(cameraID);
    }
    running = true;

    exec();
    qDebug() << "Done in run";

}

void CaptureThread::videoPlayback(bool& haveMoreFrames)
{
    qDebug() << "videoPlayback()";

    QElapsedTimer timer;

    while (running && haveMoreFrames)
    {
        timer.restart();

        if (m_state == QMediaPlayer::PlayingState){
            haveMoreFrames = readNextVideoFrame();
        }
        auto sleeptime = delay_ms - timer.elapsed();
        sleeptime = sleeptime < 0 ? 0 : sleeptime;

        // Use sleep to pause the capture thread to simulate the correct FPS
        QThread::msleep(static_cast<unsigned long>(sleeptime));
    }
}

void CaptureThread::cameraStream()
{

    qDebug() << "Is openned? "  << m_cap.isOpened();
    while(running) {
        QElapsedTimer timer;
        timer.start();
        m_cap >> tmp_frame;
        if (tmp_frame.empty()) {
            qDebug() << "read frame - nothing";
            break;
        }
        cv::cvtColor(tmp_frame, tmp_frame, cv::COLOR_BGR2RGB);
        data_lock->lock();
        frame = tmp_frame;
        data_lock->unlock();
        emit frameCaptured(&frame, 1);

        if (fps_calculating) {
            int ms = timer.elapsed();
            cur_fps_sample_count++;

            //initial fill up of deque
            if (cur_fps_sample_count > fps_samples){
                fps_sum -= time_samples[0];
                time_samples.pop_front();
            }

            fps_sum += ms;
            time_samples.push_back(ms);

            if (cur_fps_sample_count % fps_emit_every_samples == 0)
            {
                emit fpsChanged(fps_samples / fps_sum * 1000.0);
            }
        }
    }

}

bool CaptureThread::readNextVideoFrame()
{
    m_cap >> tmp_frame;
    if (tmp_frame.empty()) {
        qDebug() << "read frame - nothing";
        return false;
    }

    cv::cvtColor(tmp_frame, tmp_frame, cv::COLOR_BGR2RGB);

    // Only lock when updating the frame shared with the UI thread
    {
        std::lock_guard<std::mutex> lockGuard(*data_lock);
        frame = tmp_frame;
    }


    emit frameCaptured(&frame, m_cap.get(cv::CAP_PROP_POS_FRAMES));
    return true;
}

void CaptureThread::setState(QMediaPlayer::State state)
{
    m_state = state;
    emit stateChanged(m_state);
}

void CaptureThread::play()
{
    qDebug() << "CaptureThread::play()";
//    exec();
    setState(QMediaPlayer::PlayingState);

//    if (m_state == QMediaPlayer::StoppedState) {
//        setState(QMediaPlayer::PlayingState);
//        this->run();
//    }
//    setState(QMediaPlayer::PlayingState);

}

void CaptureThread::pause()
{
    qDebug() << "CaptureThread::pause()";
    setState(QMediaPlayer::PausedState);
}

void CaptureThread::next()
{
    qDebug() << "CaptureThread::next()";
    // Pause and then move to the next frame
    if (m_state != QMediaPlayer::PausedState)
        setState(QMediaPlayer::PausedState);
    readNextVideoFrame();
}

void CaptureThread::previous()
{
    qDebug() << "CaptureThread::previous(): " << m_state;

    // Pause and then move to the next frame
    if (m_state != QMediaPlayer::PausedState)
        setState(QMediaPlayer::PausedState);

    auto prev_frame_num = m_cap.get(cv::CAP_PROP_POS_FRAMES) - 2;
    qDebug() << "CaptureThread::previous(): " << prev_frame_num << "  " << m_state;

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
    qDebug() << "  CaptureThread::rateChanged: " << new_rate;
    if (new_rate > 0)
        playback_fps = new_rate;

    delay_ms = 1/playback_fps * 1000;
}

void CaptureThread::frameChanged(qint32 frame)
{
    auto num_frames = m_cap.get(cv::CAP_PROP_FRAME_COUNT);
    if (frame < 0 | frame > num_frames - 1)
        return;

    // Pause and then move to the frame
    if (m_state != QMediaPlayer::PausedState)
        setState(QMediaPlayer::PausedState);
    m_cap.set(cv::CAP_PROP_POS_FRAMES, frame-1);
    readNextVideoFrame();

}

void CaptureThread::togglePlayPause()
{
    if (m_state == QMediaPlayer::PlayingState)
        pause();
    else if (m_state == QMediaPlayer::PausedState)
        play();
    // TODO: How do we handle stopped state?
}

void CaptureThread::startCalcFPS(bool start)
{
    fps_calculating = start;
    fps_sum = 0;
    cur_fps_sample_count = 0;
    time_samples.clear();
}

void CaptureThread::playVideo()
{
    qDebug() << "CaptureThread::playVideo()";
    if (cameraID < 0)
    { // Video playback
        bool haveMoreFrames{true};
        videoPlayback(haveMoreFrames);
    }
    else { // Webcam stream
        cameraStream();
    }
}
int CaptureThread::exec()
{
    setState(QMediaPlayer::PlayingState);
    playVideo();
    m_state = QMediaPlayer::StoppedState;
    qDebug() << "Done in exec";

    // Play the video once. Then pause for further instructions
    // in exec loop
    while (running) {
        if (m_state == QMediaPlayer::PlayingState)
        {
            playVideo();
        }

        else {
            msleep(delay_ms);
        }
    }

    return 0;
}


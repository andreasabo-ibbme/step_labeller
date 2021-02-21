#include "capture_thread.h"
#include <QElapsedTimer>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <mutex>
#include "opencv2/videoio.hpp"

CaptureThread::CaptureThread(int camera, std::mutex *lock, PlayerControls* controls):
    running(false), cameraID(camera), videoPath(""), data_lock(lock), m_playercontrols(controls), fps_calculating(false), fps_sum(0), cur_fps_sample_count(0)
{
    time_samples.resize(fps_samples);

}

CaptureThread::CaptureThread(QString videoPath, std::mutex *lock, PlayerControls* controls):
    running(false), cameraID(-1), videoPath(videoPath), data_lock(lock), m_playercontrols(controls), fps_calculating(false), fps_sum(0), cur_fps_sample_count(0)
{
    time_samples.resize(fps_samples);

}

CaptureThread::~CaptureThread()
{
    cap.release();
}

void CaptureThread::run()
{
    qDebug() << "CaptureThread::run()" << videoPath;

    running = true;

    if (cameraID < 0) { // Video playback
        cap.open((videoPath.toStdString().c_str()));
        bool haveMoreFrames{true};
        videoPlayback(haveMoreFrames);
    }
    else { // Webcam stream
        cap.open(cameraID);
        cameraStream();
    }

    cap.release();
    running = false;
}

void CaptureThread::videoPlayback(bool& haveMoreFrames)
{
    qDebug() << "videoPlayback";

    QElapsedTimer timer;

    while (running && haveMoreFrames)
    {
        timer.restart();
        haveMoreFrames = readNextVideoFrame();
        auto sleeptime = delay_ms - timer.elapsed();
        sleeptime = sleeptime < 0 ? 0 : sleeptime;

        // Use sleep to pause the capture thread to simulate the correct FPS
        QThread::msleep(static_cast<unsigned long>(sleeptime));
    }
}

void CaptureThread::cameraStream()
{

    qDebug() << "Is openned? "  << cap.isOpened();
    while(running) {
        QElapsedTimer timer;
        timer.start();
        cap >> tmp_frame;
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
    cap >> tmp_frame;
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


    emit frameCaptured(&frame, cap.get(cv::CAP_PROP_POS_FRAMES));
    return true;
}

void CaptureThread::play()
{
    qDebug() << "CaptureThread::play()";
}

void CaptureThread::startCalcFPS(bool start)
{
    fps_calculating = start;
    fps_sum = 0;
    cur_fps_sample_count = 0;
    time_samples.clear();
}


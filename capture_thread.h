#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QMutex>
#include <QString>
#include <QThread>
#include <deque>
#include <mutex>

#include "opencv2/world.hpp"
#include "opencv2/opencv.hpp"

class PlayerControls;

class CaptureThread : public QThread
{
    Q_OBJECT

public:
    CaptureThread(QString videoPath, std::mutex *lock, PlayerControls* controls);
    CaptureThread(int camera, std::mutex *lock, PlayerControls* controls);

    ~CaptureThread();
    void setRunning(bool run) {running = run; }
    void startCalcFPS(bool start);

private:
    void run() override;
    void videoPlayback(bool& haveMoreFrames);
    void cameraStream();
    bool readNextVideoFrame();

 public slots:
    void play();
//    void pause();
//    void stop();
//    void next();
//    void previous();


signals:
    void frameCaptured(cv::Mat *data, qint64 frameNum);
    void fpsChanged(float fps);

private:
    bool running;
    int cameraID;
    QString videoPath;
    std::mutex *data_lock;
    cv::Mat frame;
    float playback_fps = 10;
    float delay_ms = 1/playback_fps * 1000;

    // OpenCV
    cv::VideoCapture cap;
    cv::Mat tmp_frame;

    // Playback controls
    PlayerControls* m_playercontrols;

    // FPS calculating
    bool fps_calculating;
    static const int fps_samples = 100;
    static const int fps_emit_every_samples = 20;
    std::deque<float> time_samples;
    float fps_sum;
    int cur_fps_sample_count;
};

#endif // CAPTURETHREAD_H

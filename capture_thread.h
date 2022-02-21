#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <mutex>
#include <deque>

#include <QMediaPlayer>
#include <QMutex>
#include <QString>
#include <QThread>

#include "opencv2/world.hpp"
#include "opencv2/opencv.hpp"


class PlayerControls;

class CaptureThread : public QThread
{
    Q_OBJECT

public:
    CaptureThread(QString videoPath, std::mutex *lock, qreal playback_rate = 30);
    CaptureThread(int camera, std::mutex *lock, qreal playback_rate = 30);

    ~CaptureThread();
    void setRunning(bool run) {running = run; }
    void startCalcFPS(bool start);

protected:
    int exec();

private:
    void run() override;

    void videoPlayback(bool& haveMoreFrames);
    bool readNextVideoFrame();
    void setState(QMediaPlayer::State state);
    void playVideo();

 public slots:
    void play();
    void pause();
    void next();
    void previous();
    void stop();
    void rateChanged(qreal new_rate);
    void frameChanged(qint64 frame);
    void togglePlayPause();


signals:
    void frameCaptured(cv::Mat *data, qint64 frameNum);
    void fpsChanged(float fps);
    void stateChanged(QMediaPlayer::State state);

private:
    bool running;
    int cameraID;
    QString videoPath;
    std::mutex *data_lock;
    cv::Mat frame;
    float playback_fps;
    float delay_ms;

    // OpenCV
    cv::VideoCapture m_cap;
    cv::Mat tmp_frame;

    // Playback controls
    QMediaPlayer::State m_state = QMediaPlayer::StoppedState;


    // FPS calculating
    bool fps_calculating;
    static const int fps_samples = 100;
    static const int fps_emit_every_samples = 20;
    std::deque<float> time_samples;
    float fps_sum;
    int cur_fps_sample_count;
};

#endif // CAPTURETHREAD_H

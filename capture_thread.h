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
    CaptureThread(QString m_videoPath, std::mutex *lock, qreal playback_rate = 30);
    CaptureThread(int camera, std::mutex *lock, qreal playback_rate = 30);

    ~CaptureThread();
    void setRunning(bool run) {m_running = run; }
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
    bool m_running;
    int m_cameraID;
    QString m_videoPath;
    std::mutex *m_dataLock;
    cv::Mat m_frame;
    float m_playbackFPS;
    float m_delayMS;

    // OpenCV
    cv::VideoCapture m_cap;
    cv::Mat m_tmpFrame;

    // Playback controls
    QMediaPlayer::State m_state = QMediaPlayer::StoppedState;


    // FPS calculating
    bool m_fpsCalculating;
    static const int m_fpsSamples = 100;
    std::deque<float> m_timeSamples;
    float m_fpsSum;
    int m_curFPSSampleCount;
};

#endif // CAPTURETHREAD_H

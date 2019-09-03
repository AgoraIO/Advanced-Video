#ifndef VIDEORENDER_H
#define VIDEORENDER_H
#include <QWidget>
#include <QtQuickWidgets/QQuickWidget>
#include <QTimer>
#include <QCloseEvent>
class AgoraRtcEngine;
namespace Ui {
class videoRender;

}

class videoRender : public QWidget
{
    Q_OBJECT

public:
    explicit videoRender(QWidget *parent = 0);
    ~videoRender();
    QQuickWidget* videoPlayerWidget;
	AgoraRtcEngine *m_engine;
	void setRemoteVideo(unsigned int uid);
	void setLocalVideo();
private slots:
    void on_pauseBtn_clicked();


    void on_openVideoFile_clicked();

    void on_quitBtn_clicked();

    void on_video_progress_sliderReleased();

    void timeUpdate();

    void on_audioMix_clicked();

    void on_pushStream_clicked(bool checked);
	

	
private:
    Ui::videoRender *ui;
    QTimer *timer;
protected:
	void closeEvent(QCloseEvent *event);
};

#endif // VIDEORENDER_H

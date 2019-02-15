#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <IAgoraRtcEngine.h>
#include "QTVideoKit.h"
#include "videorender.h"

class AgoraRtcEngine;
class QQuickWidget;
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Q_INVOKABLE void openDeviceSettings();
    Q_INVOKABLE void playVideo();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void seekTo(int msc);
    Q_INVOKABLE int  getCurrentPos();

public slots:
    void joiningChannel();
    void leavingChannel();
    void firstRemoteVideoFrameDrawn(unsigned int uid, int width, int height, int elapsed);
private:
    QQuickWidget* m_contentView;
	videoRender der;
    std::unique_ptr<AgoraRtcEngine> m_engine;
};

#endif // MAINWINDOW_H

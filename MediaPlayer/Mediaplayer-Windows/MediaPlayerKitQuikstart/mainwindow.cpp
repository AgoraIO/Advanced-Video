#include "mainwindow.h"
#include "agorartcengine.h"
#include <QtQuickWidgets/QQuickWidget>
#include <QQmlContext>
#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include "iostream"
#include <QFileDialog>
#include <QTextCodec>
using namespace std;
static AgoraRtcEngine* engine;
MainWindow::MainWindow(QWidget *parent)
    :QWidget(parent)
    ,m_engine(new AgoraRtcEngine(this))
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    resize(600, 600);

    engine = m_engine.get();
	der.m_engine = engine;
    connect(engine, SIGNAL(joiningChannel()), this, SLOT(joiningChannel()));
    connect(engine, SIGNAL(leavingChannel()), this, SLOT(leavingChannel()));
    connect(engine,SIGNAL(firstRemoteVideoDecoded(uint,int,int,int)), this, SLOT(firstRemoteVideoFrameDrawn(uint,int,int,int)));
    m_contentView = new QQuickWidget(this);
    m_contentView->rootContext()->setContextProperty("agoraRtcEngine", engine);
    m_contentView->rootContext()->setContextProperty("containerWindow", this);
    m_contentView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_contentView->setSource(QUrl("qrc:///main.qml"));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_contentView);
    setLayout(layout);
	der.setLocalVideo();

}

MainWindow::~MainWindow()
{
	delete m_contentView;
}
void MainWindow::pause(){

}

void MainWindow::joiningChannel()
{


    der.show();


//    QRect rec = QApplication::desktop()->screenGeometry();
//    int w = rec.width()/2;
//    int h = rec.height()/2;
//    resize(w, h);



}
void MainWindow::playVideo()
{

}

void MainWindow::leavingChannel()
{

    resize(600, 600);

}
void MainWindow::seekTo(int msc){

}

void MainWindow::openDeviceSettings()
{
    QQuickWidget* form = new QQuickWidget(this);
    form->setAttribute(Qt::WA_DeleteOnClose);
    form->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    form->setFixedSize(512, 640);
    form->rootContext()->setContextProperty("agoraRtcEngine", m_engine.get());
    form->rootContext()->setContextProperty("mainWindow", form);
    form->setResizeMode(QQuickWidget::SizeRootObjectToView);
    form->setSource(QUrl("qrc:///DeviceSettings.qml"));
    form->show();

}
int MainWindow::getCurrentPos(){
	return 0;
}
void MainWindow::firstRemoteVideoFrameDrawn(unsigned int uid, int width, int height, int elapsed)
{
	der.setRemoteVideo(uid);
	
}

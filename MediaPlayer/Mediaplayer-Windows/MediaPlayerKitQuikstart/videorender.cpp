#include "videorender.h"
#include "ui_videorender.h"
#include "QTVideoKit.h"
#include <QTextCodec>
#include <QFileDialog>
#include "agorartcengine.h"

static bool isAudioMix = false;
static bool isPublish = false;
videoRender::videoRender(QWidget *parent) :
    QWidget(parent), 
    ui(new Ui::videoRender)
{

    ui->setupUi(this);
	m_engine = NULL;
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeUpdate()));
	timer->start(1000);

}

videoRender::~videoRender()
{

    delete ui;
	delete timer;
}

void videoRender::on_pauseBtn_clicked()
{
    //调整播放暂停 目前是一个接口
    QTVideoKit::Get()->pause();

}

void videoRender::on_openVideoFile_clicked()
{
	RECT rctA;
	::GetWindowRect((HWND)ui->localView->winId(), &rctA);
	int width = rctA.right - rctA.left;
	int height = rctA.bottom - rctA.top;
	//初始化窗口的大小
	QTVideoKit::Get()->setPreview((void *)ui->localView->winId());
    QTextCodec* codec =QTextCodec::codecForName("GB2312");
    QTextCodec::setCodecForLocale(codec);
    QString fileName = QFileDialog::getOpenFileName(
                 this, tr("open video file"),
                 "./", tr("Image files(*.mp4 *.avi *.mpng *.rmvb *.3gp *.ogg *.mp3 *.mkv);;All files (*.*)"));
    if (!fileName.isEmpty())
    {
        ui->localView->setVisible(true);
        //打开播放文件 最好是英文路径 英文名字的文件 中文名的文件有特殊字符 有一定概率识别错误
        QTVideoKit::Get()->loadVideo(fileName,true);
    };
}

void videoRender::on_quitBtn_clicked()
{
    //退出当前播放视频
    ui->localView->setVisible(false);
    QTVideoKit::Get()->unload();

}


void videoRender::on_video_progress_sliderReleased()
{
    //调整播放进度
    QTVideoKit::Get()->seekTo(ui->video_progress->value());
}
void videoRender::setRemoteVideo(unsigned int uid)
{
    //设置远端画面
    m_engine->setupRemoteVideo(uid,(void *)ui->remoteView->winId());

}
void videoRender::setLocalVideo(){
    //设置自己预览画面
	m_engine->setupLocalVideo((void *)ui->localView->winId());
}
void videoRender::timeUpdate(){
    //获取当前的进度
	int msec = QTVideoKit::Get()->getCurrentPos();
	ui->video_progress->setValue(msec);
}
void videoRender::closeEvent(QCloseEvent *event){

	QTVideoKit::Get()->unload();

}

void videoRender::on_audioMix_clicked()
{
   if (isAudioMix)
   {
	   ui->audioMix->setText("play");

   }
   else {
	   ui->audioMix->setText("stop");

   }
   QTVideoKit::Get()->setAudioMix(isAudioMix);
   isAudioMix = !isAudioMix;
}

void videoRender::on_pushStream_clicked(bool checked)
{	

	if (isPublish)
	{
		QTVideoKit::Get()->unPublish();
		ui->pushStream->setText("publish");
		checked = false;
	}
	else
	{
		QTVideoKit::Get()->publish();
		ui->pushStream->setText("unPublish");
		checked = true;
	}
	isPublish = !isPublish;

}

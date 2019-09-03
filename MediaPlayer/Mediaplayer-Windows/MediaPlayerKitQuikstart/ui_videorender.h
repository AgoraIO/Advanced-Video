/********************************************************************************
** Form generated from reading UI file 'videorender.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIDEORENDER_H
#define UI_VIDEORENDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_videoRender
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QOpenGLWidget *localView;
    QOpenGLWidget *remoteView;
    QVBoxLayout *verticalLayout_3;
    QSlider *video_progress;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *openVideoFile;
    QPushButton *pushStream;
    QPushButton *pauseBtn;
    QPushButton *audioMix;
    QPushButton *quitBtn;

    void setupUi(QWidget *videoRender)
    {
        if (videoRender->objectName().isEmpty())
            videoRender->setObjectName(QStringLiteral("videoRender"));
        videoRender->resize(1280, 720);
        videoRender->setLayoutDirection(Qt::LeftToRight);
        verticalLayout_2 = new QVBoxLayout(videoRender);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        localView = new QOpenGLWidget(videoRender);
        localView->setObjectName(QStringLiteral("localView"));

        horizontalLayout_3->addWidget(localView);

        remoteView = new QOpenGLWidget(videoRender);
        remoteView->setObjectName(QStringLiteral("remoteView"));

        horizontalLayout_3->addWidget(remoteView);


        verticalLayout->addLayout(horizontalLayout_3);


        verticalLayout_2->addLayout(verticalLayout);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        video_progress = new QSlider(videoRender);
        video_progress->setObjectName(QStringLiteral("video_progress"));
        video_progress->setMaximum(1000);
        video_progress->setOrientation(Qt::Horizontal);

        verticalLayout_3->addWidget(video_progress);


        verticalLayout_2->addLayout(verticalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setSizeConstraint(QLayout::SetMaximumSize);
        horizontalLayout_2->setContentsMargins(-1, 0, -1, -1);
        openVideoFile = new QPushButton(videoRender);
        openVideoFile->setObjectName(QStringLiteral("openVideoFile"));

        horizontalLayout_2->addWidget(openVideoFile);

        pushStream = new QPushButton(videoRender);
        pushStream->setObjectName(QStringLiteral("pushStream"));

        horizontalLayout_2->addWidget(pushStream);

        pauseBtn = new QPushButton(videoRender);
        pauseBtn->setObjectName(QStringLiteral("pauseBtn"));

        horizontalLayout_2->addWidget(pauseBtn);

        audioMix = new QPushButton(videoRender);
        audioMix->setObjectName(QStringLiteral("audioMix"));

        horizontalLayout_2->addWidget(audioMix);

        quitBtn = new QPushButton(videoRender);
        quitBtn->setObjectName(QStringLiteral("quitBtn"));

        horizontalLayout_2->addWidget(quitBtn);


        verticalLayout_2->addLayout(horizontalLayout_2);


        retranslateUi(videoRender);

        QMetaObject::connectSlotsByName(videoRender);
    } // setupUi

    void retranslateUi(QWidget *videoRender)
    {
        videoRender->setWindowTitle(QApplication::translate("videoRender", "AgoraVideoCall", Q_NULLPTR));
        openVideoFile->setText(QApplication::translate("videoRender", "Open", Q_NULLPTR));
        pushStream->setText(QApplication::translate("videoRender", "publish", Q_NULLPTR));
        pauseBtn->setText(QApplication::translate("videoRender", "Play", Q_NULLPTR));
        audioMix->setText(QApplication::translate("videoRender", "isAudioMix", Q_NULLPTR));
        quitBtn->setText(QApplication::translate("videoRender", "Quit", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class videoRender: public Ui_videoRender {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIDEORENDER_H

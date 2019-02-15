TEMPLATE = app

QT += core quickwidgets gui
CONFIG += c++11

SOURCES += main.cpp \
    agorartcengine.cpp \
    mainwindow.cpp \
    QTVideoKit.cpp             \
    videorender.cpp          \
    #引入videokit文件夹下的cpp文件
    $$PWD/Agora_Video_Kit/videokit/ExtendAudioFrameObserver.cpp \
    $$PWD/Agora_Video_Kit/videokit/videoinfocallback.cpp \
    $$PWD/Agora_Video_kit/videokit/videokit.cpp
    $$PWD/Agora_Video_kit/videokit/ZD3DRender.cpp


RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)


HEADERS += agorartcengine.h \
    mainwindow.h \
    QTVideoKit.h \
    videorender.h \



#添加头文件
INCLUDEPATH += $$PWD/Agora_Video_Kit/include \
               $$PWD/Agora_Video_Kit/videokit \
               $$PWD/sdk/include


#添加库文件
win32: LIBS += -L$$PWD/Agora_Video_Kit/lib/  -lre_sampler -lVideoPlayerKit  \
               -L$$PWD/sdk/lib/  -lagora_rtc_sdk \

FORMS += \
    videorender.ui








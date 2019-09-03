TEMPLATE = app

QT += core quickwidgets gui
CONFIG += c++11

SOURCES += main.cpp \
    agorartcengine.cpp \
    mainwindow.cpp \
    QTVideoKit.cpp             \
    videorender.cpp          \
    #引入videokit文件夹下的cpp文件
    $$PWD/Agora_native_sdk/videokit/ExtendAudioFrameObserver.cpp \
    $$PWD/Agora_native_sdk/videokit/videoinfocallback.cpp \
    $$PWD/Agora_native_sdk/videokit/videokit.cpp
    $$PWD/Agora_native_sdk/videokit/ZD3DRender.cpp



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
INCLUDEPATH += $$PWD/Agora_native_sdk/include \
               $$PWD/Agora_native_sdk/videokit


#添加库文件
win32: LIBS += -L$$PWD/Agora_native_sdk/lib/ -lagora_rtc_sdk -lre_sampler -lVideoPlayerKit

FORMS += \
    videorender.ui








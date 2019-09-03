/****************************************************************************
** Meta object code from reading C++ file 'videorender.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../videorender.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'videorender.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_videoRender_t {
    QByteArrayData data[10];
    char stringdata0[171];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_videoRender_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_videoRender_t qt_meta_stringdata_videoRender = {
    {
QT_MOC_LITERAL(0, 0, 11), // "videoRender"
QT_MOC_LITERAL(1, 12, 19), // "on_pauseBtn_clicked"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 24), // "on_openVideoFile_clicked"
QT_MOC_LITERAL(4, 58, 18), // "on_quitBtn_clicked"
QT_MOC_LITERAL(5, 77, 32), // "on_video_progress_sliderReleased"
QT_MOC_LITERAL(6, 110, 10), // "timeUpdate"
QT_MOC_LITERAL(7, 121, 19), // "on_audioMix_clicked"
QT_MOC_LITERAL(8, 141, 21), // "on_pushStream_clicked"
QT_MOC_LITERAL(9, 163, 7) // "checked"

    },
    "videoRender\0on_pauseBtn_clicked\0\0"
    "on_openVideoFile_clicked\0on_quitBtn_clicked\0"
    "on_video_progress_sliderReleased\0"
    "timeUpdate\0on_audioMix_clicked\0"
    "on_pushStream_clicked\0checked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_videoRender[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x08 /* Private */,
       3,    0,   50,    2, 0x08 /* Private */,
       4,    0,   51,    2, 0x08 /* Private */,
       5,    0,   52,    2, 0x08 /* Private */,
       6,    0,   53,    2, 0x08 /* Private */,
       7,    0,   54,    2, 0x08 /* Private */,
       8,    1,   55,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    9,

       0        // eod
};

void videoRender::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        videoRender *_t = static_cast<videoRender *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_pauseBtn_clicked(); break;
        case 1: _t->on_openVideoFile_clicked(); break;
        case 2: _t->on_quitBtn_clicked(); break;
        case 3: _t->on_video_progress_sliderReleased(); break;
        case 4: _t->timeUpdate(); break;
        case 5: _t->on_audioMix_clicked(); break;
        case 6: _t->on_pushStream_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject videoRender::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_videoRender.data,
      qt_meta_data_videoRender,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *videoRender::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *videoRender::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_videoRender.stringdata0))
        return static_cast<void*>(const_cast< videoRender*>(this));
    return QWidget::qt_metacast(_clname);
}

int videoRender::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

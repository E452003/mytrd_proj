/****************************************************************************
** Meta object code from reading C++ file 'winner_app.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../winner_app.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'winner_app.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_WinnerApp_t {
    QByteArrayData data[12];
    char stringdata[146];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_WinnerApp_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_WinnerApp_t qt_meta_stringdata_WinnerApp = {
    {
QT_MOC_LITERAL(0, 0, 9),
QT_MOC_LITERAL(1, 10, 17),
QT_MOC_LITERAL(2, 28, 0),
QT_MOC_LITERAL(3, 29, 13),
QT_MOC_LITERAL(4, 43, 12),
QT_MOC_LITERAL(5, 56, 5),
QT_MOC_LITERAL(6, 62, 13),
QT_MOC_LITERAL(7, 76, 9),
QT_MOC_LITERAL(8, 86, 12),
QT_MOC_LITERAL(9, 99, 22),
QT_MOC_LITERAL(10, 122, 13),
QT_MOC_LITERAL(11, 136, 8)
    },
    "WinnerApp\0SigTaskStatChange\0\0StrategyTask*\0"
    "SigAppendLog\0char*\0SigRemoveTask\0"
    "SigShowUi\0std::string*\0DoStrategyTasksTimeout\0"
    "DoNormalTimer\0DoShowUi\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WinnerApp[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   49,    2, 0x06,
       4,    1,   54,    2, 0x06,
       6,    1,   57,    2, 0x06,
       7,    1,   60,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       9,    0,   63,    2, 0x08,
      10,    0,   64,    2, 0x08,
      11,    1,   65,    2, 0x08,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    2,    2,
    QMetaType::Void, 0x80000000 | 5,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 8,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    2,

       0        // eod
};

void WinnerApp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WinnerApp *_t = static_cast<WinnerApp *>(_o);
        switch (_id) {
        case 0: _t->SigTaskStatChange((*reinterpret_cast< StrategyTask*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->SigAppendLog((*reinterpret_cast< char*(*)>(_a[1]))); break;
        case 2: _t->SigRemoveTask((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->SigShowUi((*reinterpret_cast< std::string*(*)>(_a[1]))); break;
        case 4: _t->DoStrategyTasksTimeout(); break;
        case 5: _t->DoNormalTimer(); break;
        case 6: _t->DoShowUi((*reinterpret_cast< std::string*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (WinnerApp::*_t)(StrategyTask * , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WinnerApp::SigTaskStatChange)) {
                *result = 0;
            }
        }
        {
            typedef void (WinnerApp::*_t)(char * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WinnerApp::SigAppendLog)) {
                *result = 1;
            }
        }
        {
            typedef void (WinnerApp::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WinnerApp::SigRemoveTask)) {
                *result = 2;
            }
        }
        {
            typedef void (WinnerApp::*_t)(std::string * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WinnerApp::SigShowUi)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject WinnerApp::staticMetaObject = {
    { &QApplication::staticMetaObject, qt_meta_stringdata_WinnerApp.data,
      qt_meta_data_WinnerApp,  qt_static_metacall, 0, 0}
};


const QMetaObject *WinnerApp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WinnerApp::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WinnerApp.stringdata))
        return static_cast<void*>(const_cast< WinnerApp*>(this));
    if (!strcmp(_clname, "TSystem::ServerClientAppBase"))
        return static_cast< TSystem::ServerClientAppBase*>(const_cast< WinnerApp*>(this));
    return QApplication::qt_metacast(_clname);
}

int WinnerApp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QApplication::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void WinnerApp::SigTaskStatChange(StrategyTask * _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WinnerApp::SigAppendLog(char * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WinnerApp::SigRemoveTask(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void WinnerApp::SigShowUi(std::string * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE

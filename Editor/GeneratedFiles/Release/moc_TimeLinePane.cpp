/****************************************************************************
** Meta object code from reading C++ file 'TimeLinePane.h'
**
** Created: Thu Aug 6 11:16:31 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../TimeLinePane.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TimeLinePane.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TimeLineBar[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_TimeLineBar[] = {
    "TimeLineBar\0"
};

void TimeLineBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData TimeLineBar::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TimeLineBar::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TimeLineBar,
      qt_meta_data_TimeLineBar, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TimeLineBar::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TimeLineBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TimeLineBar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TimeLineBar))
        return static_cast<void*>(const_cast< TimeLineBar*>(this));
    return QWidget::qt_metacast(_clname);
}

int TimeLineBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ChannelBars[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x09,
      22,   12,   12,   12, 0x09,
      32,   12,   12,   12, 0x09,
      42,   12,   12,   12, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ChannelBars[] = {
    "ChannelBars\0\0onCopy()\0onPaste()\0"
    "onShift()\0onReverse()\0"
};

void ChannelBars::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChannelBars *_t = static_cast<ChannelBars *>(_o);
        switch (_id) {
        case 0: _t->onCopy(); break;
        case 1: _t->onPaste(); break;
        case 2: _t->onShift(); break;
        case 3: _t->onReverse(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ChannelBars::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ChannelBars::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ChannelBars,
      qt_meta_data_ChannelBars, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ChannelBars::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ChannelBars::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ChannelBars::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChannelBars))
        return static_cast<void*>(const_cast< ChannelBars*>(this));
    return QWidget::qt_metacast(_clname);
}

int ChannelBars::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
static const uint qt_meta_data_TimeLinePane[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   14,   13,   13, 0x05,
      50,   44,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      68,   13,   13,   13, 0x0a,
      77,   13,   13,   13, 0x0a,
      86,   13,   13,   13, 0x0a,
     101,   97,   13,   13, 0x09,
     121,   13,   13,   13, 0x09,
     149,   13,   13,   13, 0x09,
     171,   13,   13,   13, 0x09,
     184,   13,   13,   13, 0x09,
     210,   13,   13,   13, 0x09,
     221,   13,   13,   13, 0x09,
     232,   44,   13,   13, 0x09,
     253,   13,   13,   13, 0x09,
     268,   13,   13,   13, 0x09,
     284,   13,   13,   13, 0x09,
     308,   13,   13,   13, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_TimeLinePane[] = {
    "TimeLinePane\0\0min,max\0rangeChanged(int,int)\0"
    "value\0valueChanged(int)\0onPlay()\0"
    "onStop()\0onSetKey()\0val\0hScrollChanged(int)\0"
    "animationSelectionChanged()\0"
    "timeLineTimeChanged()\0timeEdited()\0"
    "channelSelectionChanged()\0onUpdate()\0"
    "onDelete()\0timeLineSpacing(int)\0"
    "onTimeZoomIn()\0onTimeZoomOut()\0"
    "guideCheckChacnged(int)\0guideEdited()\0"
};

void TimeLinePane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TimeLinePane *_t = static_cast<TimeLinePane *>(_o);
        switch (_id) {
        case 0: _t->rangeChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->onPlay(); break;
        case 3: _t->onStop(); break;
        case 4: _t->onSetKey(); break;
        case 5: _t->hScrollChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->animationSelectionChanged(); break;
        case 7: _t->timeLineTimeChanged(); break;
        case 8: _t->timeEdited(); break;
        case 9: _t->channelSelectionChanged(); break;
        case 10: _t->onUpdate(); break;
        case 11: _t->onDelete(); break;
        case 12: _t->timeLineSpacing((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->onTimeZoomIn(); break;
        case 14: _t->onTimeZoomOut(); break;
        case 15: _t->guideCheckChacnged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->guideEdited(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData TimeLinePane::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TimeLinePane::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TimeLinePane,
      qt_meta_data_TimeLinePane, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TimeLinePane::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TimeLinePane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TimeLinePane::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TimeLinePane))
        return static_cast<void*>(const_cast< TimeLinePane*>(this));
    return QWidget::qt_metacast(_clname);
}

int TimeLinePane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void TimeLinePane::rangeChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TimeLinePane::valueChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'Command.h'
**
** Created: Thu Aug 6 11:16:39 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../Command.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Command.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Command[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      23,       // signalCount

 // signals: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x05,
      33,    8,    8,    8, 0x05,
      52,    8,    8,    8, 0x05,
      67,    8,    8,    8, 0x05,
      83,    8,    8,    8, 0x05,
     108,    8,    8,    8, 0x05,
     128,    8,    8,    8, 0x05,
     147,    8,    8,    8, 0x05,
     175,    8,    8,    8, 0x05,
     198,    8,    8,    8, 0x05,
     216,    8,    8,    8, 0x05,
     258,  238,    8,    8, 0x05,
     289,    8,    8,    8, 0x05,
     309,    8,    8,    8, 0x05,
     334,    8,    8,    8, 0x05,
     357,    8,    8,    8, 0x05,
     376,    8,    8,    8, 0x05,
     397,    8,    8,    8, 0x05,
     423,    8,    8,    8, 0x05,
     462,  440,    8,    8, 0x05,
     528,    8,    8,    8, 0x05,
     546,    8,    8,    8, 0x05,
     568,    8,    8,    8, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Command[] = {
    "Command\0\0sceneSelectionChanged()\0"
    "sceneListChanged()\0sceneChanged()\0"
    "objectChanged()\0objectSelectionChanged()\0"
    "objectListChanged()\0appObjectChanged()\0"
    "appObjectSelectionChanged()\0"
    "appObjectListChanged()\0listenerChanged()\0"
    "listenerListChanged()\0oldAction,newAction\0"
    "actionChanged(Action*,Action*)\0"
    "actionListChanged()\0actionSelectionChanged()\0"
    "animationListChanged()\0animationChanged()\0"
    "channelListChanged()\0channelSelectionChanged()\0"
    "channelChanged()\0oldKeyFrames,newTimes\0"
    "keyFrameTimeChanged(std::vector<KeyFrameData>,std::vector<float>)\0"
    "keyFrameDeleted()\0resourceListChanged()\0"
    "channelTemplateListChanged()\0"
};

void Command::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Command *_t = static_cast<Command *>(_o);
        switch (_id) {
        case 0: _t->sceneSelectionChanged(); break;
        case 1: _t->sceneListChanged(); break;
        case 2: _t->sceneChanged(); break;
        case 3: _t->objectChanged(); break;
        case 4: _t->objectSelectionChanged(); break;
        case 5: _t->objectListChanged(); break;
        case 6: _t->appObjectChanged(); break;
        case 7: _t->appObjectSelectionChanged(); break;
        case 8: _t->appObjectListChanged(); break;
        case 9: _t->listenerChanged(); break;
        case 10: _t->listenerListChanged(); break;
        case 11: _t->actionChanged((*reinterpret_cast< Action*(*)>(_a[1])),(*reinterpret_cast< Action*(*)>(_a[2]))); break;
        case 12: _t->actionListChanged(); break;
        case 13: _t->actionSelectionChanged(); break;
        case 14: _t->animationListChanged(); break;
        case 15: _t->animationChanged(); break;
        case 16: _t->channelListChanged(); break;
        case 17: _t->channelSelectionChanged(); break;
        case 18: _t->channelChanged(); break;
        case 19: _t->keyFrameTimeChanged((*reinterpret_cast< const std::vector<KeyFrameData>(*)>(_a[1])),(*reinterpret_cast< const std::vector<float>(*)>(_a[2]))); break;
        case 20: _t->keyFrameDeleted(); break;
        case 21: _t->resourceListChanged(); break;
        case 22: _t->channelTemplateListChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Command::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Command::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Command,
      qt_meta_data_Command, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Command::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Command::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Command::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Command))
        return static_cast<void*>(const_cast< Command*>(this));
    return QObject::qt_metacast(_clname);
}

int Command::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    }
    return _id;
}

// SIGNAL 0
void Command::sceneSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Command::sceneListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Command::sceneChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Command::objectChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Command::objectSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void Command::objectListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void Command::appObjectChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void Command::appObjectSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void Command::appObjectListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void Command::listenerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void Command::listenerListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void Command::actionChanged(Action * _t1, Action * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void Command::actionListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}

// SIGNAL 13
void Command::actionSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, 0);
}

// SIGNAL 14
void Command::animationListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 14, 0);
}

// SIGNAL 15
void Command::animationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}

// SIGNAL 16
void Command::channelListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, 0);
}

// SIGNAL 17
void Command::channelSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 17, 0);
}

// SIGNAL 18
void Command::channelChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 18, 0);
}

// SIGNAL 19
void Command::keyFrameTimeChanged(const std::vector<KeyFrameData> & _t1, const std::vector<float> & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}

// SIGNAL 20
void Command::keyFrameDeleted()
{
    QMetaObject::activate(this, &staticMetaObject, 20, 0);
}

// SIGNAL 21
void Command::resourceListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 21, 0);
}

// SIGNAL 22
void Command::channelTemplateListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 22, 0);
}
QT_END_MOC_NAMESPACE

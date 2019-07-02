/****************************************************************************
** Meta object code from reading C++ file 'CommandHistory.h'
**
** Created: Thu Aug 6 11:16:39 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../CommandHistory.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CommandHistory.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CommandHistory[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      31,   15,   15,   15, 0x05,
      47,   15,   15,   15, 0x05,
      71,   15,   15,   15, 0x05,
      94,   15,   15,   15, 0x05,
     113,   15,   15,   15, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_CommandHistory[] = {
    "CommandHistory\0\0sceneChanged()\0"
    "objectChanged()\0commandHistoryChanged()\0"
    "animationListChanged()\0animationChanged()\0"
    "channelListChanged()\0"
};

void CommandHistory::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CommandHistory *_t = static_cast<CommandHistory *>(_o);
        switch (_id) {
        case 0: _t->sceneChanged(); break;
        case 1: _t->objectChanged(); break;
        case 2: _t->commandHistoryChanged(); break;
        case 3: _t->animationListChanged(); break;
        case 4: _t->animationChanged(); break;
        case 5: _t->channelListChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData CommandHistory::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CommandHistory::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CommandHistory,
      qt_meta_data_CommandHistory, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CommandHistory::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CommandHistory::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CommandHistory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CommandHistory))
        return static_cast<void*>(const_cast< CommandHistory*>(this));
    return QObject::qt_metacast(_clname);
}

int CommandHistory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void CommandHistory::sceneChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void CommandHistory::objectChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void CommandHistory::commandHistoryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void CommandHistory::animationListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void CommandHistory::animationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void CommandHistory::channelListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}
QT_END_MOC_NAMESPACE

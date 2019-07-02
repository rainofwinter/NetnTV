/****************************************************************************
** Meta object code from reading C++ file 'Tool.h'
**
** Created: Thu Aug 6 11:16:31 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../Tool.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Tool.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Tool[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
       6,    5,    5,    5, 0x05,
      22,    5,    5,    5, 0x05,

 // slots: signature, parameters, type, tag, flags
      41,    5,    5,    5, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Tool[] = {
    "Tool\0\0objectChanged()\0appObjectChanged()\0"
    "onObjectSelectionChanged()\0"
};

void Tool::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Tool *_t = static_cast<Tool *>(_o);
        switch (_id) {
        case 0: _t->objectChanged(); break;
        case 1: _t->appObjectChanged(); break;
        case 2: _t->onObjectSelectionChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Tool::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Tool::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Tool,
      qt_meta_data_Tool, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Tool::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Tool::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Tool::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Tool))
        return static_cast<void*>(const_cast< Tool*>(this));
    return QObject::qt_metacast(_clname);
}

int Tool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Tool::objectChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Tool::appObjectChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE

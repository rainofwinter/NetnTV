/****************************************************************************
** Meta object code from reading C++ file 'InspectComponentTool.h'
**
** Created: Thu Aug 6 11:16:31 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../InspectComponentTool.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'InspectComponentTool.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_InspectComponentWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x05,

 // slots: signature, parameters, type, tag, flags
      33,   23,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_InspectComponentWindow[] = {
    "InspectComponentWindow\0\0closed()\0"
    "onDone()\0"
};

void InspectComponentWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        InspectComponentWindow *_t = static_cast<InspectComponentWindow *>(_o);
        switch (_id) {
        case 0: _t->closed(); break;
        case 1: _t->onDone(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData InspectComponentWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject InspectComponentWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_InspectComponentWindow,
      qt_meta_data_InspectComponentWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &InspectComponentWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *InspectComponentWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *InspectComponentWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_InspectComponentWindow))
        return static_cast<void*>(const_cast< InspectComponentWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int InspectComponentWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void InspectComponentWindow::closed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_InspectComponentTool[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x08,
      39,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_InspectComponentTool[] = {
    "InspectComponentTool\0\0onWindowClosed()\0"
    "onLostObjSelection()\0"
};

void InspectComponentTool::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        InspectComponentTool *_t = static_cast<InspectComponentTool *>(_o);
        switch (_id) {
        case 0: _t->onWindowClosed(); break;
        case 1: _t->onLostObjSelection(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData InspectComponentTool::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject InspectComponentTool::staticMetaObject = {
    { &Tool::staticMetaObject, qt_meta_stringdata_InspectComponentTool,
      qt_meta_data_InspectComponentTool, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &InspectComponentTool::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *InspectComponentTool::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *InspectComponentTool::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_InspectComponentTool))
        return static_cast<void*>(const_cast< InspectComponentTool*>(this));
    return Tool::qt_metacast(_clname);
}

int InspectComponentTool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Tool::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

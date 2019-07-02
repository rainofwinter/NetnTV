/****************************************************************************
** Meta object code from reading C++ file 'ActionDlg.h'
**
** Created: Thu Aug 6 11:16:32 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../ActionDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ActionDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ActionPropertiesWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_ActionPropertiesWidget[] = {
    "ActionPropertiesWidget\0\0changed(Property*)\0"
};

void ActionPropertiesWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ActionPropertiesWidget *_t = static_cast<ActionPropertiesWidget *>(_o);
        switch (_id) {
        case 0: _t->changed((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ActionPropertiesWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ActionPropertiesWidget::staticMetaObject = {
    { &QScrollArea::staticMetaObject, qt_meta_stringdata_ActionPropertiesWidget,
      qt_meta_data_ActionPropertiesWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ActionPropertiesWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ActionPropertiesWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ActionPropertiesWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ActionPropertiesWidget))
        return static_cast<void*>(const_cast< ActionPropertiesWidget*>(this));
    return QScrollArea::qt_metacast(_clname);
}

int ActionPropertiesWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void ActionPropertiesWidget::changed(Property * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_ActionsWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      21,   15,   14,   14, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_ActionsWidget[] = {
    "ActionsWidget\0\0index\0actionSelectionChanged(int)\0"
};

void ActionsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ActionsWidget *_t = static_cast<ActionsWidget *>(_o);
        switch (_id) {
        case 0: _t->actionSelectionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ActionsWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ActionsWidget::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_ActionsWidget,
      qt_meta_data_ActionsWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ActionsWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ActionsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ActionsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ActionsWidget))
        return static_cast<void*>(const_cast< ActionsWidget*>(this));
    return QTreeWidget::qt_metacast(_clname);
}

int ActionsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void ActionsWidget::actionSelectionChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_ActionDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   11,   10,   10, 0x08,
      51,   10,   10,   10, 0x08,
      92,   86,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ActionDlg[] = {
    "ActionDlg\0\0obj\0onSceneObjectSelected(SceneObject*)\0"
    "onActionPropertyChanged(Property*)\0"
    "index\0onActionSelectionChanged(int)\0"
};

void ActionDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ActionDlg *_t = static_cast<ActionDlg *>(_o);
        switch (_id) {
        case 0: _t->onSceneObjectSelected((*reinterpret_cast< SceneObject*(*)>(_a[1]))); break;
        case 1: _t->onActionPropertyChanged((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        case 2: _t->onActionSelectionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ActionDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ActionDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ActionDlg,
      qt_meta_data_ActionDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ActionDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ActionDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ActionDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ActionDlg))
        return static_cast<void*>(const_cast< ActionDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int ActionDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

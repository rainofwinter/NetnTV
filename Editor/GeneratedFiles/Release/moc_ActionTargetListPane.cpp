/****************************************************************************
** Meta object code from reading C++ file 'ActionTargetListPane.h'
**
** Created: Thu Aug 6 11:16:30 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../ActionTargetListPane.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ActionTargetListPane.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ActionTargetListModel[] = {

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

static const char qt_meta_stringdata_ActionTargetListModel[] = {
    "ActionTargetListModel\0"
};

void ActionTargetListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ActionTargetListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ActionTargetListModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_ActionTargetListModel,
      qt_meta_data_ActionTargetListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ActionTargetListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ActionTargetListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ActionTargetListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ActionTargetListModel))
        return static_cast<void*>(const_cast< ActionTargetListModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int ActionTargetListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ActionTargetListDelegate[] = {

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

static const char qt_meta_stringdata_ActionTargetListDelegate[] = {
    "ActionTargetListDelegate\0"
};

void ActionTargetListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ActionTargetListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ActionTargetListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_ActionTargetListDelegate,
      qt_meta_data_ActionTargetListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ActionTargetListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ActionTargetListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ActionTargetListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ActionTargetListDelegate))
        return static_cast<void*>(const_cast< ActionTargetListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int ActionTargetListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ActionTargetListTreeView[] = {

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
      26,   25,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
      53,   25,   25,   25, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ActionTargetListTreeView[] = {
    "ActionTargetListTreeView\0\0"
    "itemSelected(SceneObject*)\0"
    "onActionTargetListChanged()\0"
};

void ActionTargetListTreeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ActionTargetListTreeView *_t = static_cast<ActionTargetListTreeView *>(_o);
        switch (_id) {
        case 0: _t->itemSelected((*reinterpret_cast< SceneObject*(*)>(_a[1]))); break;
        case 1: _t->onActionTargetListChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ActionTargetListTreeView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ActionTargetListTreeView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_ActionTargetListTreeView,
      qt_meta_data_ActionTargetListTreeView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ActionTargetListTreeView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ActionTargetListTreeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ActionTargetListTreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ActionTargetListTreeView))
        return static_cast<void*>(const_cast< ActionTargetListTreeView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int ActionTargetListTreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
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
void ActionTargetListTreeView::itemSelected(SceneObject * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_ActionTargetListPane[] = {

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
      22,   21,   21,   21, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_ActionTargetListPane[] = {
    "ActionTargetListPane\0\0itemSelected(SceneObject*)\0"
};

void ActionTargetListPane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ActionTargetListPane *_t = static_cast<ActionTargetListPane *>(_o);
        switch (_id) {
        case 0: _t->itemSelected((*reinterpret_cast< SceneObject*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ActionTargetListPane::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ActionTargetListPane::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ActionTargetListPane,
      qt_meta_data_ActionTargetListPane, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ActionTargetListPane::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ActionTargetListPane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ActionTargetListPane::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ActionTargetListPane))
        return static_cast<void*>(const_cast< ActionTargetListPane*>(this));
    return QWidget::qt_metacast(_clname);
}

int ActionTargetListPane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void ActionTargetListPane::itemSelected(SceneObject * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE

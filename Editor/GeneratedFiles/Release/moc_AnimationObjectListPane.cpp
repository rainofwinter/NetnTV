/****************************************************************************
** Meta object code from reading C++ file 'AnimationObjectListPane.h'
**
** Created: Thu Aug 6 11:16:23 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../AnimationObjectListPane.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnimationObjectListPane.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AnimationObjectListModel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_AnimationObjectListModel[] = {
    "AnimationObjectListModel\0\0update()\0"
};

void AnimationObjectListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AnimationObjectListModel *_t = static_cast<AnimationObjectListModel *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData AnimationObjectListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AnimationObjectListModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_AnimationObjectListModel,
      qt_meta_data_AnimationObjectListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AnimationObjectListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AnimationObjectListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AnimationObjectListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AnimationObjectListModel))
        return static_cast<void*>(const_cast< AnimationObjectListModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int AnimationObjectListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_AnimationObjectListDelegate[] = {

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

static const char qt_meta_stringdata_AnimationObjectListDelegate[] = {
    "AnimationObjectListDelegate\0"
};

void AnimationObjectListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData AnimationObjectListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AnimationObjectListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_AnimationObjectListDelegate,
      qt_meta_data_AnimationObjectListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AnimationObjectListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AnimationObjectListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AnimationObjectListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AnimationObjectListDelegate))
        return static_cast<void*>(const_cast< AnimationObjectListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int AnimationObjectListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_AnimationObjectListTreeView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      29,   28,   28,   28, 0x05,

 // slots: signature, parameters, type, tag, flags
      51,   28,   28,   28, 0x09,
      69,   28,   28,   28, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_AnimationObjectListTreeView[] = {
    "AnimationObjectListTreeView\0\0"
    "uiPropertiesChanged()\0updateSelection()\0"
    "updateSceneObjectList()\0"
};

void AnimationObjectListTreeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AnimationObjectListTreeView *_t = static_cast<AnimationObjectListTreeView *>(_o);
        switch (_id) {
        case 0: _t->uiPropertiesChanged(); break;
        case 1: _t->updateSelection(); break;
        case 2: _t->updateSceneObjectList(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData AnimationObjectListTreeView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AnimationObjectListTreeView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_AnimationObjectListTreeView,
      qt_meta_data_AnimationObjectListTreeView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AnimationObjectListTreeView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AnimationObjectListTreeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AnimationObjectListTreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AnimationObjectListTreeView))
        return static_cast<void*>(const_cast< AnimationObjectListTreeView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int AnimationObjectListTreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
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
void AnimationObjectListTreeView::uiPropertiesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_AnimationObjectListPane[] = {

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
      31,   25,   24,   24, 0x08,
      55,   25,   24,   24, 0x08,
      80,   24,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_AnimationObjectListPane[] = {
    "AnimationObjectListPane\0\0index\0"
    "onExpanded(QModelIndex)\0"
    "onCollapsed(QModelIndex)\0updateScene()\0"
};

void AnimationObjectListPane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AnimationObjectListPane *_t = static_cast<AnimationObjectListPane *>(_o);
        switch (_id) {
        case 0: _t->onExpanded((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: _t->onCollapsed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: _t->updateScene(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData AnimationObjectListPane::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AnimationObjectListPane::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_AnimationObjectListPane,
      qt_meta_data_AnimationObjectListPane, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AnimationObjectListPane::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AnimationObjectListPane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AnimationObjectListPane::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AnimationObjectListPane))
        return static_cast<void*>(const_cast< AnimationObjectListPane*>(this));
    return QWidget::qt_metacast(_clname);
}

int AnimationObjectListPane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

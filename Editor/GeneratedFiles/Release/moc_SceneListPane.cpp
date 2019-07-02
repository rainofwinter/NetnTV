/****************************************************************************
** Meta object code from reading C++ file 'SceneListPane.h'
**
** Created: Thu Aug 6 11:16:30 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../SceneListPane.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SceneListPane.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SceneListModel[] = {

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
      16,   15,   15,   15, 0x0a,
      37,   15,   15,   15, 0x0a,
      46,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SceneListModel[] = {
    "SceneListModel\0\0onSceneListChanged()\0"
    "update()\0updateThumbnail()\0"
};

void SceneListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SceneListModel *_t = static_cast<SceneListModel *>(_o);
        switch (_id) {
        case 0: _t->onSceneListChanged(); break;
        case 1: _t->update(); break;
        case 2: _t->updateThumbnail(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SceneListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SceneListModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_SceneListModel,
      qt_meta_data_SceneListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SceneListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SceneListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SceneListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SceneListModel))
        return static_cast<void*>(const_cast< SceneListModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int SceneListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_SceneListDelegate[] = {

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
      19,   18,   18,   18, 0x09,
      42,   18,   18,   18, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SceneListDelegate[] = {
    "SceneListDelegate\0\0commitAndCloseEditor()\0"
    "cancelEdit()\0"
};

void SceneListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SceneListDelegate *_t = static_cast<SceneListDelegate *>(_o);
        switch (_id) {
        case 0: _t->commitAndCloseEditor(); break;
        case 1: _t->cancelEdit(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SceneListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SceneListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_SceneListDelegate,
      qt_meta_data_SceneListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SceneListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SceneListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SceneListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SceneListDelegate))
        return static_cast<void*>(const_cast< SceneListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int SceneListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_SceneListTreeView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x09,
      37,   18,   18,   18, 0x09,
      57,   18,   18,   18, 0x09,
      71,   18,   18,   18, 0x09,
      86,   18,   18,   18, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SceneListTreeView[] = {
    "SceneListTreeView\0\0updateSelection()\0"
    "onSceneProperties()\0onSceneCopy()\0"
    "onScenePaste()\0onTemplateSceneCreate()\0"
};

void SceneListTreeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SceneListTreeView *_t = static_cast<SceneListTreeView *>(_o);
        switch (_id) {
        case 0: _t->updateSelection(); break;
        case 1: _t->onSceneProperties(); break;
        case 2: _t->onSceneCopy(); break;
        case 3: _t->onScenePaste(); break;
        case 4: _t->onTemplateSceneCreate(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SceneListTreeView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SceneListTreeView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_SceneListTreeView,
      qt_meta_data_SceneListTreeView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SceneListTreeView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SceneListTreeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SceneListTreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SceneListTreeView))
        return static_cast<void*>(const_cast< SceneListTreeView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int SceneListTreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
static const uint qt_meta_data_SceneListPane[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   15,   14,   14, 0x05,
      51,   45,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      69,   14,   14,   14, 0x0a,
      85,   14,   14,   14, 0x0a,
     106,   14,   14,   14, 0x0a,
     114,   14,   14,   14, 0x0a,
     125,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SceneListPane[] = {
    "SceneListPane\0\0min,max\0rangeChanged(int,int)\0"
    "value\0valueChanged(int)\0triggerRedraw()\0"
    "changeButtonsState()\0onNew()\0onDelete()\0"
    "onTemplateSceneChange()\0"
};

void SceneListPane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SceneListPane *_t = static_cast<SceneListPane *>(_o);
        switch (_id) {
        case 0: _t->rangeChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->triggerRedraw(); break;
        case 3: _t->changeButtonsState(); break;
        case 4: _t->onNew(); break;
        case 5: _t->onDelete(); break;
        case 6: _t->onTemplateSceneChange(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SceneListPane::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SceneListPane::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SceneListPane,
      qt_meta_data_SceneListPane, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SceneListPane::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SceneListPane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SceneListPane::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SceneListPane))
        return static_cast<void*>(const_cast< SceneListPane*>(this));
    return QWidget::qt_metacast(_clname);
}

int SceneListPane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void SceneListPane::rangeChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SceneListPane::valueChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE

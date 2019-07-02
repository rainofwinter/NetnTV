/****************************************************************************
** Meta object code from reading C++ file 'AnimationListPane.h'
**
** Created: Thu Aug 6 11:16:31 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../AnimationListPane.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnimationListPane.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AnimationListModel[] = {

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
      20,   19,   19,   19, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_AnimationListModel[] = {
    "AnimationListModel\0\0update()\0"
};

void AnimationListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AnimationListModel *_t = static_cast<AnimationListModel *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData AnimationListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AnimationListModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_AnimationListModel,
      qt_meta_data_AnimationListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AnimationListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AnimationListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AnimationListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AnimationListModel))
        return static_cast<void*>(const_cast< AnimationListModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int AnimationListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_AnimationListDelegate[] = {

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
      23,   22,   22,   22, 0x09,
      46,   22,   22,   22, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_AnimationListDelegate[] = {
    "AnimationListDelegate\0\0commitAndCloseEditor()\0"
    "cancelEdit()\0"
};

void AnimationListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AnimationListDelegate *_t = static_cast<AnimationListDelegate *>(_o);
        switch (_id) {
        case 0: _t->commitAndCloseEditor(); break;
        case 1: _t->cancelEdit(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData AnimationListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AnimationListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_AnimationListDelegate,
      qt_meta_data_AnimationListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AnimationListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AnimationListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AnimationListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AnimationListDelegate))
        return static_cast<void*>(const_cast< AnimationListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int AnimationListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_AnimationListTreeView[] = {

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
      23,   22,   22,   22, 0x09,
      39,   22,   22,   22, 0x09,
      57,   22,   22,   22, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_AnimationListTreeView[] = {
    "AnimationListTreeView\0\0layoutChanged()\0"
    "updateSelection()\0onProperties()\0"
};

void AnimationListTreeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AnimationListTreeView *_t = static_cast<AnimationListTreeView *>(_o);
        switch (_id) {
        case 0: _t->layoutChanged(); break;
        case 1: _t->updateSelection(); break;
        case 2: _t->onProperties(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData AnimationListTreeView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AnimationListTreeView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_AnimationListTreeView,
      qt_meta_data_AnimationListTreeView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AnimationListTreeView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AnimationListTreeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AnimationListTreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AnimationListTreeView))
        return static_cast<void*>(const_cast< AnimationListTreeView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int AnimationListTreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_AnimationListPane[] = {

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
      19,   18,   18,   18, 0x0a,
      40,   18,   18,   18, 0x0a,
      48,   18,   18,   18, 0x0a,
      59,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_AnimationListPane[] = {
    "AnimationListPane\0\0changeButtonsState()\0"
    "onNew()\0onDelete()\0updateScene()\0"
};

void AnimationListPane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AnimationListPane *_t = static_cast<AnimationListPane *>(_o);
        switch (_id) {
        case 0: _t->changeButtonsState(); break;
        case 1: _t->onNew(); break;
        case 2: _t->onDelete(); break;
        case 3: _t->updateScene(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData AnimationListPane::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AnimationListPane::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_AnimationListPane,
      qt_meta_data_AnimationListPane, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AnimationListPane::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AnimationListPane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AnimationListPane::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AnimationListPane))
        return static_cast<void*>(const_cast< AnimationListPane*>(this));
    return QWidget::qt_metacast(_clname);
}

int AnimationListPane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE

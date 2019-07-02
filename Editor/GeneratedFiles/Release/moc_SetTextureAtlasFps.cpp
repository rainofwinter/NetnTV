/****************************************************************************
** Meta object code from reading C++ file 'SetTextureAtlasFps.h'
**
** Created: Thu Aug 6 11:16:28 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../SetTextureAtlasFps.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SetTextureAtlasFps.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SetTextureAtlasFpsListModel[] = {

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

static const char qt_meta_stringdata_SetTextureAtlasFpsListModel[] = {
    "SetTextureAtlasFpsListModel\0"
};

void SetTextureAtlasFpsListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SetTextureAtlasFpsListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SetTextureAtlasFpsListModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_SetTextureAtlasFpsListModel,
      qt_meta_data_SetTextureAtlasFpsListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SetTextureAtlasFpsListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SetTextureAtlasFpsListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SetTextureAtlasFpsListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SetTextureAtlasFpsListModel))
        return static_cast<void*>(const_cast< SetTextureAtlasFpsListModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int SetTextureAtlasFpsListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_SetTextureAtlasFpsListDelegate[] = {

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
      37,   32,   31,   31, 0x05,

 // slots: signature, parameters, type, tag, flags
      73,   31,   31,   31, 0x09,
      96,   31,   31,   31, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SetTextureAtlasFpsListDelegate[] = {
    "SetTextureAtlasFpsListDelegate\0\0item\0"
    "itemEdited(ImageSpriteFpsListItem*)\0"
    "commitAndCloseEditor()\0cancelEdit()\0"
};

void SetTextureAtlasFpsListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SetTextureAtlasFpsListDelegate *_t = static_cast<SetTextureAtlasFpsListDelegate *>(_o);
        switch (_id) {
        case 0: _t->itemEdited((*reinterpret_cast< ImageSpriteFpsListItem*(*)>(_a[1]))); break;
        case 1: _t->commitAndCloseEditor(); break;
        case 2: _t->cancelEdit(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SetTextureAtlasFpsListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SetTextureAtlasFpsListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_SetTextureAtlasFpsListDelegate,
      qt_meta_data_SetTextureAtlasFpsListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SetTextureAtlasFpsListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SetTextureAtlasFpsListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SetTextureAtlasFpsListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SetTextureAtlasFpsListDelegate))
        return static_cast<void*>(const_cast< SetTextureAtlasFpsListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int SetTextureAtlasFpsListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
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
void SetTextureAtlasFpsListDelegate::itemEdited(ImageSpriteFpsListItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_SetTextureAtlasFpsList[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x05,
      43,   23,   23,   23, 0x05,
      64,   59,   23,   23, 0x05,

 // slots: signature, parameters, type, tag, flags
     100,   23,   23,   23, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SetTextureAtlasFpsList[] = {
    "SetTextureAtlasFpsList\0\0selectionChanged()\0"
    "layoutChanged()\0item\0"
    "itemEdited(ImageSpriteFpsListItem*)\0"
    "onLayoutChanged()\0"
};

void SetTextureAtlasFpsList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SetTextureAtlasFpsList *_t = static_cast<SetTextureAtlasFpsList *>(_o);
        switch (_id) {
        case 0: _t->selectionChanged(); break;
        case 1: _t->layoutChanged(); break;
        case 2: _t->itemEdited((*reinterpret_cast< ImageSpriteFpsListItem*(*)>(_a[1]))); break;
        case 3: _t->onLayoutChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SetTextureAtlasFpsList::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SetTextureAtlasFpsList::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_SetTextureAtlasFpsList,
      qt_meta_data_SetTextureAtlasFpsList, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SetTextureAtlasFpsList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SetTextureAtlasFpsList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SetTextureAtlasFpsList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SetTextureAtlasFpsList))
        return static_cast<void*>(const_cast< SetTextureAtlasFpsList*>(this));
    return QTreeView::qt_metacast(_clname);
}

int SetTextureAtlasFpsList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void SetTextureAtlasFpsList::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void SetTextureAtlasFpsList::layoutChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void SetTextureAtlasFpsList::itemEdited(ImageSpriteFpsListItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
static const uint qt_meta_data_SetTextureAtlasFps[] = {

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
      20,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SetTextureAtlasFps[] = {
    "SetTextureAtlasFps\0\0onOk()\0"
};

void SetTextureAtlasFps::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SetTextureAtlasFps *_t = static_cast<SetTextureAtlasFps *>(_o);
        switch (_id) {
        case 0: _t->onOk(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SetTextureAtlasFps::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SetTextureAtlasFps::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SetTextureAtlasFps,
      qt_meta_data_SetTextureAtlasFps, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SetTextureAtlasFps::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SetTextureAtlasFps::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SetTextureAtlasFps::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SetTextureAtlasFps))
        return static_cast<void*>(const_cast< SetTextureAtlasFps*>(this));
    return QDialog::qt_metacast(_clname);
}

int SetTextureAtlasFps::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

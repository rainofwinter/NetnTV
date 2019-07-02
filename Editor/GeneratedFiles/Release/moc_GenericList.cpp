/****************************************************************************
** Meta object code from reading C++ file 'GenericList.h'
**
** Created: Thu Aug 6 11:16:30 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../GenericList.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GenericList.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GenericListModel[] = {

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

static const char qt_meta_stringdata_GenericListModel[] = {
    "GenericListModel\0"
};

void GenericListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData GenericListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GenericListModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_GenericListModel,
      qt_meta_data_GenericListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GenericListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GenericListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GenericListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GenericListModel))
        return static_cast<void*>(const_cast< GenericListModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int GenericListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_GenericListDelegate[] = {

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
      26,   21,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
      55,   20,   20,   20, 0x09,
      78,   20,   20,   20, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GenericListDelegate[] = {
    "GenericListDelegate\0\0item\0"
    "itemEdited(GenericListItem*)\0"
    "commitAndCloseEditor()\0cancelEdit()\0"
};

void GenericListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GenericListDelegate *_t = static_cast<GenericListDelegate *>(_o);
        switch (_id) {
        case 0: _t->itemEdited((*reinterpret_cast< GenericListItem*(*)>(_a[1]))); break;
        case 1: _t->commitAndCloseEditor(); break;
        case 2: _t->cancelEdit(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GenericListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GenericListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_GenericListDelegate,
      qt_meta_data_GenericListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GenericListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GenericListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GenericListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GenericListDelegate))
        return static_cast<void*>(const_cast< GenericListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int GenericListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void GenericListDelegate::itemEdited(GenericListItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_GenericList[] = {

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
      13,   12,   12,   12, 0x05,
      32,   12,   12,   12, 0x05,
      53,   48,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      82,   12,   12,   12, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GenericList[] = {
    "GenericList\0\0selectionChanged()\0"
    "layoutChanged()\0item\0itemEdited(GenericListItem*)\0"
    "onLayoutChanged()\0"
};

void GenericList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GenericList *_t = static_cast<GenericList *>(_o);
        switch (_id) {
        case 0: _t->selectionChanged(); break;
        case 1: _t->layoutChanged(); break;
        case 2: _t->itemEdited((*reinterpret_cast< GenericListItem*(*)>(_a[1]))); break;
        case 3: _t->onLayoutChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GenericList::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GenericList::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_GenericList,
      qt_meta_data_GenericList, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GenericList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GenericList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GenericList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GenericList))
        return static_cast<void*>(const_cast< GenericList*>(this));
    return QTreeView::qt_metacast(_clname);
}

int GenericList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void GenericList::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void GenericList::layoutChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void GenericList::itemEdited(GenericListItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE

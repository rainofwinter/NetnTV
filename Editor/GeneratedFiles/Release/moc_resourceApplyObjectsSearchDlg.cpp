/****************************************************************************
** Meta object code from reading C++ file 'resourceApplyObjectsSearchDlg.h'
**
** Created: Thu Aug 6 11:16:28 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../resourceApplyObjectsSearchDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'resourceApplyObjectsSearchDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_resourceApplyObjectsSearchModel[] = {

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

static const char qt_meta_stringdata_resourceApplyObjectsSearchModel[] = {
    "resourceApplyObjectsSearchModel\0"
};

void resourceApplyObjectsSearchModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData resourceApplyObjectsSearchModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject resourceApplyObjectsSearchModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_resourceApplyObjectsSearchModel,
      qt_meta_data_resourceApplyObjectsSearchModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &resourceApplyObjectsSearchModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *resourceApplyObjectsSearchModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *resourceApplyObjectsSearchModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_resourceApplyObjectsSearchModel))
        return static_cast<void*>(const_cast< resourceApplyObjectsSearchModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int resourceApplyObjectsSearchModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_resourceApplyObjectsSearchDelegate[] = {

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

static const char qt_meta_stringdata_resourceApplyObjectsSearchDelegate[] = {
    "resourceApplyObjectsSearchDelegate\0"
};

void resourceApplyObjectsSearchDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData resourceApplyObjectsSearchDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject resourceApplyObjectsSearchDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_resourceApplyObjectsSearchDelegate,
      qt_meta_data_resourceApplyObjectsSearchDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &resourceApplyObjectsSearchDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *resourceApplyObjectsSearchDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *resourceApplyObjectsSearchDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_resourceApplyObjectsSearchDelegate))
        return static_cast<void*>(const_cast< resourceApplyObjectsSearchDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int resourceApplyObjectsSearchDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_resourceApplyObjectsSearchList[] = {

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
      32,   31,   31,   31, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_resourceApplyObjectsSearchList[] = {
    "resourceApplyObjectsSearchList\0\0"
    "layoutChanged()\0"
};

void resourceApplyObjectsSearchList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        resourceApplyObjectsSearchList *_t = static_cast<resourceApplyObjectsSearchList *>(_o);
        switch (_id) {
        case 0: _t->layoutChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData resourceApplyObjectsSearchList::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject resourceApplyObjectsSearchList::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_resourceApplyObjectsSearchList,
      qt_meta_data_resourceApplyObjectsSearchList, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &resourceApplyObjectsSearchList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *resourceApplyObjectsSearchList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *resourceApplyObjectsSearchList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_resourceApplyObjectsSearchList))
        return static_cast<void*>(const_cast< resourceApplyObjectsSearchList*>(this));
    return QTreeView::qt_metacast(_clname);
}

int resourceApplyObjectsSearchList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
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
void resourceApplyObjectsSearchList::layoutChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_resourceApplyObjectsSearchDlg[] = {

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
      31,   30,   30,   30, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_resourceApplyObjectsSearchDlg[] = {
    "resourceApplyObjectsSearchDlg\0\0onOk()\0"
};

void resourceApplyObjectsSearchDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        resourceApplyObjectsSearchDlg *_t = static_cast<resourceApplyObjectsSearchDlg *>(_o);
        switch (_id) {
        case 0: _t->onOk(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData resourceApplyObjectsSearchDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject resourceApplyObjectsSearchDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_resourceApplyObjectsSearchDlg,
      qt_meta_data_resourceApplyObjectsSearchDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &resourceApplyObjectsSearchDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *resourceApplyObjectsSearchDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *resourceApplyObjectsSearchDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_resourceApplyObjectsSearchDlg))
        return static_cast<void*>(const_cast< resourceApplyObjectsSearchDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int resourceApplyObjectsSearchDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

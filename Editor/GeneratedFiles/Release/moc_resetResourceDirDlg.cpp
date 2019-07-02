/****************************************************************************
** Meta object code from reading C++ file 'resetResourceDirDlg.h'
**
** Created: Thu Aug 6 11:16:28 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../resetResourceDirDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'resetResourceDirDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_resetResourceDirModel[] = {

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

static const char qt_meta_stringdata_resetResourceDirModel[] = {
    "resetResourceDirModel\0"
};

void resetResourceDirModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData resetResourceDirModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject resetResourceDirModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_resetResourceDirModel,
      qt_meta_data_resetResourceDirModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &resetResourceDirModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *resetResourceDirModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *resetResourceDirModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_resetResourceDirModel))
        return static_cast<void*>(const_cast< resetResourceDirModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int resetResourceDirModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_resetResourceDirDelegate[] = {

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

static const char qt_meta_stringdata_resetResourceDirDelegate[] = {
    "resetResourceDirDelegate\0"
};

void resetResourceDirDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData resetResourceDirDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject resetResourceDirDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_resetResourceDirDelegate,
      qt_meta_data_resetResourceDirDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &resetResourceDirDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *resetResourceDirDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *resetResourceDirDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_resetResourceDirDelegate))
        return static_cast<void*>(const_cast< resetResourceDirDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int resetResourceDirDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_resetResourceDirList[] = {

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
      22,   21,   21,   21, 0x05,
      38,   21,   21,   21, 0x05,
      57,   21,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
      73,   21,   21,   21, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_resetResourceDirList[] = {
    "resetResourceDirList\0\0layoutChanged()\0"
    "selectionChanged()\0doubleClicked()\0"
    "onLayoutChanged()\0"
};

void resetResourceDirList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        resetResourceDirList *_t = static_cast<resetResourceDirList *>(_o);
        switch (_id) {
        case 0: _t->layoutChanged(); break;
        case 1: _t->selectionChanged(); break;
        case 2: _t->doubleClicked(); break;
        case 3: _t->onLayoutChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData resetResourceDirList::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject resetResourceDirList::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_resetResourceDirList,
      qt_meta_data_resetResourceDirList, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &resetResourceDirList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *resetResourceDirList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *resetResourceDirList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_resetResourceDirList))
        return static_cast<void*>(const_cast< resetResourceDirList*>(this));
    return QTreeView::qt_metacast(_clname);
}

int resetResourceDirList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void resetResourceDirList::layoutChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void resetResourceDirList::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void resetResourceDirList::doubleClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
static const uint qt_meta_data_resetResourceDirDlg[] = {

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
      21,   20,   20,   20, 0x08,
      33,   20,   20,   20, 0x08,
      40,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_resetResourceDirDlg[] = {
    "resetResourceDirDlg\0\0onSetFile()\0"
    "onOk()\0onFileSelectionChanged()\0"
};

void resetResourceDirDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        resetResourceDirDlg *_t = static_cast<resetResourceDirDlg *>(_o);
        switch (_id) {
        case 0: _t->onSetFile(); break;
        case 1: _t->onOk(); break;
        case 2: _t->onFileSelectionChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData resetResourceDirDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject resetResourceDirDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_resetResourceDirDlg,
      qt_meta_data_resetResourceDirDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &resetResourceDirDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *resetResourceDirDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *resetResourceDirDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_resetResourceDirDlg))
        return static_cast<void*>(const_cast< resetResourceDirDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int resetResourceDirDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

/****************************************************************************
** Meta object code from reading C++ file 'SetResourceFileDlg.h'
**
** Created: Thu Aug 6 11:16:28 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../SetResourceFileDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SetResourceFileDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SetResourceFileListModel[] = {

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

static const char qt_meta_stringdata_SetResourceFileListModel[] = {
    "SetResourceFileListModel\0"
};

void SetResourceFileListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SetResourceFileListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SetResourceFileListModel::staticMetaObject = {
    { &QStandardItemModel::staticMetaObject, qt_meta_stringdata_SetResourceFileListModel,
      qt_meta_data_SetResourceFileListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SetResourceFileListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SetResourceFileListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SetResourceFileListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SetResourceFileListModel))
        return static_cast<void*>(const_cast< SetResourceFileListModel*>(this));
    return QStandardItemModel::qt_metacast(_clname);
}

int SetResourceFileListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStandardItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_SetResourceFileListDelegate[] = {

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

static const char qt_meta_stringdata_SetResourceFileListDelegate[] = {
    "SetResourceFileListDelegate\0"
};

void SetResourceFileListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SetResourceFileListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SetResourceFileListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_SetResourceFileListDelegate,
      qt_meta_data_SetResourceFileListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SetResourceFileListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SetResourceFileListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SetResourceFileListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SetResourceFileListDelegate))
        return static_cast<void*>(const_cast< SetResourceFileListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int SetResourceFileListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_SetResourceFileList[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x05,
      40,   20,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
      56,   20,   20,   20, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SetResourceFileList[] = {
    "SetResourceFileList\0\0selectionChanged()\0"
    "layoutChanged()\0onLayoutChanged()\0"
};

void SetResourceFileList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SetResourceFileList *_t = static_cast<SetResourceFileList *>(_o);
        switch (_id) {
        case 0: _t->selectionChanged(); break;
        case 1: _t->layoutChanged(); break;
        case 2: _t->onLayoutChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SetResourceFileList::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SetResourceFileList::staticMetaObject = {
    { &QListView::staticMetaObject, qt_meta_stringdata_SetResourceFileList,
      qt_meta_data_SetResourceFileList, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SetResourceFileList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SetResourceFileList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SetResourceFileList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SetResourceFileList))
        return static_cast<void*>(const_cast< SetResourceFileList*>(this));
    return QListView::qt_metacast(_clname);
}

int SetResourceFileList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListView::qt_metacall(_c, _id, _a);
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
void SetResourceFileList::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void SetResourceFileList::layoutChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
static const uint qt_meta_data_SetResourceFileDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x08,
      27,   19,   19,   19, 0x08,
      52,   19,   19,   19, 0x08,
      68,   19,   19,   19, 0x08,
      86,   19,   19,   19, 0x08,
      94,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SetResourceFileDlg[] = {
    "SetResourceFileDlg\0\0onOk()\0"
    "onFileSelectionChanged()\0onSearchClear()\0"
    "onSearch(QString)\0onNew()\0onChangeList()\0"
};

void SetResourceFileDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SetResourceFileDlg *_t = static_cast<SetResourceFileDlg *>(_o);
        switch (_id) {
        case 0: _t->onOk(); break;
        case 1: _t->onFileSelectionChanged(); break;
        case 2: _t->onSearchClear(); break;
        case 3: _t->onSearch((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->onNew(); break;
        case 5: _t->onChangeList(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SetResourceFileDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SetResourceFileDlg::staticMetaObject = {
    { &resourceDialog::staticMetaObject, qt_meta_stringdata_SetResourceFileDlg,
      qt_meta_data_SetResourceFileDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SetResourceFileDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SetResourceFileDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SetResourceFileDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SetResourceFileDlg))
        return static_cast<void*>(const_cast< SetResourceFileDlg*>(this));
    return resourceDialog::qt_metacast(_clname);
}

int SetResourceFileDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = resourceDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

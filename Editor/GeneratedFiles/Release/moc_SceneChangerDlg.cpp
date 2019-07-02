/****************************************************************************
** Meta object code from reading C++ file 'SceneChangerDlg.h'
**
** Created: Thu Aug 6 11:16:37 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../SceneChangerDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SceneChangerDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SceneChangerListModel[] = {

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

static const char qt_meta_stringdata_SceneChangerListModel[] = {
    "SceneChangerListModel\0"
};

void SceneChangerListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SceneChangerListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SceneChangerListModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_SceneChangerListModel,
      qt_meta_data_SceneChangerListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SceneChangerListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SceneChangerListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SceneChangerListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SceneChangerListModel))
        return static_cast<void*>(const_cast< SceneChangerListModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int SceneChangerListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_SceneChangerListDelegate[] = {

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

static const char qt_meta_stringdata_SceneChangerListDelegate[] = {
    "SceneChangerListDelegate\0"
};

void SceneChangerListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SceneChangerListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SceneChangerListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_SceneChangerListDelegate,
      qt_meta_data_SceneChangerListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SceneChangerListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SceneChangerListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SceneChangerListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SceneChangerListDelegate))
        return static_cast<void*>(const_cast< SceneChangerListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int SceneChangerListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_SceneChangerListView[] = {

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
      41,   21,   21,   21, 0x05,
      62,   57,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
      91,   21,   21,   21, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SceneChangerListView[] = {
    "SceneChangerListView\0\0selectionChanged()\0"
    "layoutChanged()\0item\0itemEdited(GenericListItem*)\0"
    "onLayoutChanged()\0"
};

void SceneChangerListView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SceneChangerListView *_t = static_cast<SceneChangerListView *>(_o);
        switch (_id) {
        case 0: _t->selectionChanged(); break;
        case 1: _t->layoutChanged(); break;
        case 2: _t->itemEdited((*reinterpret_cast< GenericListItem*(*)>(_a[1]))); break;
        case 3: _t->onLayoutChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SceneChangerListView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SceneChangerListView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_SceneChangerListView,
      qt_meta_data_SceneChangerListView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SceneChangerListView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SceneChangerListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SceneChangerListView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SceneChangerListView))
        return static_cast<void*>(const_cast< SceneChangerListView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int SceneChangerListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void SceneChangerListView::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void SceneChangerListView::layoutChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void SceneChangerListView::itemEdited(GenericListItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
static const uint qt_meta_data_SceneChangerDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      42,   16,   16,   16, 0x08,
      68,   16,   16,   16, 0x08,
      80,   16,   16,   16, 0x08,
     103,   16,   16,   16, 0x08,
     118,   16,   16,   16, 0x08,
     125,   16,   16,   16, 0x08,
     139,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SceneChangerDlg[] = {
    "SceneChangerDlg\0\0onPageSelectionChanged()\0"
    "onSceneSelectionChanged()\0onAddPage()\0"
    "onPagesLayoutChanged()\0onDeletePage()\0"
    "onOk()\0onGenThumbs()\0onThumbFolderBrowse()\0"
};

void SceneChangerDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SceneChangerDlg *_t = static_cast<SceneChangerDlg *>(_o);
        switch (_id) {
        case 0: _t->onPageSelectionChanged(); break;
        case 1: _t->onSceneSelectionChanged(); break;
        case 2: _t->onAddPage(); break;
        case 3: _t->onPagesLayoutChanged(); break;
        case 4: _t->onDeletePage(); break;
        case 5: _t->onOk(); break;
        case 6: _t->onGenThumbs(); break;
        case 7: _t->onThumbFolderBrowse(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SceneChangerDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SceneChangerDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SceneChangerDlg,
      qt_meta_data_SceneChangerDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SceneChangerDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SceneChangerDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SceneChangerDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SceneChangerDlg))
        return static_cast<void*>(const_cast< SceneChangerDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int SceneChangerDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

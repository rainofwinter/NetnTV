/****************************************************************************
** Meta object code from reading C++ file 'ResourceListPane.h'
**
** Created: Thu Aug 6 11:16:28 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../ResourceListPane.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ResourceListPane.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ResourceListModel[] = {

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
      19,   18,   18,   18, 0x0a,
      43,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ResourceListModel[] = {
    "ResourceListModel\0\0onResourceListChanged()\0"
    "update()\0"
};

void ResourceListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ResourceListModel *_t = static_cast<ResourceListModel *>(_o);
        switch (_id) {
        case 0: _t->onResourceListChanged(); break;
        case 1: _t->update(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ResourceListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ResourceListModel::staticMetaObject = {
    { &QStandardItemModel::staticMetaObject, qt_meta_stringdata_ResourceListModel,
      qt_meta_data_ResourceListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ResourceListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ResourceListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ResourceListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ResourceListModel))
        return static_cast<void*>(const_cast< ResourceListModel*>(this));
    return QStandardItemModel::qt_metacast(_clname);
}

int ResourceListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStandardItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_ResourceListDelegate[] = {

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

static const char qt_meta_stringdata_ResourceListDelegate[] = {
    "ResourceListDelegate\0"
};

void ResourceListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ResourceListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ResourceListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_ResourceListDelegate,
      qt_meta_data_ResourceListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ResourceListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ResourceListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ResourceListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ResourceListDelegate))
        return static_cast<void*>(const_cast< ResourceListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int ResourceListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ResourceListView[] = {

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
      18,   17,   17,   17, 0x09,
      38,   17,   17,   17, 0x09,
      66,   17,   17,   17, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ResourceListView[] = {
    "ResourceListView\0\0onResourcePreview()\0"
    "onResourceApplyObjsSearch()\0"
    "onObjectCreate()\0"
};

void ResourceListView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ResourceListView *_t = static_cast<ResourceListView *>(_o);
        switch (_id) {
        case 0: _t->onResourcePreview(); break;
        case 1: _t->onResourceApplyObjsSearch(); break;
        case 2: _t->onObjectCreate(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ResourceListView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ResourceListView::staticMetaObject = {
    { &QListView::staticMetaObject, qt_meta_stringdata_ResourceListView,
      qt_meta_data_ResourceListView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ResourceListView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ResourceListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ResourceListView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ResourceListView))
        return static_cast<void*>(const_cast< ResourceListView*>(this));
    return QListView::qt_metacast(_clname);
}

int ResourceListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_ResourceListPane[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      26,   18,   17,   17, 0x05,
      54,   48,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      72,   17,   17,   17, 0x0a,
      88,   17,   17,   17, 0x0a,
     109,   17,   17,   17, 0x0a,
     117,   17,   17,   17, 0x0a,
     128,   48,   17,   17, 0x0a,
     149,   17,   17,   17, 0x0a,
     165,   17,   17,   17, 0x0a,
     183,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ResourceListPane[] = {
    "ResourceListPane\0\0min,max\0"
    "rangeChanged(int,int)\0value\0"
    "valueChanged(int)\0triggerRedraw()\0"
    "changeButtonsState()\0onNew()\0onDelete()\0"
    "comboboxChanged(int)\0onSearchClear()\0"
    "onSearch(QString)\0onChangeList()\0"
};

void ResourceListPane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ResourceListPane *_t = static_cast<ResourceListPane *>(_o);
        switch (_id) {
        case 0: _t->rangeChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->triggerRedraw(); break;
        case 3: _t->changeButtonsState(); break;
        case 4: _t->onNew(); break;
        case 5: _t->onDelete(); break;
        case 6: _t->comboboxChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->onSearchClear(); break;
        case 8: _t->onSearch((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->onChangeList(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ResourceListPane::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ResourceListPane::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ResourceListPane,
      qt_meta_data_ResourceListPane, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ResourceListPane::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ResourceListPane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ResourceListPane::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ResourceListPane))
        return static_cast<void*>(const_cast< ResourceListPane*>(this));
    return QWidget::qt_metacast(_clname);
}

int ResourceListPane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void ResourceListPane::rangeChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ResourceListPane::valueChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'ChannelListPane.h'
**
** Created: Thu Aug 6 11:16:31 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../ChannelListPane.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ChannelListPane.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ChannelListModel[] = {

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
      18,   17,   17,   17, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ChannelListModel[] = {
    "ChannelListModel\0\0update()\0"
};

void ChannelListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChannelListModel *_t = static_cast<ChannelListModel *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ChannelListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ChannelListModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_ChannelListModel,
      qt_meta_data_ChannelListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ChannelListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ChannelListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ChannelListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChannelListModel))
        return static_cast<void*>(const_cast< ChannelListModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int ChannelListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_ChannelListDelegate[] = {

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

static const char qt_meta_stringdata_ChannelListDelegate[] = {
    "ChannelListDelegate\0"
};

void ChannelListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ChannelListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ChannelListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_ChannelListDelegate,
      qt_meta_data_ChannelListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ChannelListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ChannelListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ChannelListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChannelListDelegate))
        return static_cast<void*>(const_cast< ChannelListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int ChannelListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ChannelListTreeView[] = {

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
      21,   20,   20,   20, 0x09,
      39,   20,   20,   20, 0x09,
      48,   20,   20,   20, 0x09,
      58,   20,   20,   20, 0x09,
      71,   20,   20,   20, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ChannelListTreeView[] = {
    "ChannelListTreeView\0\0updateSelection()\0"
    "onCopy()\0onPaste()\0onPasteKey()\0"
    "onAddChannelTemplate()\0"
};

void ChannelListTreeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChannelListTreeView *_t = static_cast<ChannelListTreeView *>(_o);
        switch (_id) {
        case 0: _t->updateSelection(); break;
        case 1: _t->onCopy(); break;
        case 2: _t->onPaste(); break;
        case 3: _t->onPasteKey(); break;
        case 4: _t->onAddChannelTemplate(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ChannelListTreeView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ChannelListTreeView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_ChannelListTreeView,
      qt_meta_data_ChannelListTreeView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ChannelListTreeView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ChannelListTreeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ChannelListTreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChannelListTreeView))
        return static_cast<void*>(const_cast< ChannelListTreeView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int ChannelListTreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_ChannelListPane[] = {

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
      25,   17,   16,   16, 0x05,
      53,   47,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      71,   16,   16,   16, 0x0a,
      92,   16,   16,   16, 0x0a,
     107,   16,   16,   16, 0x0a,
     135,   16,   16,   16, 0x0a,
     143,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ChannelListPane[] = {
    "ChannelListPane\0\0min,max\0rangeChanged(int,int)\0"
    "value\0valueChanged(int)\0changeButtonsState()\0"
    "updateObject()\0animationSelectionChanged()\0"
    "onNew()\0onDelete()\0"
};

void ChannelListPane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChannelListPane *_t = static_cast<ChannelListPane *>(_o);
        switch (_id) {
        case 0: _t->rangeChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->changeButtonsState(); break;
        case 3: _t->updateObject(); break;
        case 4: _t->animationSelectionChanged(); break;
        case 5: _t->onNew(); break;
        case 6: _t->onDelete(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ChannelListPane::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ChannelListPane::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ChannelListPane,
      qt_meta_data_ChannelListPane, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ChannelListPane::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ChannelListPane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ChannelListPane::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChannelListPane))
        return static_cast<void*>(const_cast< ChannelListPane*>(this));
    return QWidget::qt_metacast(_clname);
}

int ChannelListPane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ChannelListPane::rangeChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ChannelListPane::valueChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE

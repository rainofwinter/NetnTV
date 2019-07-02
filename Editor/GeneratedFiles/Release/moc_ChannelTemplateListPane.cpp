/****************************************************************************
** Meta object code from reading C++ file 'ChannelTemplateListPane.h'
**
** Created: Thu Aug 6 11:16:23 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../ChannelTemplateListPane.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ChannelTemplateListPane.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ChannelTemplateListModel[] = {

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
      26,   25,   25,   25, 0x0a,
      35,   25,   25,   25, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ChannelTemplateListModel[] = {
    "ChannelTemplateListModel\0\0update()\0"
    "onChannelTemplateListChanged()\0"
};

void ChannelTemplateListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChannelTemplateListModel *_t = static_cast<ChannelTemplateListModel *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        case 1: _t->onChannelTemplateListChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ChannelTemplateListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ChannelTemplateListModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_ChannelTemplateListModel,
      qt_meta_data_ChannelTemplateListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ChannelTemplateListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ChannelTemplateListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ChannelTemplateListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChannelTemplateListModel))
        return static_cast<void*>(const_cast< ChannelTemplateListModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int ChannelTemplateListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_ChannelTemplateListDelegate[] = {

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
      41,   29,   28,   28, 0x09,
      99,   28,   28,   28, 0x09,
     122,   28,   28,   28, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ChannelTemplateListDelegate[] = {
    "ChannelTemplateListDelegate\0\0editor,hint\0"
    "editorClosed(QWidget*,QAbstractItemDelegate::EndEditHint)\0"
    "commitAndCloseEditor()\0cancelEdit()\0"
};

void ChannelTemplateListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChannelTemplateListDelegate *_t = static_cast<ChannelTemplateListDelegate *>(_o);
        switch (_id) {
        case 0: _t->editorClosed((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< QAbstractItemDelegate::EndEditHint(*)>(_a[2]))); break;
        case 1: _t->commitAndCloseEditor(); break;
        case 2: _t->cancelEdit(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ChannelTemplateListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ChannelTemplateListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_ChannelTemplateListDelegate,
      qt_meta_data_ChannelTemplateListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ChannelTemplateListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ChannelTemplateListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ChannelTemplateListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChannelTemplateListDelegate))
        return static_cast<void*>(const_cast< ChannelTemplateListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int ChannelTemplateListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_ChannelTemplateListTreeView[] = {

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
      29,   28,   28,   28, 0x09,
      57,   28,   28,   28, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ChannelTemplateListTreeView[] = {
    "ChannelTemplateListTreeView\0\0"
    "updateChannelTemplateList()\0onCopy()\0"
};

void ChannelTemplateListTreeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChannelTemplateListTreeView *_t = static_cast<ChannelTemplateListTreeView *>(_o);
        switch (_id) {
        case 0: _t->updateChannelTemplateList(); break;
        case 1: _t->onCopy(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ChannelTemplateListTreeView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ChannelTemplateListTreeView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_ChannelTemplateListTreeView,
      qt_meta_data_ChannelTemplateListTreeView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ChannelTemplateListTreeView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ChannelTemplateListTreeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ChannelTemplateListTreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChannelTemplateListTreeView))
        return static_cast<void*>(const_cast< ChannelTemplateListTreeView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int ChannelTemplateListTreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_ChannelTemplateListPane[] = {

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
      25,   24,   24,   24, 0x0a,
      46,   24,   24,   24, 0x0a,
      57,   24,   24,   24, 0x0a,
      73,   24,   24,   24, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ChannelTemplateListPane[] = {
    "ChannelTemplateListPane\0\0changeButtonsState()\0"
    "onDelete()\0onSearchClear()\0onSearch(QString)\0"
};

void ChannelTemplateListPane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChannelTemplateListPane *_t = static_cast<ChannelTemplateListPane *>(_o);
        switch (_id) {
        case 0: _t->changeButtonsState(); break;
        case 1: _t->onDelete(); break;
        case 2: _t->onSearchClear(); break;
        case 3: _t->onSearch((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ChannelTemplateListPane::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ChannelTemplateListPane::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ChannelTemplateListPane,
      qt_meta_data_ChannelTemplateListPane, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ChannelTemplateListPane::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ChannelTemplateListPane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ChannelTemplateListPane::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChannelTemplateListPane))
        return static_cast<void*>(const_cast< ChannelTemplateListPane*>(this));
    return QWidget::qt_metacast(_clname);
}

int ChannelTemplateListPane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

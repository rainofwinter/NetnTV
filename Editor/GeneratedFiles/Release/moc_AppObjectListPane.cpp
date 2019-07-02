/****************************************************************************
** Meta object code from reading C++ file 'AppObjectListPane.h'
**
** Created: Thu Aug 6 11:16:30 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../AppObjectListPane.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AppObjectListPane.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AppObjectListModel[] = {

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

static const char qt_meta_stringdata_AppObjectListModel[] = {
    "AppObjectListModel\0\0update()\0"
};

void AppObjectListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AppObjectListModel *_t = static_cast<AppObjectListModel *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData AppObjectListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AppObjectListModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_AppObjectListModel,
      qt_meta_data_AppObjectListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AppObjectListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AppObjectListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AppObjectListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AppObjectListModel))
        return static_cast<void*>(const_cast< AppObjectListModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int AppObjectListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_AppObjectListDelegate[] = {

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
      35,   23,   22,   22, 0x09,
      93,   22,   22,   22, 0x09,
     116,   22,   22,   22, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_AppObjectListDelegate[] = {
    "AppObjectListDelegate\0\0editor,hint\0"
    "editorClosed(QWidget*,QAbstractItemDelegate::EndEditHint)\0"
    "commitAndCloseEditor()\0cancelEdit()\0"
};

void AppObjectListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AppObjectListDelegate *_t = static_cast<AppObjectListDelegate *>(_o);
        switch (_id) {
        case 0: _t->editorClosed((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< QAbstractItemDelegate::EndEditHint(*)>(_a[2]))); break;
        case 1: _t->commitAndCloseEditor(); break;
        case 2: _t->cancelEdit(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData AppObjectListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AppObjectListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_AppObjectListDelegate,
      qt_meta_data_AppObjectListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AppObjectListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AppObjectListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AppObjectListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AppObjectListDelegate))
        return static_cast<void*>(const_cast< AppObjectListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int AppObjectListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_AppObjectListTreeView[] = {

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
      23,   22,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
      45,   22,   22,   22, 0x09,
      63,   22,   22,   22, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_AppObjectListTreeView[] = {
    "AppObjectListTreeView\0\0uiPropertiesChanged()\0"
    "updateSelection()\0updateAppObjectList()\0"
};

void AppObjectListTreeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AppObjectListTreeView *_t = static_cast<AppObjectListTreeView *>(_o);
        switch (_id) {
        case 0: _t->uiPropertiesChanged(); break;
        case 1: _t->updateSelection(); break;
        case 2: _t->updateAppObjectList(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData AppObjectListTreeView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AppObjectListTreeView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_AppObjectListTreeView,
      qt_meta_data_AppObjectListTreeView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AppObjectListTreeView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AppObjectListTreeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AppObjectListTreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AppObjectListTreeView))
        return static_cast<void*>(const_cast< AppObjectListTreeView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int AppObjectListTreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void AppObjectListTreeView::uiPropertiesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_AppObjectListPane[] = {

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
      19,   18,   18,   18, 0x08,
      40,   18,   18,   18, 0x08,
      48,   18,   18,   18, 0x08,
      59,   18,   18,   18, 0x08,
      73,   18,   18,   18, 0x08,
      87,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_AppObjectListPane[] = {
    "AppObjectListPane\0\0changeButtonsState()\0"
    "onNew()\0onDelete()\0updateScene()\0"
    "onNewWebApp()\0onNewAppImage()\0"
};

void AppObjectListPane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AppObjectListPane *_t = static_cast<AppObjectListPane *>(_o);
        switch (_id) {
        case 0: _t->changeButtonsState(); break;
        case 1: _t->onNew(); break;
        case 2: _t->onDelete(); break;
        case 3: _t->updateScene(); break;
        case 4: _t->onNewWebApp(); break;
        case 5: _t->onNewAppImage(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData AppObjectListPane::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AppObjectListPane::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_AppObjectListPane,
      qt_meta_data_AppObjectListPane, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AppObjectListPane::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AppObjectListPane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AppObjectListPane::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AppObjectListPane))
        return static_cast<void*>(const_cast< AppObjectListPane*>(this));
    return QWidget::qt_metacast(_clname);
}

int AppObjectListPane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

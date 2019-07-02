/****************************************************************************
** Meta object code from reading C++ file 'SceneObjectListPane.h'
**
** Created: Thu Aug 6 11:16:30 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../SceneObjectListPane.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SceneObjectListPane.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SceneObjectListModel[] = {

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
      22,   21,   21,   21, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SceneObjectListModel[] = {
    "SceneObjectListModel\0\0update()\0"
};

void SceneObjectListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SceneObjectListModel *_t = static_cast<SceneObjectListModel *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SceneObjectListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SceneObjectListModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_SceneObjectListModel,
      qt_meta_data_SceneObjectListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SceneObjectListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SceneObjectListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SceneObjectListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SceneObjectListModel))
        return static_cast<void*>(const_cast< SceneObjectListModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int SceneObjectListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_SceneObjectListDelegate[] = {

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
      37,   25,   24,   24, 0x09,
      95,   24,   24,   24, 0x09,
     118,   24,   24,   24, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SceneObjectListDelegate[] = {
    "SceneObjectListDelegate\0\0editor,hint\0"
    "editorClosed(QWidget*,QAbstractItemDelegate::EndEditHint)\0"
    "commitAndCloseEditor()\0cancelEdit()\0"
};

void SceneObjectListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SceneObjectListDelegate *_t = static_cast<SceneObjectListDelegate *>(_o);
        switch (_id) {
        case 0: _t->editorClosed((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< QAbstractItemDelegate::EndEditHint(*)>(_a[2]))); break;
        case 1: _t->commitAndCloseEditor(); break;
        case 2: _t->cancelEdit(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SceneObjectListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SceneObjectListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_SceneObjectListDelegate,
      qt_meta_data_SceneObjectListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SceneObjectListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SceneObjectListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SceneObjectListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SceneObjectListDelegate))
        return static_cast<void*>(const_cast< SceneObjectListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int SceneObjectListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_SceneObjectListTreeView[] = {

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
      25,   24,   24,   24, 0x05,

 // slots: signature, parameters, type, tag, flags
      47,   24,   24,   24, 0x09,
      65,   24,   24,   24, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SceneObjectListTreeView[] = {
    "SceneObjectListTreeView\0\0uiPropertiesChanged()\0"
    "updateSelection()\0updateSceneObjectList()\0"
};

void SceneObjectListTreeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SceneObjectListTreeView *_t = static_cast<SceneObjectListTreeView *>(_o);
        switch (_id) {
        case 0: _t->uiPropertiesChanged(); break;
        case 1: _t->updateSelection(); break;
        case 2: _t->updateSceneObjectList(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SceneObjectListTreeView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SceneObjectListTreeView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_SceneObjectListTreeView,
      qt_meta_data_SceneObjectListTreeView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SceneObjectListTreeView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SceneObjectListTreeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SceneObjectListTreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SceneObjectListTreeView))
        return static_cast<void*>(const_cast< SceneObjectListTreeView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int SceneObjectListTreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void SceneObjectListTreeView::uiPropertiesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_SceneObjectListPane[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      27,   21,   20,   20, 0x08,
      51,   21,   20,   20, 0x08,
      76,   20,   20,   20, 0x08,
      97,   20,   20,   20, 0x08,
     105,   20,   20,   20, 0x08,
     116,   20,   20,   20, 0x08,
     137,  130,   20,   20, 0x08,
     158,   20,   20,   20, 0x08,
     171,   20,   20,   20, 0x08,
     184,   20,   20,   20, 0x08,
     201,   20,   20,   20, 0x08,
     217,   20,   20,   20, 0x08,
     229,   20,   20,   20, 0x08,
     249,   20,   20,   20, 0x08,
     263,   20,   20,   20, 0x08,
     279,   20,   20,   20, 0x08,
     292,   20,   20,   20, 0x08,
     305,   20,   20,   20, 0x08,
     321,   20,   20,   20, 0x08,
     337,   20,   20,   20, 0x08,
     354,   20,   20,   20, 0x08,
     370,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SceneObjectListPane[] = {
    "SceneObjectListPane\0\0index\0"
    "onExpanded(QModelIndex)\0"
    "onCollapsed(QModelIndex)\0changeButtonsState()\0"
    "onNew()\0onDelete()\0updateScene()\0"
    "action\0onQuickNew(QAction*)\0onNewImage()\0"
    "onNewGroup()\0onNewRectangle()\0"
    "onNewSubScene()\0onNewText()\0"
    "onNewImageChanger()\0onNewButton()\0"
    "onNewImageAni()\0onNewAudio()\0onNewVideo()\0"
    "onPageChanger()\0onBringFoward()\0"
    "onSendBackward()\0onSearchClear()\0"
    "onSearch(QString)\0"
};

void SceneObjectListPane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SceneObjectListPane *_t = static_cast<SceneObjectListPane *>(_o);
        switch (_id) {
        case 0: _t->onExpanded((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: _t->onCollapsed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: _t->changeButtonsState(); break;
        case 3: _t->onNew(); break;
        case 4: _t->onDelete(); break;
        case 5: _t->updateScene(); break;
        case 6: _t->onQuickNew((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 7: _t->onNewImage(); break;
        case 8: _t->onNewGroup(); break;
        case 9: _t->onNewRectangle(); break;
        case 10: _t->onNewSubScene(); break;
        case 11: _t->onNewText(); break;
        case 12: _t->onNewImageChanger(); break;
        case 13: _t->onNewButton(); break;
        case 14: _t->onNewImageAni(); break;
        case 15: _t->onNewAudio(); break;
        case 16: _t->onNewVideo(); break;
        case 17: _t->onPageChanger(); break;
        case 18: _t->onBringFoward(); break;
        case 19: _t->onSendBackward(); break;
        case 20: _t->onSearchClear(); break;
        case 21: _t->onSearch((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SceneObjectListPane::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SceneObjectListPane::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SceneObjectListPane,
      qt_meta_data_SceneObjectListPane, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SceneObjectListPane::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SceneObjectListPane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SceneObjectListPane::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SceneObjectListPane))
        return static_cast<void*>(const_cast< SceneObjectListPane*>(this));
    return QWidget::qt_metacast(_clname);
}

int SceneObjectListPane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

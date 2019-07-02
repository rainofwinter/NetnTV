/****************************************************************************
** Meta object code from reading C++ file 'ImageAnimationImagesDlg.h'
**
** Created: Thu Aug 6 11:16:26 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../ImageAnimationImagesDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ImageAnimationImagesDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ImageAnimationListModel[] = {

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

static const char qt_meta_stringdata_ImageAnimationListModel[] = {
    "ImageAnimationListModel\0"
};

void ImageAnimationListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ImageAnimationListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ImageAnimationListModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_ImageAnimationListModel,
      qt_meta_data_ImageAnimationListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ImageAnimationListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ImageAnimationListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ImageAnimationListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ImageAnimationListModel))
        return static_cast<void*>(const_cast< ImageAnimationListModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int ImageAnimationListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ImageAnimationListDelegate[] = {

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
      33,   28,   27,   27, 0x05,

 // slots: signature, parameters, type, tag, flags
      69,   27,   27,   27, 0x09,
      92,   27,   27,   27, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ImageAnimationListDelegate[] = {
    "ImageAnimationListDelegate\0\0item\0"
    "itemEdited(ImageAnimationListItem*)\0"
    "commitAndCloseEditor()\0cancelEdit()\0"
};

void ImageAnimationListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ImageAnimationListDelegate *_t = static_cast<ImageAnimationListDelegate *>(_o);
        switch (_id) {
        case 0: _t->itemEdited((*reinterpret_cast< ImageAnimationListItem*(*)>(_a[1]))); break;
        case 1: _t->commitAndCloseEditor(); break;
        case 2: _t->cancelEdit(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ImageAnimationListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ImageAnimationListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_ImageAnimationListDelegate,
      qt_meta_data_ImageAnimationListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ImageAnimationListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ImageAnimationListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ImageAnimationListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ImageAnimationListDelegate))
        return static_cast<void*>(const_cast< ImageAnimationListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int ImageAnimationListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ImageAnimationListDelegate::itemEdited(ImageAnimationListItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_ImageAnimationList[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x05,
      39,   19,   19,   19, 0x05,
      60,   55,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      96,   19,   19,   19, 0x09,
     114,   19,   19,   19, 0x09,
     123,   19,   19,   19, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ImageAnimationList[] = {
    "ImageAnimationList\0\0selectionChanged()\0"
    "layoutChanged()\0item\0"
    "itemEdited(ImageAnimationListItem*)\0"
    "onLayoutChanged()\0onCopy()\0onPaste()\0"
};

void ImageAnimationList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ImageAnimationList *_t = static_cast<ImageAnimationList *>(_o);
        switch (_id) {
        case 0: _t->selectionChanged(); break;
        case 1: _t->layoutChanged(); break;
        case 2: _t->itemEdited((*reinterpret_cast< ImageAnimationListItem*(*)>(_a[1]))); break;
        case 3: _t->onLayoutChanged(); break;
        case 4: _t->onCopy(); break;
        case 5: _t->onPaste(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ImageAnimationList::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ImageAnimationList::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_ImageAnimationList,
      qt_meta_data_ImageAnimationList, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ImageAnimationList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ImageAnimationList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ImageAnimationList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ImageAnimationList))
        return static_cast<void*>(const_cast< ImageAnimationList*>(this));
    return QTreeView::qt_metacast(_clname);
}

int ImageAnimationList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ImageAnimationList::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ImageAnimationList::layoutChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void ImageAnimationList::itemEdited(ImageAnimationListItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
static const uint qt_meta_data_ImageAnimationImagesDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      25,   24,   24,   24, 0x08,
      38,   24,   24,   24, 0x08,
      54,   24,   24,   24, 0x08,
      61,   24,   24,   24, 0x08,
      86,   24,   24,   24, 0x08,
     101,   24,   24,   24, 0x08,
     134,   24,   24,   24, 0x08,
     142,   24,   24,   24, 0x08,
     156,   24,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ImageAnimationImagesDlg[] = {
    "ImageAnimationImagesDlg\0\0onNewFiles()\0"
    "onDeleteFiles()\0onOk()\0onFileSelectionChanged()\0"
    "onChangeList()\0onResourceFileSelectionChanged()\0"
    "onNew()\0onAutoSlide()\0onAutoClick()\0"
};

void ImageAnimationImagesDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ImageAnimationImagesDlg *_t = static_cast<ImageAnimationImagesDlg *>(_o);
        switch (_id) {
        case 0: _t->onNewFiles(); break;
        case 1: _t->onDeleteFiles(); break;
        case 2: _t->onOk(); break;
        case 3: _t->onFileSelectionChanged(); break;
        case 4: _t->onChangeList(); break;
        case 5: _t->onResourceFileSelectionChanged(); break;
        case 6: _t->onNew(); break;
        case 7: _t->onAutoSlide(); break;
        case 8: _t->onAutoClick(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ImageAnimationImagesDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ImageAnimationImagesDlg::staticMetaObject = {
    { &resourceDialog::staticMetaObject, qt_meta_stringdata_ImageAnimationImagesDlg,
      qt_meta_data_ImageAnimationImagesDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ImageAnimationImagesDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ImageAnimationImagesDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ImageAnimationImagesDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ImageAnimationImagesDlg))
        return static_cast<void*>(const_cast< ImageAnimationImagesDlg*>(this));
    return resourceDialog::qt_metacast(_clname);
}

int ImageAnimationImagesDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = resourceDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

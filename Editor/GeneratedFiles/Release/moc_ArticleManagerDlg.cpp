/****************************************************************************
** Meta object code from reading C++ file 'ArticleManagerDlg.h'
**
** Created: Thu Aug 6 11:16:32 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../ArticleManagerDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ArticleManagerDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ArticleListModel[] = {

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

static const char qt_meta_stringdata_ArticleListModel[] = {
    "ArticleListModel\0"
};

void ArticleListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ArticleListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ArticleListModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_ArticleListModel,
      qt_meta_data_ArticleListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ArticleListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ArticleListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ArticleListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ArticleListModel))
        return static_cast<void*>(const_cast< ArticleListModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int ArticleListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ArticleListDelegate[] = {

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

static const char qt_meta_stringdata_ArticleListDelegate[] = {
    "ArticleListDelegate\0"
};

void ArticleListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ArticleListDelegate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ArticleListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_ArticleListDelegate,
      qt_meta_data_ArticleListDelegate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ArticleListDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ArticleListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ArticleListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ArticleListDelegate))
        return static_cast<void*>(const_cast< ArticleListDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int ArticleListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ArticleListView[] = {

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
      17,   16,   16,   16, 0x05,
      36,   16,   16,   16, 0x05,
      57,   52,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      86,   16,   16,   16, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ArticleListView[] = {
    "ArticleListView\0\0selectionChanged()\0"
    "layoutChanged()\0item\0itemEdited(GenericListItem*)\0"
    "onLayoutChanged()\0"
};

void ArticleListView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ArticleListView *_t = static_cast<ArticleListView *>(_o);
        switch (_id) {
        case 0: _t->selectionChanged(); break;
        case 1: _t->layoutChanged(); break;
        case 2: _t->itemEdited((*reinterpret_cast< GenericListItem*(*)>(_a[1]))); break;
        case 3: _t->onLayoutChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ArticleListView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ArticleListView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_ArticleListView,
      qt_meta_data_ArticleListView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ArticleListView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ArticleListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ArticleListView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ArticleListView))
        return static_cast<void*>(const_cast< ArticleListView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int ArticleListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ArticleListView::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ArticleListView::layoutChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void ArticleListView::itemEdited(GenericListItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
static const uint qt_meta_data_ArticleManagerDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x08,
      34,   18,   18,   18, 0x08,
      53,   18,   18,   18, 0x08,
      81,   18,   18,   18, 0x08,
     106,   18,   18,   18, 0x08,
     132,   18,   18,   18, 0x08,
     144,   18,   18,   18, 0x08,
     170,   18,   18,   18, 0x08,
     193,   18,   18,   18, 0x08,
     213,  208,   18,   18, 0x08,
     252,   18,   18,   18, 0x08,
     259,   18,   18,   18, 0x08,
     273,   18,   18,   18, 0x08,
     299,   18,   18,   18, 0x08,
     318,   18,   18,   18, 0x08,
     352,  346,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ArticleManagerDlg[] = {
    "ArticleManagerDlg\0\0onNewArticle()\0"
    "onDeleteArticles()\0onArticleSelectionChanged()\0"
    "onPageSelectionChanged()\0"
    "onSceneSelectionChanged()\0onAddPage()\0"
    "onArticlesLayoutChanged()\0"
    "onPagesLayoutChanged()\0onDeletePage()\0"
    "item\0onArticleNameChanged(GenericListItem*)\0"
    "onOk()\0onGenThumbs()\0onThumbnailFolderBrowse()\0"
    "onUiFolderBrowse()\0onContentsCheckBoxClicked()\0"
    "index\0onContentsPageChanged(int)\0"
};

void ArticleManagerDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ArticleManagerDlg *_t = static_cast<ArticleManagerDlg *>(_o);
        switch (_id) {
        case 0: _t->onNewArticle(); break;
        case 1: _t->onDeleteArticles(); break;
        case 2: _t->onArticleSelectionChanged(); break;
        case 3: _t->onPageSelectionChanged(); break;
        case 4: _t->onSceneSelectionChanged(); break;
        case 5: _t->onAddPage(); break;
        case 6: _t->onArticlesLayoutChanged(); break;
        case 7: _t->onPagesLayoutChanged(); break;
        case 8: _t->onDeletePage(); break;
        case 9: _t->onArticleNameChanged((*reinterpret_cast< GenericListItem*(*)>(_a[1]))); break;
        case 10: _t->onOk(); break;
        case 11: _t->onGenThumbs(); break;
        case 12: _t->onThumbnailFolderBrowse(); break;
        case 13: _t->onUiFolderBrowse(); break;
        case 14: _t->onContentsCheckBoxClicked(); break;
        case 15: _t->onContentsPageChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ArticleManagerDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ArticleManagerDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ArticleManagerDlg,
      qt_meta_data_ArticleManagerDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ArticleManagerDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ArticleManagerDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ArticleManagerDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ArticleManagerDlg))
        return static_cast<void*>(const_cast< ArticleManagerDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int ArticleManagerDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

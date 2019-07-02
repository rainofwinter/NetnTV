/****************************************************************************
** Meta object code from reading C++ file 'ImageGridDlg.h'
**
** Created: Thu Aug 6 11:16:33 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../ImageGridDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ImageGridDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ImageGridDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x08,
      27,   13,   13,   13, 0x08,
      43,   13,   13,   13, 0x08,
      50,   13,   13,   13, 0x08,
      75,   13,   13,   13, 0x08,
      90,   13,   13,   13, 0x08,
     106,   13,   13,   13, 0x08,
     124,   13,   13,   13, 0x08,
     157,   13,   13,   13, 0x08,
     165,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ImageGridDlg[] = {
    "ImageGridDlg\0\0onNewFiles()\0onDeleteFiles()\0"
    "onOk()\0onFileSelectionChanged()\0"
    "onChangeList()\0onSearchClear()\0"
    "onSearch(QString)\0onResourceFileSelectionChanged()\0"
    "onNew()\0onAutoSlide()\0"
};

void ImageGridDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ImageGridDlg *_t = static_cast<ImageGridDlg *>(_o);
        switch (_id) {
        case 0: _t->onNewFiles(); break;
        case 1: _t->onDeleteFiles(); break;
        case 2: _t->onOk(); break;
        case 3: _t->onFileSelectionChanged(); break;
        case 4: _t->onChangeList(); break;
        case 5: _t->onSearchClear(); break;
        case 6: _t->onSearch((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->onResourceFileSelectionChanged(); break;
        case 8: _t->onNew(); break;
        case 9: _t->onAutoSlide(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ImageGridDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ImageGridDlg::staticMetaObject = {
    { &resourceDialog::staticMetaObject, qt_meta_stringdata_ImageGridDlg,
      qt_meta_data_ImageGridDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ImageGridDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ImageGridDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ImageGridDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ImageGridDlg))
        return static_cast<void*>(const_cast< ImageGridDlg*>(this));
    return resourceDialog::qt_metacast(_clname);
}

int ImageGridDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = resourceDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

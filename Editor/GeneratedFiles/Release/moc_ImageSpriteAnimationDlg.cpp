/****************************************************************************
** Meta object code from reading C++ file 'ImageSpriteAnimationDlg.h'
**
** Created: Thu Aug 6 11:16:26 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../ImageSpriteAnimationDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ImageSpriteAnimationDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ImageSpriteAnimationDlg[] = {

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
      25,   24,   24,   24, 0x08,
      32,   24,   24,   24, 0x08,
      50,   24,   24,   24, 0x08,
      71,   24,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ImageSpriteAnimationDlg[] = {
    "ImageSpriteAnimationDlg\0\0onOk()\0"
    "setTextureAtlas()\0setTextureAtlasFps()\0"
    "setImageFile()\0"
};

void ImageSpriteAnimationDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ImageSpriteAnimationDlg *_t = static_cast<ImageSpriteAnimationDlg *>(_o);
        switch (_id) {
        case 0: _t->onOk(); break;
        case 1: _t->setTextureAtlas(); break;
        case 2: _t->setTextureAtlasFps(); break;
        case 3: _t->setImageFile(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ImageSpriteAnimationDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ImageSpriteAnimationDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ImageSpriteAnimationDlg,
      qt_meta_data_ImageSpriteAnimationDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ImageSpriteAnimationDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ImageSpriteAnimationDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ImageSpriteAnimationDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ImageSpriteAnimationDlg))
        return static_cast<void*>(const_cast< ImageSpriteAnimationDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int ImageSpriteAnimationDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
static const uint qt_meta_data_ImageSpriteWidget[] = {

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

static const char qt_meta_stringdata_ImageSpriteWidget[] = {
    "ImageSpriteWidget\0"
};

void ImageSpriteWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ImageSpriteWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ImageSpriteWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ImageSpriteWidget,
      qt_meta_data_ImageSpriteWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ImageSpriteWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ImageSpriteWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ImageSpriteWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ImageSpriteWidget))
        return static_cast<void*>(const_cast< ImageSpriteWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int ImageSpriteWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE

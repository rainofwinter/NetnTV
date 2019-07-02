/****************************************************************************
** Meta object code from reading C++ file 'KeyFramePane.h'
**
** Created: Thu Aug 6 11:16:31 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../KeyFramePane.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KeyFramePane.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KeyFramePropertyPage[] = {

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

static const char qt_meta_stringdata_KeyFramePropertyPage[] = {
    "KeyFramePropertyPage\0"
};

void KeyFramePropertyPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData KeyFramePropertyPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KeyFramePropertyPage::staticMetaObject = {
    { &PropertyPage::staticMetaObject, qt_meta_stringdata_KeyFramePropertyPage,
      qt_meta_data_KeyFramePropertyPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KeyFramePropertyPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KeyFramePropertyPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KeyFramePropertyPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KeyFramePropertyPage))
        return static_cast<void*>(const_cast< KeyFramePropertyPage*>(this));
    return PropertyPage::qt_metacast(_clname);
}

int KeyFramePropertyPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PropertyPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_KeyFramePane[] = {

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
      14,   13,   13,   13, 0x08,
      35,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_KeyFramePane[] = {
    "KeyFramePane\0\0changeButtonsState()\0"
    "keyFrameSelectionChanged()\0"
};

void KeyFramePane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KeyFramePane *_t = static_cast<KeyFramePane *>(_o);
        switch (_id) {
        case 0: _t->changeButtonsState(); break;
        case 1: _t->keyFrameSelectionChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData KeyFramePane::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KeyFramePane::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KeyFramePane,
      qt_meta_data_KeyFramePane, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KeyFramePane::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KeyFramePane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KeyFramePane::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KeyFramePane))
        return static_cast<void*>(const_cast< KeyFramePane*>(this));
    return QWidget::qt_metacast(_clname);
}

int KeyFramePane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

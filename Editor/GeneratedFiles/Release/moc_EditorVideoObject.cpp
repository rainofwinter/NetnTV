/****************************************************************************
** Meta object code from reading C++ file 'EditorVideoObject.h'
**
** Created: Thu Aug 6 11:16:39 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../EditorVideoObject.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditorVideoObject.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EditorVideoObjectPropertyPage[] = {

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
      31,   30,   30,   30, 0x08,
      53,   30,   30,   30, 0x08,
      69,   30,   30,   30, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EditorVideoObjectPropertyPage[] = {
    "EditorVideoObjectPropertyPage\0\0"
    "onInteractiveRegion()\0onToolChanged()\0"
    "onUrl()\0"
};

void EditorVideoObjectPropertyPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditorVideoObjectPropertyPage *_t = static_cast<EditorVideoObjectPropertyPage *>(_o);
        switch (_id) {
        case 0: _t->onInteractiveRegion(); break;
        case 1: _t->onToolChanged(); break;
        case 2: _t->onUrl(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData EditorVideoObjectPropertyPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditorVideoObjectPropertyPage::staticMetaObject = {
    { &PropertyPage::staticMetaObject, qt_meta_stringdata_EditorVideoObjectPropertyPage,
      qt_meta_data_EditorVideoObjectPropertyPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditorVideoObjectPropertyPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditorVideoObjectPropertyPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditorVideoObjectPropertyPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditorVideoObjectPropertyPage))
        return static_cast<void*>(const_cast< EditorVideoObjectPropertyPage*>(this));
    return PropertyPage::qt_metacast(_clname);
}

int EditorVideoObjectPropertyPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PropertyPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

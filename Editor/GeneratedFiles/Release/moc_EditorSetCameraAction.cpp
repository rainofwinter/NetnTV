/****************************************************************************
** Meta object code from reading C++ file 'EditorSetCameraAction.h'
**
** Created: Thu Aug 6 11:16:40 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../EditorSetCameraAction.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditorSetCameraAction.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EditorSetCameraPropertyPage[] = {

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
      29,   28,   28,   28, 0x0a,
      47,   38,   28,   28, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_EditorSetCameraPropertyPage[] = {
    "EditorSetCameraPropertyPage\0\0update()\0"
    "property\0onChanged(Property*)\0"
};

void EditorSetCameraPropertyPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditorSetCameraPropertyPage *_t = static_cast<EditorSetCameraPropertyPage *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        case 1: _t->onChanged((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EditorSetCameraPropertyPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditorSetCameraPropertyPage::staticMetaObject = {
    { &ActionPropertyPage::staticMetaObject, qt_meta_stringdata_EditorSetCameraPropertyPage,
      qt_meta_data_EditorSetCameraPropertyPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditorSetCameraPropertyPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditorSetCameraPropertyPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditorSetCameraPropertyPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditorSetCameraPropertyPage))
        return static_cast<void*>(const_cast< EditorSetCameraPropertyPage*>(this));
    return ActionPropertyPage::qt_metacast(_clname);
}

int EditorSetCameraPropertyPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ActionPropertyPage::qt_metacall(_c, _id, _a);
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

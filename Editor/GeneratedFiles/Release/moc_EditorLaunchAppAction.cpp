/****************************************************************************
** Meta object code from reading C++ file 'EditorLaunchAppAction.h'
**
** Created: Thu Aug 6 11:16:40 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../EditorLaunchAppAction.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditorLaunchAppAction.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EditorLaunchAppActionPropertyPage[] = {

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
      35,   34,   34,   34, 0x0a,
      53,   44,   34,   34, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_EditorLaunchAppActionPropertyPage[] = {
    "EditorLaunchAppActionPropertyPage\0\0"
    "update()\0property\0onChanged(Property*)\0"
};

void EditorLaunchAppActionPropertyPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditorLaunchAppActionPropertyPage *_t = static_cast<EditorLaunchAppActionPropertyPage *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        case 1: _t->onChanged((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EditorLaunchAppActionPropertyPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditorLaunchAppActionPropertyPage::staticMetaObject = {
    { &ActionPropertyPage::staticMetaObject, qt_meta_stringdata_EditorLaunchAppActionPropertyPage,
      qt_meta_data_EditorLaunchAppActionPropertyPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditorLaunchAppActionPropertyPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditorLaunchAppActionPropertyPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditorLaunchAppActionPropertyPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditorLaunchAppActionPropertyPage))
        return static_cast<void*>(const_cast< EditorLaunchAppActionPropertyPage*>(this));
    return ActionPropertyPage::qt_metacast(_clname);
}

int EditorLaunchAppActionPropertyPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

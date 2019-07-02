/****************************************************************************
** Meta object code from reading C++ file 'EditorSetVisibilityAction.h'
**
** Created: Thu Aug 6 11:16:40 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../EditorSetVisibilityAction.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditorSetVisibilityAction.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EditorSetVisibilityActionPropertyPage[] = {

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
      39,   38,   38,   38, 0x0a,
      57,   48,   38,   38, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_EditorSetVisibilityActionPropertyPage[] = {
    "EditorSetVisibilityActionPropertyPage\0"
    "\0update()\0property\0onChanged(Property*)\0"
};

void EditorSetVisibilityActionPropertyPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditorSetVisibilityActionPropertyPage *_t = static_cast<EditorSetVisibilityActionPropertyPage *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        case 1: _t->onChanged((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EditorSetVisibilityActionPropertyPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditorSetVisibilityActionPropertyPage::staticMetaObject = {
    { &ActionPropertyPage::staticMetaObject, qt_meta_stringdata_EditorSetVisibilityActionPropertyPage,
      qt_meta_data_EditorSetVisibilityActionPropertyPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditorSetVisibilityActionPropertyPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditorSetVisibilityActionPropertyPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditorSetVisibilityActionPropertyPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditorSetVisibilityActionPropertyPage))
        return static_cast<void*>(const_cast< EditorSetVisibilityActionPropertyPage*>(this));
    return ActionPropertyPage::qt_metacast(_clname);
}

int EditorSetVisibilityActionPropertyPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

/****************************************************************************
** Meta object code from reading C++ file 'EditorSceneChangerSceneSetAction.h'
**
** Created: Tue Jun 2 17:20:05 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../EditorSceneChangerSceneSetAction.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditorSceneChangerSceneSetAction.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EditorSceneChangerSceneSetPropertyPage[] = {

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
      40,   39,   39,   39, 0x0a,
      58,   49,   39,   39, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_EditorSceneChangerSceneSetPropertyPage[] = {
    "EditorSceneChangerSceneSetPropertyPage\0"
    "\0update()\0property\0onChanged(Property*)\0"
};

void EditorSceneChangerSceneSetPropertyPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditorSceneChangerSceneSetPropertyPage *_t = static_cast<EditorSceneChangerSceneSetPropertyPage *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        case 1: _t->onChanged((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EditorSceneChangerSceneSetPropertyPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditorSceneChangerSceneSetPropertyPage::staticMetaObject = {
    { &ActionPropertyPage::staticMetaObject, qt_meta_stringdata_EditorSceneChangerSceneSetPropertyPage,
      qt_meta_data_EditorSceneChangerSceneSetPropertyPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditorSceneChangerSceneSetPropertyPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditorSceneChangerSceneSetPropertyPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditorSceneChangerSceneSetPropertyPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditorSceneChangerSceneSetPropertyPage))
        return static_cast<void*>(const_cast< EditorSceneChangerSceneSetPropertyPage*>(this));
    return ActionPropertyPage::qt_metacast(_clname);
}

int EditorSceneChangerSceneSetPropertyPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

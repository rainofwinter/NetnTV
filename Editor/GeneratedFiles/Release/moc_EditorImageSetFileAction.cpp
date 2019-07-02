/****************************************************************************
** Meta object code from reading C++ file 'EditorImageSetFileAction.h'
**
** Created: Thu Aug 6 11:16:39 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../EditorImageSetFileAction.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditorImageSetFileAction.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EditorImageSetFileActionPropertyPage[] = {

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
      38,   37,   37,   37, 0x0a,
      56,   47,   37,   37, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_EditorImageSetFileActionPropertyPage[] = {
    "EditorImageSetFileActionPropertyPage\0"
    "\0update()\0property\0onChanged(Property*)\0"
};

void EditorImageSetFileActionPropertyPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditorImageSetFileActionPropertyPage *_t = static_cast<EditorImageSetFileActionPropertyPage *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        case 1: _t->onChanged((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EditorImageSetFileActionPropertyPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditorImageSetFileActionPropertyPage::staticMetaObject = {
    { &ActionPropertyPage::staticMetaObject, qt_meta_stringdata_EditorImageSetFileActionPropertyPage,
      qt_meta_data_EditorImageSetFileActionPropertyPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditorImageSetFileActionPropertyPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditorImageSetFileActionPropertyPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditorImageSetFileActionPropertyPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditorImageSetFileActionPropertyPage))
        return static_cast<void*>(const_cast< EditorImageSetFileActionPropertyPage*>(this));
    return ActionPropertyPage::qt_metacast(_clname);
}

int EditorImageSetFileActionPropertyPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

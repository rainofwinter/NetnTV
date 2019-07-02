/****************************************************************************
** Meta object code from reading C++ file 'EditorReceiveMessageEvent.h'
**
** Created: Thu Aug 6 11:16:40 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../EditorReceiveMessageEvent.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditorReceiveMessageEvent.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EditorReceiveMessageEventPropertyPage[] = {

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

static const char qt_meta_stringdata_EditorReceiveMessageEventPropertyPage[] = {
    "EditorReceiveMessageEventPropertyPage\0"
    "\0update()\0property\0onChanged(Property*)\0"
};

void EditorReceiveMessageEventPropertyPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditorReceiveMessageEventPropertyPage *_t = static_cast<EditorReceiveMessageEventPropertyPage *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        case 1: _t->onChanged((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EditorReceiveMessageEventPropertyPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditorReceiveMessageEventPropertyPage::staticMetaObject = {
    { &EventPropertyPage::staticMetaObject, qt_meta_stringdata_EditorReceiveMessageEventPropertyPage,
      qt_meta_data_EditorReceiveMessageEventPropertyPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditorReceiveMessageEventPropertyPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditorReceiveMessageEventPropertyPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditorReceiveMessageEventPropertyPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditorReceiveMessageEventPropertyPage))
        return static_cast<void*>(const_cast< EditorReceiveMessageEventPropertyPage*>(this));
    return EventPropertyPage::qt_metacast(_clname);
}

int EditorReceiveMessageEventPropertyPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EventPropertyPage::qt_metacall(_c, _id, _a);
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

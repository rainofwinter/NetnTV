/****************************************************************************
** Meta object code from reading C++ file 'PropertyPane.h'
**
** Created: Thu Aug 6 11:16:30 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../PropertyPane.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PropertyPane.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PropertyPane[] = {

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
      14,   13,   13,   13, 0x0a,
      39,   13,   13,   13, 0x0a,
      67,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PropertyPane[] = {
    "PropertyPane\0\0objectSelectionChanged()\0"
    "appObjectSelectionChanged()\0updateCurPage()\0"
};

void PropertyPane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PropertyPane *_t = static_cast<PropertyPane *>(_o);
        switch (_id) {
        case 0: _t->objectSelectionChanged(); break;
        case 1: _t->appObjectSelectionChanged(); break;
        case 2: _t->updateCurPage(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData PropertyPane::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PropertyPane::staticMetaObject = {
    { &QScrollArea::staticMetaObject, qt_meta_stringdata_PropertyPane,
      qt_meta_data_PropertyPane, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PropertyPane::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PropertyPane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PropertyPane::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PropertyPane))
        return static_cast<void*>(const_cast< PropertyPane*>(this));
    return QScrollArea::qt_metacast(_clname);
}

int PropertyPane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScrollArea::qt_metacall(_c, _id, _a);
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

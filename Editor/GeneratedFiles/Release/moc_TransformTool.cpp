/****************************************************************************
** Meta object code from reading C++ file 'TransformTool.h'
**
** Created: Thu Aug 6 11:16:31 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../TransformTool.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TransformTool.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TransformTool[] = {

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
      15,   14,   14,   14, 0x08,
      33,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TransformTool[] = {
    "TransformTool\0\0onObjectChanged()\0"
    "initTransformObjs()\0"
};

void TransformTool::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TransformTool *_t = static_cast<TransformTool *>(_o);
        switch (_id) {
        case 0: _t->onObjectChanged(); break;
        case 1: _t->initTransformObjs(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData TransformTool::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TransformTool::staticMetaObject = {
    { &Tool::staticMetaObject, qt_meta_stringdata_TransformTool,
      qt_meta_data_TransformTool, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TransformTool::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TransformTool::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TransformTool::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TransformTool))
        return static_cast<void*>(const_cast< TransformTool*>(this));
    return Tool::qt_metacast(_clname);
}

int TransformTool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Tool::qt_metacall(_c, _id, _a);
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

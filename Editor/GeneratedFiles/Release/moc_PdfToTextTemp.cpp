/****************************************************************************
** Meta object code from reading C++ file 'PdfToTextTemp.h'
**
** Created: Tue Jun 2 17:19:58 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../PdfToTextTemp.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PdfToTextTemp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PdfToTextTemp[] = {

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
      15,   14,   14,   14, 0x09,
      36,   14,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_PdfToTextTemp[] = {
    "PdfToTextTemp\0\0onProcessSucessOut()\0"
    "onProcessErrorOut()\0"
};

void PdfToTextTemp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PdfToTextTemp *_t = static_cast<PdfToTextTemp *>(_o);
        switch (_id) {
        case 0: _t->onProcessSucessOut(); break;
        case 1: _t->onProcessErrorOut(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData PdfToTextTemp::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PdfToTextTemp::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PdfToTextTemp,
      qt_meta_data_PdfToTextTemp, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PdfToTextTemp::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PdfToTextTemp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PdfToTextTemp::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PdfToTextTemp))
        return static_cast<void*>(const_cast< PdfToTextTemp*>(this));
    return QObject::qt_metacast(_clname);
}

int PdfToTextTemp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

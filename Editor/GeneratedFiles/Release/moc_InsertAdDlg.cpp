/****************************************************************************
** Meta object code from reading C++ file 'InsertAdDlg.h'
**
** Created: Thu Aug 6 11:16:34 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../InsertAdDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'InsertAdDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_InsertAdDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      29,   12,   12,   12, 0x08,
      45,   12,   12,   12, 0x08,
      61,   12,   12,   12, 0x08,
      76,   12,   12,   12, 0x08,
      91,   12,   12,   12, 0x08,
     107,   12,   12,   12, 0x08,
     114,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_InsertAdDlg[] = {
    "InsertAdDlg\0\0onImage1Radio()\0"
    "onImage2Radio()\0onImage3Radio()\0"
    "onText1Radio()\0onText2Radio()\0"
    "onMobileRadio()\0onOk()\0onRegister()\0"
};

void InsertAdDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        InsertAdDlg *_t = static_cast<InsertAdDlg *>(_o);
        switch (_id) {
        case 0: _t->onImage1Radio(); break;
        case 1: _t->onImage2Radio(); break;
        case 2: _t->onImage3Radio(); break;
        case 3: _t->onText1Radio(); break;
        case 4: _t->onText2Radio(); break;
        case 5: _t->onMobileRadio(); break;
        case 6: _t->onOk(); break;
        case 7: _t->onRegister(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData InsertAdDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject InsertAdDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_InsertAdDlg,
      qt_meta_data_InsertAdDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &InsertAdDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *InsertAdDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *InsertAdDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_InsertAdDlg))
        return static_cast<void*>(const_cast< InsertAdDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int InsertAdDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

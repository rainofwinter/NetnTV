/****************************************************************************
** Meta object code from reading C++ file 'NewAppObjectDlg.h'
**
** Created: Thu Aug 6 11:16:35 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../NewAppObjectDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NewAppObjectDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NewAppObjectDlg[] = {

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
      17,   16,   16,   16, 0x08,
      47,   42,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NewAppObjectDlg[] = {
    "NewAppObjectDlg\0\0onItemSelectionChanged()\0"
    "item\0onItemDoubleClicked(QListWidgetItem*)\0"
};

void NewAppObjectDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NewAppObjectDlg *_t = static_cast<NewAppObjectDlg *>(_o);
        switch (_id) {
        case 0: _t->onItemSelectionChanged(); break;
        case 1: _t->onItemDoubleClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NewAppObjectDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NewAppObjectDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_NewAppObjectDlg,
      qt_meta_data_NewAppObjectDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NewAppObjectDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NewAppObjectDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NewAppObjectDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NewAppObjectDlg))
        return static_cast<void*>(const_cast< NewAppObjectDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int NewAppObjectDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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

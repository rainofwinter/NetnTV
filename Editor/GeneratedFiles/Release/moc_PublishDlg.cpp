/****************************************************************************
** Meta object code from reading C++ file 'PublishDlg.h'
**
** Created: Thu Aug 6 11:16:36 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../PublishDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PublishDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PublishDlg[] = {

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
      12,   11,   11,   11, 0x08,
      26,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PublishDlg[] = {
    "PublishDlg\0\0onZipBrowse()\0onOk()\0"
};

void PublishDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PublishDlg *_t = static_cast<PublishDlg *>(_o);
        switch (_id) {
        case 0: _t->onZipBrowse(); break;
        case 1: _t->onOk(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData PublishDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PublishDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PublishDlg,
      qt_meta_data_PublishDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PublishDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PublishDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PublishDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PublishDlg))
        return static_cast<void*>(const_cast< PublishDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int PublishDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

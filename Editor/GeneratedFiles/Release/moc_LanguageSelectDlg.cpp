/****************************************************************************
** Meta object code from reading C++ file 'LanguageSelectDlg.h'
**
** Created: Tue Jun 2 17:19:58 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../LanguageSelectDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LanguageSelectDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LanguageSelectDlg[] = {

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
      25,   19,   18,   18, 0x08,
      38,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_LanguageSelectDlg[] = {
    "LanguageSelectDlg\0\0index\0changed(int)\0"
    "onOk()\0"
};

void LanguageSelectDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        LanguageSelectDlg *_t = static_cast<LanguageSelectDlg *>(_o);
        switch (_id) {
        case 0: _t->changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->onOk(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData LanguageSelectDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject LanguageSelectDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_LanguageSelectDlg,
      qt_meta_data_LanguageSelectDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LanguageSelectDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LanguageSelectDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LanguageSelectDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LanguageSelectDlg))
        return static_cast<void*>(const_cast< LanguageSelectDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int LanguageSelectDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

/****************************************************************************
** Meta object code from reading C++ file 'ChangePasswordDlg.h'
**
** Created: Thu Aug 6 11:16:22 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../ChangePasswordDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ChangePasswordDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ChangePasswordDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x08,
      32,   18,   18,   18, 0x08,
      47,   41,   18,   18, 0x08,
      93,   81,   77,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ChangePasswordDlg[] = {
    "ChangePasswordDlg\0\0initDialog()\0"
    "onSend()\0reply\0replyFinished(QNetworkReply*)\0"
    "int\0replyString\0parsePage(QString)\0"
};

void ChangePasswordDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChangePasswordDlg *_t = static_cast<ChangePasswordDlg *>(_o);
        switch (_id) {
        case 0: _t->initDialog(); break;
        case 1: _t->onSend(); break;
        case 2: _t->replyFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 3: { int _r = _t->parsePage((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ChangePasswordDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ChangePasswordDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ChangePasswordDlg,
      qt_meta_data_ChangePasswordDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ChangePasswordDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ChangePasswordDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ChangePasswordDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChangePasswordDlg))
        return static_cast<void*>(const_cast< ChangePasswordDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int ChangePasswordDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

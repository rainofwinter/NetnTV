/****************************************************************************
** Meta object code from reading C++ file 'VersionCheckDlg.h'
**
** Created: Tue Jun 2 17:19:59 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../VersionCheckDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VersionCheckDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VersionCheckDlg[] = {

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
      23,   17,   16,   16, 0x08,
      55,   53,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_VersionCheckDlg[] = {
    "VersionCheckDlg\0\0reply\0"
    "replyFinished(QNetworkReply*)\0,\0"
    "downloadProgress(qint64,qint64)\0"
};

void VersionCheckDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VersionCheckDlg *_t = static_cast<VersionCheckDlg *>(_o);
        switch (_id) {
        case 0: _t->replyFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 1: _t->downloadProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VersionCheckDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VersionCheckDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_VersionCheckDlg,
      qt_meta_data_VersionCheckDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VersionCheckDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VersionCheckDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VersionCheckDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VersionCheckDlg))
        return static_cast<void*>(const_cast< VersionCheckDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int VersionCheckDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

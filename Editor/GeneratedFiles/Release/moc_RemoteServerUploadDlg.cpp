/****************************************************************************
** Meta object code from reading C++ file 'RemoteServerUploadDlg.h'
**
** Created: Tue Jun 2 17:19:58 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../RemoteServerUploadDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RemoteServerUploadDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RemoteServerUploadDlg[] = {

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
      23,   22,   22,   22, 0x08,
      40,   34,   22,   22, 0x08,
      70,   22,   22,   22, 0x08,
      90,   88,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_RemoteServerUploadDlg[] = {
    "RemoteServerUploadDlg\0\0onUpload()\0"
    "reply\0replyFinished(QNetworkReply*)\0"
    "onLineEditCheck()\0,\0uploadProgress(qint64,qint64)\0"
};

void RemoteServerUploadDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RemoteServerUploadDlg *_t = static_cast<RemoteServerUploadDlg *>(_o);
        switch (_id) {
        case 0: _t->onUpload(); break;
        case 1: _t->replyFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 2: _t->onLineEditCheck(); break;
        case 3: _t->uploadProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData RemoteServerUploadDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RemoteServerUploadDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_RemoteServerUploadDlg,
      qt_meta_data_RemoteServerUploadDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RemoteServerUploadDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RemoteServerUploadDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RemoteServerUploadDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RemoteServerUploadDlg))
        return static_cast<void*>(const_cast< RemoteServerUploadDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int RemoteServerUploadDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

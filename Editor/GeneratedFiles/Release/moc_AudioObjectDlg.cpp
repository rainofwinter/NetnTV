/****************************************************************************
** Meta object code from reading C++ file 'AudioObjectDlg.h'
**
** Created: Thu Aug 6 11:16:24 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../AudioObjectDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AudioObjectDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AudioObjectDlg[] = {

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
      16,   15,   15,   15, 0x08,
      30,   15,   15,   15, 0x08,
      37,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_AudioObjectDlg[] = {
    "AudioObjectDlg\0\0onAudioFile()\0onOk()\0"
    "onStartPlay()\0"
};

void AudioObjectDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AudioObjectDlg *_t = static_cast<AudioObjectDlg *>(_o);
        switch (_id) {
        case 0: _t->onAudioFile(); break;
        case 1: _t->onOk(); break;
        case 2: _t->onStartPlay(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData AudioObjectDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AudioObjectDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_AudioObjectDlg,
      qt_meta_data_AudioObjectDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AudioObjectDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AudioObjectDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AudioObjectDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioObjectDlg))
        return static_cast<void*>(const_cast< AudioObjectDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int AudioObjectDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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

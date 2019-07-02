/****************************************************************************
** Meta object code from reading C++ file 'LoginDlg.h'
**
** Created: Thu Aug 6 11:16:25 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../LoginDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LoginDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LoginDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      25,    9,    9,    9, 0x0a,
      35,    9,    9,    9, 0x0a,
      52,   46,    9,    9, 0x08,
      82,    9,    9,    9, 0x08,
     106,    9,    9,    9, 0x08,
     136,  124,  120,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_LoginDlg[] = {
    "LoginDlg\0\0dialogClosed()\0onLogin()\0"
    "onLogout()\0reply\0replyFinished(QNetworkReply*)\0"
    "rememberIdChecked(bool)\0illegalQuit()\0"
    "int\0replyString\0parsePage(QString)\0"
};

void LoginDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        LoginDlg *_t = static_cast<LoginDlg *>(_o);
        switch (_id) {
        case 0: _t->dialogClosed(); break;
        case 1: _t->onLogin(); break;
        case 2: _t->onLogout(); break;
        case 3: _t->replyFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 4: _t->rememberIdChecked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->illegalQuit(); break;
        case 6: { int _r = _t->parsePage((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData LoginDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject LoginDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_LoginDlg,
      qt_meta_data_LoginDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LoginDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LoginDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LoginDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LoginDlg))
        return static_cast<void*>(const_cast< LoginDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int LoginDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void LoginDlg::dialogClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE

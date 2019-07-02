/****************************************************************************
** Meta object code from reading C++ file 'EpubParser.h'
**
** Created: Tue Jun 2 17:19:57 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../EpubParser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EpubParser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_XmlOrganization[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_XmlOrganization[] = {
    "XmlOrganization\0\0"
    "CompleteOrganization(XmlOrganization*)\0"
};

void XmlOrganization::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        XmlOrganization *_t = static_cast<XmlOrganization *>(_o);
        switch (_id) {
        case 0: _t->CompleteOrganization((*reinterpret_cast< XmlOrganization*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData XmlOrganization::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject XmlOrganization::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_XmlOrganization,
      qt_meta_data_XmlOrganization, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &XmlOrganization::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *XmlOrganization::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *XmlOrganization::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_XmlOrganization))
        return static_cast<void*>(const_cast< XmlOrganization*>(this));
    return QThread::qt_metacast(_clname);
}

int XmlOrganization::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void XmlOrganization::CompleteOrganization(XmlOrganization * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_EpubParser[] = {

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
      12,   11,   11,   11, 0x0a,
      42,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_EpubParser[] = {
    "EpubParser\0\0on_webView_loadFinished(bool)\0"
    "ThreadEnd(XmlOrganization*)\0"
};

void EpubParser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EpubParser *_t = static_cast<EpubParser *>(_o);
        switch (_id) {
        case 0: _t->on_webView_loadFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->ThreadEnd((*reinterpret_cast< XmlOrganization*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EpubParser::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EpubParser::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_EpubParser,
      qt_meta_data_EpubParser, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EpubParser::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EpubParser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EpubParser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EpubParser))
        return static_cast<void*>(const_cast< EpubParser*>(this));
    return QObject::qt_metacast(_clname);
}

int EpubParser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

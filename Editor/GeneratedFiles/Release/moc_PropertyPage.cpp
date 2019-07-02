/****************************************************************************
** Meta object code from reading C++ file 'PropertyPage.h'
**
** Created: Thu Aug 6 11:16:30 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../PropertyPage.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PropertyPage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PropertyPage[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      42,   13,   13,   13, 0x0a,
      51,   14,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PropertyPage[] = {
    "PropertyPage\0\0property\0changed(Property*)\0"
    "update()\0onChanged(Property*)\0"
};

void PropertyPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PropertyPage *_t = static_cast<PropertyPage *>(_o);
        switch (_id) {
        case 0: _t->changed((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        case 1: _t->update(); break;
        case 2: _t->onChanged((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PropertyPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PropertyPage::staticMetaObject = {
    { &Property::staticMetaObject, qt_meta_stringdata_PropertyPage,
      qt_meta_data_PropertyPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PropertyPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PropertyPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PropertyPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PropertyPage))
        return static_cast<void*>(const_cast< PropertyPage*>(this));
    return Property::qt_metacast(_clname);
}

int PropertyPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Property::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void PropertyPage::changed(Property * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_CorePropertiesPage[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      29,   20,   19,   19, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_CorePropertiesPage[] = {
    "CorePropertiesPage\0\0property\0"
    "onChanged(Property*)\0"
};

void CorePropertiesPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CorePropertiesPage *_t = static_cast<CorePropertiesPage *>(_o);
        switch (_id) {
        case 0: _t->onChanged((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CorePropertiesPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CorePropertiesPage::staticMetaObject = {
    { &PropertyPage::staticMetaObject, qt_meta_stringdata_CorePropertiesPage,
      qt_meta_data_CorePropertiesPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CorePropertiesPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CorePropertiesPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CorePropertiesPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CorePropertiesPage))
        return static_cast<void*>(const_cast< CorePropertiesPage*>(this));
    return PropertyPage::qt_metacast(_clname);
}

int CorePropertiesPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PropertyPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_CoreAppObjectPropertiesPage[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      38,   29,   28,   28, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_CoreAppObjectPropertiesPage[] = {
    "CoreAppObjectPropertiesPage\0\0property\0"
    "onChanged(Property*)\0"
};

void CoreAppObjectPropertiesPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CoreAppObjectPropertiesPage *_t = static_cast<CoreAppObjectPropertiesPage *>(_o);
        switch (_id) {
        case 0: _t->onChanged((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CoreAppObjectPropertiesPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CoreAppObjectPropertiesPage::staticMetaObject = {
    { &PropertyPage::staticMetaObject, qt_meta_stringdata_CoreAppObjectPropertiesPage,
      qt_meta_data_CoreAppObjectPropertiesPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CoreAppObjectPropertiesPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CoreAppObjectPropertiesPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CoreAppObjectPropertiesPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CoreAppObjectPropertiesPage))
        return static_cast<void*>(const_cast< CoreAppObjectPropertiesPage*>(this));
    return PropertyPage::qt_metacast(_clname);
}

int CoreAppObjectPropertiesPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PropertyPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_ScalePropertiesPage[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_ScalePropertiesPage[] = {
    "ScalePropertiesPage\0"
};

void ScalePropertiesPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ScalePropertiesPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ScalePropertiesPage::staticMetaObject = {
    { &PropertyPage::staticMetaObject, qt_meta_stringdata_ScalePropertiesPage,
      qt_meta_data_ScalePropertiesPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ScalePropertiesPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ScalePropertiesPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ScalePropertiesPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ScalePropertiesPage))
        return static_cast<void*>(const_cast< ScalePropertiesPage*>(this));
    return PropertyPage::qt_metacast(_clname);
}

int ScalePropertiesPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PropertyPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ScaleAppObjectPropertiesPage[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_ScaleAppObjectPropertiesPage[] = {
    "ScaleAppObjectPropertiesPage\0"
};

void ScaleAppObjectPropertiesPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ScaleAppObjectPropertiesPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ScaleAppObjectPropertiesPage::staticMetaObject = {
    { &PropertyPage::staticMetaObject, qt_meta_stringdata_ScaleAppObjectPropertiesPage,
      qt_meta_data_ScaleAppObjectPropertiesPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ScaleAppObjectPropertiesPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ScaleAppObjectPropertiesPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ScaleAppObjectPropertiesPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ScaleAppObjectPropertiesPage))
        return static_cast<void*>(const_cast< ScaleAppObjectPropertiesPage*>(this));
    return PropertyPage::qt_metacast(_clname);
}

int ScaleAppObjectPropertiesPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PropertyPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_TransformPropertiesPage[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_TransformPropertiesPage[] = {
    "TransformPropertiesPage\0"
};

void TransformPropertiesPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData TransformPropertiesPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TransformPropertiesPage::staticMetaObject = {
    { &PropertyPage::staticMetaObject, qt_meta_stringdata_TransformPropertiesPage,
      qt_meta_data_TransformPropertiesPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TransformPropertiesPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TransformPropertiesPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TransformPropertiesPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TransformPropertiesPage))
        return static_cast<void*>(const_cast< TransformPropertiesPage*>(this));
    return PropertyPage::qt_metacast(_clname);
}

int TransformPropertiesPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PropertyPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_TransformAppObjectPropertiesPage[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_TransformAppObjectPropertiesPage[] = {
    "TransformAppObjectPropertiesPage\0"
};

void TransformAppObjectPropertiesPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData TransformAppObjectPropertiesPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TransformAppObjectPropertiesPage::staticMetaObject = {
    { &PropertyPage::staticMetaObject, qt_meta_stringdata_TransformAppObjectPropertiesPage,
      qt_meta_data_TransformAppObjectPropertiesPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TransformAppObjectPropertiesPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TransformAppObjectPropertiesPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TransformAppObjectPropertiesPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TransformAppObjectPropertiesPage))
        return static_cast<void*>(const_cast< TransformAppObjectPropertiesPage*>(this));
    return PropertyPage::qt_metacast(_clname);
}

int TransformAppObjectPropertiesPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PropertyPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_VisualPropertiesPage[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      31,   22,   21,   21, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_VisualPropertiesPage[] = {
    "VisualPropertiesPage\0\0property\0"
    "onChanged(Property*)\0"
};

void VisualPropertiesPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VisualPropertiesPage *_t = static_cast<VisualPropertiesPage *>(_o);
        switch (_id) {
        case 0: _t->onChanged((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VisualPropertiesPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VisualPropertiesPage::staticMetaObject = {
    { &PropertyPage::staticMetaObject, qt_meta_stringdata_VisualPropertiesPage,
      qt_meta_data_VisualPropertiesPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VisualPropertiesPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VisualPropertiesPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VisualPropertiesPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VisualPropertiesPage))
        return static_cast<void*>(const_cast< VisualPropertiesPage*>(this));
    return PropertyPage::qt_metacast(_clname);
}

int VisualPropertiesPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PropertyPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_VisualAppObjectPropertiesPage[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      40,   31,   30,   30, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_VisualAppObjectPropertiesPage[] = {
    "VisualAppObjectPropertiesPage\0\0property\0"
    "onChanged(Property*)\0"
};

void VisualAppObjectPropertiesPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VisualAppObjectPropertiesPage *_t = static_cast<VisualAppObjectPropertiesPage *>(_o);
        switch (_id) {
        case 0: _t->onChanged((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VisualAppObjectPropertiesPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VisualAppObjectPropertiesPage::staticMetaObject = {
    { &PropertyPage::staticMetaObject, qt_meta_stringdata_VisualAppObjectPropertiesPage,
      qt_meta_data_VisualAppObjectPropertiesPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VisualAppObjectPropertiesPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VisualAppObjectPropertiesPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VisualAppObjectPropertiesPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VisualAppObjectPropertiesPage))
        return static_cast<void*>(const_cast< VisualAppObjectPropertiesPage*>(this));
    return PropertyPage::qt_metacast(_clname);
}

int VisualAppObjectPropertiesPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PropertyPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

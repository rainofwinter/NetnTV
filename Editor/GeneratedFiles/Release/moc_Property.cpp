/****************************************************************************
** Meta object code from reading C++ file 'Property.h'
**
** Created: Thu Aug 6 11:16:30 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../Property.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Property.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Property[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   10,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      38,    9,    9,    9, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_Property[] = {
    "Property\0\0property\0changed(Property*)\0"
    "changed()\0"
};

void Property::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Property *_t = static_cast<Property *>(_o);
        switch (_id) {
        case 0: _t->changed((*reinterpret_cast< Property*(*)>(_a[1]))); break;
        case 1: _t->changed(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Property::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Property::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Property,
      qt_meta_data_Property, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Property::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Property::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Property::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Property))
        return static_cast<void*>(const_cast< Property*>(this));
    return QObject::qt_metacast(_clname);
}

int Property::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void Property::changed(Property * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_EditProperty[] = {

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
      14,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_EditProperty[] = {
    "EditProperty\0\0changed()\0"
};

void EditProperty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditProperty *_t = static_cast<EditProperty *>(_o);
        switch (_id) {
        case 0: _t->changed(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData EditProperty::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditProperty::staticMetaObject = {
    { &Property::staticMetaObject, qt_meta_stringdata_EditProperty,
      qt_meta_data_EditProperty, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditProperty::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditProperty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditProperty::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditProperty))
        return static_cast<void*>(const_cast< EditProperty*>(this));
    return Property::qt_metacast(_clname);
}

int EditProperty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Property::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_PivotEditProperty[] = {

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
      19,   18,   18,   18, 0x09,
      29,   18,   18,   18, 0x09,
      43,   18,   18,   18, 0x09,
      60,   18,   18,   18, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_PivotEditProperty[] = {
    "PivotEditProperty\0\0changed()\0onChangeMin()\0"
    "onChangeCenter()\0onChangeMax()\0"
};

void PivotEditProperty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PivotEditProperty *_t = static_cast<PivotEditProperty *>(_o);
        switch (_id) {
        case 0: _t->changed(); break;
        case 1: _t->onChangeMin(); break;
        case 2: _t->onChangeCenter(); break;
        case 3: _t->onChangeMax(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData PivotEditProperty::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PivotEditProperty::staticMetaObject = {
    { &Property::staticMetaObject, qt_meta_stringdata_PivotEditProperty,
      qt_meta_data_PivotEditProperty, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PivotEditProperty::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PivotEditProperty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PivotEditProperty::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PivotEditProperty))
        return static_cast<void*>(const_cast< PivotEditProperty*>(this));
    return Property::qt_metacast(_clname);
}

int PivotEditProperty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Property::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
static const uint qt_meta_data_EditTextPropertyWidget[] = {

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
      24,   23,   23,   23, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_EditTextPropertyWidget[] = {
    "EditTextPropertyWidget\0\0editingFinished()\0"
};

void EditTextPropertyWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditTextPropertyWidget *_t = static_cast<EditTextPropertyWidget *>(_o);
        switch (_id) {
        case 0: _t->editingFinished(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData EditTextPropertyWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditTextPropertyWidget::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_EditTextPropertyWidget,
      qt_meta_data_EditTextPropertyWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditTextPropertyWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditTextPropertyWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditTextPropertyWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditTextPropertyWidget))
        return static_cast<void*>(const_cast< EditTextPropertyWidget*>(this));
    return QTextEdit::qt_metacast(_clname);
}

int EditTextPropertyWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
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
void EditTextPropertyWidget::editingFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_EditTextProperty[] = {

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
      18,   17,   17,   17, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_EditTextProperty[] = {
    "EditTextProperty\0\0changed()\0"
};

void EditTextProperty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditTextProperty *_t = static_cast<EditTextProperty *>(_o);
        switch (_id) {
        case 0: _t->changed(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData EditTextProperty::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditTextProperty::staticMetaObject = {
    { &Property::staticMetaObject, qt_meta_stringdata_EditTextProperty,
      qt_meta_data_EditTextProperty, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditTextProperty::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditTextProperty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditTextProperty::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditTextProperty))
        return static_cast<void*>(const_cast< EditTextProperty*>(this));
    return Property::qt_metacast(_clname);
}

int EditTextProperty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Property::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_ComboBoxProperty[] = {

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
      24,   18,   17,   17, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ComboBoxProperty[] = {
    "ComboBoxProperty\0\0index\0changed(int)\0"
};

void ComboBoxProperty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ComboBoxProperty *_t = static_cast<ComboBoxProperty *>(_o);
        switch (_id) {
        case 0: _t->changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ComboBoxProperty::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ComboBoxProperty::staticMetaObject = {
    { &Property::staticMetaObject, qt_meta_stringdata_ComboBoxProperty,
      qt_meta_data_ComboBoxProperty, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ComboBoxProperty::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ComboBoxProperty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ComboBoxProperty::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ComboBoxProperty))
        return static_cast<void*>(const_cast< ComboBoxProperty*>(this));
    return Property::qt_metacast(_clname);
}

int ComboBoxProperty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Property::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_FontProperty[] = {

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
      14,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FontProperty[] = {
    "FontProperty\0\0onClicked()\0"
};

void FontProperty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FontProperty *_t = static_cast<FontProperty *>(_o);
        switch (_id) {
        case 0: _t->onClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FontProperty::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FontProperty::staticMetaObject = {
    { &Property::staticMetaObject, qt_meta_stringdata_FontProperty,
      qt_meta_data_FontProperty, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FontProperty::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FontProperty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FontProperty::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FontProperty))
        return static_cast<void*>(const_cast< FontProperty*>(this));
    return Property::qt_metacast(_clname);
}

int FontProperty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Property::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_RadioGroupProperty[] = {

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
      20,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_RadioGroupProperty[] = {
    "RadioGroupProperty\0\0onClicked()\0"
};

void RadioGroupProperty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RadioGroupProperty *_t = static_cast<RadioGroupProperty *>(_o);
        switch (_id) {
        case 0: _t->onClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData RadioGroupProperty::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RadioGroupProperty::staticMetaObject = {
    { &Property::staticMetaObject, qt_meta_stringdata_RadioGroupProperty,
      qt_meta_data_RadioGroupProperty, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RadioGroupProperty::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RadioGroupProperty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RadioGroupProperty::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RadioGroupProperty))
        return static_cast<void*>(const_cast< RadioGroupProperty*>(this));
    return Property::qt_metacast(_clname);
}

int RadioGroupProperty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Property::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_ButtonProperty[] = {

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
      16,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ButtonProperty[] = {
    "ButtonProperty\0\0onClicked()\0"
};

void ButtonProperty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ButtonProperty *_t = static_cast<ButtonProperty *>(_o);
        switch (_id) {
        case 0: _t->onClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ButtonProperty::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ButtonProperty::staticMetaObject = {
    { &Property::staticMetaObject, qt_meta_stringdata_ButtonProperty,
      qt_meta_data_ButtonProperty, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ButtonProperty::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ButtonProperty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ButtonProperty::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ButtonProperty))
        return static_cast<void*>(const_cast< ButtonProperty*>(this));
    return Property::qt_metacast(_clname);
}

int ButtonProperty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Property::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_ToolButtonProperty[] = {

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

static const char qt_meta_stringdata_ToolButtonProperty[] = {
    "ToolButtonProperty\0"
};

void ToolButtonProperty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ToolButtonProperty::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ToolButtonProperty::staticMetaObject = {
    { &ButtonProperty::staticMetaObject, qt_meta_stringdata_ToolButtonProperty,
      qt_meta_data_ToolButtonProperty, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ToolButtonProperty::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ToolButtonProperty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ToolButtonProperty::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ToolButtonProperty))
        return static_cast<void*>(const_cast< ToolButtonProperty*>(this));
    return ButtonProperty::qt_metacast(_clname);
}

int ToolButtonProperty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ButtonProperty::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ColorPropertyWidget[] = {

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
      21,   20,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
      31,   20,   20,   20, 0x0a,
      43,   20,   20,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ColorPropertyWidget[] = {
    "ColorPropertyWidget\0\0changed()\0"
    "onClicked()\0onColorSelected()\0"
};

void ColorPropertyWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ColorPropertyWidget *_t = static_cast<ColorPropertyWidget *>(_o);
        switch (_id) {
        case 0: _t->changed(); break;
        case 1: _t->onClicked(); break;
        case 2: _t->onColorSelected(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ColorPropertyWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ColorPropertyWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ColorPropertyWidget,
      qt_meta_data_ColorPropertyWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ColorPropertyWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ColorPropertyWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ColorPropertyWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ColorPropertyWidget))
        return static_cast<void*>(const_cast< ColorPropertyWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int ColorPropertyWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void ColorPropertyWidget::changed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_ColorProperty[] = {

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
      15,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ColorProperty[] = {
    "ColorProperty\0\0onChanged()\0"
};

void ColorProperty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ColorProperty *_t = static_cast<ColorProperty *>(_o);
        switch (_id) {
        case 0: _t->onChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ColorProperty::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ColorProperty::staticMetaObject = {
    { &Property::staticMetaObject, qt_meta_stringdata_ColorProperty,
      qt_meta_data_ColorProperty, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ColorProperty::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ColorProperty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ColorProperty::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ColorProperty))
        return static_cast<void*>(const_cast< ColorProperty*>(this));
    return Property::qt_metacast(_clname);
}

int ColorProperty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Property::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_BoolProperty[] = {

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
      18,   14,   13,   13, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_BoolProperty[] = {
    "BoolProperty\0\0val\0onChanged(bool)\0"
};

void BoolProperty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BoolProperty *_t = static_cast<BoolProperty *>(_o);
        switch (_id) {
        case 0: _t->onChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData BoolProperty::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BoolProperty::staticMetaObject = {
    { &Property::staticMetaObject, qt_meta_stringdata_BoolProperty,
      qt_meta_data_BoolProperty, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BoolProperty::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BoolProperty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BoolProperty::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BoolProperty))
        return static_cast<void*>(const_cast< BoolProperty*>(this));
    return Property::qt_metacast(_clname);
}

int BoolProperty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Property::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_FileNameProperty[] = {

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
      18,   17,   17,   17, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_FileNameProperty[] = {
    "FileNameProperty\0\0onChanged()\0"
};

void FileNameProperty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FileNameProperty *_t = static_cast<FileNameProperty *>(_o);
        switch (_id) {
        case 0: _t->onChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FileNameProperty::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FileNameProperty::staticMetaObject = {
    { &Property::staticMetaObject, qt_meta_stringdata_FileNameProperty,
      qt_meta_data_FileNameProperty, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FileNameProperty::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FileNameProperty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FileNameProperty::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FileNameProperty))
        return static_cast<void*>(const_cast< FileNameProperty*>(this));
    return Property::qt_metacast(_clname);
}

int FileNameProperty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Property::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_CheckBoxProperty[] = {

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
      22,   18,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CheckBoxProperty[] = {
    "CheckBoxProperty\0\0val\0onChanged(bool)\0"
};

void CheckBoxProperty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CheckBoxProperty *_t = static_cast<CheckBoxProperty *>(_o);
        switch (_id) {
        case 0: _t->onChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CheckBoxProperty::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CheckBoxProperty::staticMetaObject = {
    { &Property::staticMetaObject, qt_meta_stringdata_CheckBoxProperty,
      qt_meta_data_CheckBoxProperty, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CheckBoxProperty::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CheckBoxProperty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CheckBoxProperty::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CheckBoxProperty))
        return static_cast<void*>(const_cast< CheckBoxProperty*>(this));
    return Property::qt_metacast(_clname);
}

int CheckBoxProperty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Property::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_SetResourceFileProperty[] = {

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
      25,   24,   24,   24, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SetResourceFileProperty[] = {
    "SetResourceFileProperty\0\0onChanged()\0"
};

void SetResourceFileProperty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SetResourceFileProperty *_t = static_cast<SetResourceFileProperty *>(_o);
        switch (_id) {
        case 0: _t->onChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SetResourceFileProperty::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SetResourceFileProperty::staticMetaObject = {
    { &Property::staticMetaObject, qt_meta_stringdata_SetResourceFileProperty,
      qt_meta_data_SetResourceFileProperty, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SetResourceFileProperty::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SetResourceFileProperty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SetResourceFileProperty::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SetResourceFileProperty))
        return static_cast<void*>(const_cast< SetResourceFileProperty*>(this));
    return Property::qt_metacast(_clname);
}

int SetResourceFileProperty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Property::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_FontValidator[] = {

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

static const char qt_meta_stringdata_FontValidator[] = {
    "FontValidator\0"
};

void FontValidator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FontValidator::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FontValidator::staticMetaObject = {
    { &QDoubleValidator::staticMetaObject, qt_meta_stringdata_FontValidator,
      qt_meta_data_FontValidator, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FontValidator::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FontValidator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FontValidator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FontValidator))
        return static_cast<void*>(const_cast< FontValidator*>(this));
    return QDoubleValidator::qt_metacast(_clname);
}

int FontValidator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDoubleValidator::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE

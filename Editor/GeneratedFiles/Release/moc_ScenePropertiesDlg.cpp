/****************************************************************************
** Meta object code from reading C++ file 'ScenePropertiesDlg.h'
**
** Created: Thu Aug 6 11:16:37 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../ScenePropertiesDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ScenePropertiesDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ColorLabel[] = {

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

static const char qt_meta_stringdata_ColorLabel[] = {
    "ColorLabel\0"
};

void ColorLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ColorLabel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ColorLabel::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ColorLabel,
      qt_meta_data_ColorLabel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ColorLabel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ColorLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ColorLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ColorLabel))
        return static_cast<void*>(const_cast< ColorLabel*>(this));
    return QWidget::qt_metacast(_clname);
}

int ColorLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ScenePropertiesDlg[] = {

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
      20,   19,   19,   19, 0x09,
      44,   19,   19,   19, 0x09,
      51,   19,   19,   19, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ScenePropertiesDlg[] = {
    "ScenePropertiesDlg\0\0onInputChanged(QString)\0"
    "onOk()\0onSetColor()\0"
};

void ScenePropertiesDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ScenePropertiesDlg *_t = static_cast<ScenePropertiesDlg *>(_o);
        switch (_id) {
        case 0: _t->onInputChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->onOk(); break;
        case 2: _t->onSetColor(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ScenePropertiesDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ScenePropertiesDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ScenePropertiesDlg,
      qt_meta_data_ScenePropertiesDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ScenePropertiesDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ScenePropertiesDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ScenePropertiesDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ScenePropertiesDlg))
        return static_cast<void*>(const_cast< ScenePropertiesDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int ScenePropertiesDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

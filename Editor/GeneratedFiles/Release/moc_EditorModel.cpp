/****************************************************************************
** Meta object code from reading C++ file 'EditorModel.h'
**
** Created: Thu Aug 6 11:16:39 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../EditorModel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditorModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EditorModelPropertyPage[] = {

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
      25,   24,   24,   24, 0x08,
      50,   24,   24,   24, 0x08,
      74,   24,   24,   24, 0x08,
     104,   24,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EditorModelPropertyPage[] = {
    "EditorModelPropertyPage\0\0"
    "onInspectComponentTool()\0"
    "onModelAnimationPanel()\0"
    "onModelAnimationPanelClosed()\0"
    "onToolChanged()\0"
};

void EditorModelPropertyPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditorModelPropertyPage *_t = static_cast<EditorModelPropertyPage *>(_o);
        switch (_id) {
        case 0: _t->onInspectComponentTool(); break;
        case 1: _t->onModelAnimationPanel(); break;
        case 2: _t->onModelAnimationPanelClosed(); break;
        case 3: _t->onToolChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData EditorModelPropertyPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditorModelPropertyPage::staticMetaObject = {
    { &PropertyPage::staticMetaObject, qt_meta_stringdata_EditorModelPropertyPage,
      qt_meta_data_EditorModelPropertyPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditorModelPropertyPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditorModelPropertyPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditorModelPropertyPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditorModelPropertyPage))
        return static_cast<void*>(const_cast< EditorModelPropertyPage*>(this));
    return PropertyPage::qt_metacast(_clname);
}

int EditorModelPropertyPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PropertyPage::qt_metacall(_c, _id, _a);
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

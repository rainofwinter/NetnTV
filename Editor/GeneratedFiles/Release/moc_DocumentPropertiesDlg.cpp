/****************************************************************************
** Meta object code from reading C++ file 'DocumentPropertiesDlg.h'
**
** Created: Thu Aug 6 11:16:33 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../DocumentPropertiesDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DocumentPropertiesDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DocumentPropertiesDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x09,
      30,   22,   22,   22, 0x09,
      44,   22,   22,   22, 0x09,
      61,   22,   22,   22, 0x09,
      79,   22,   22,   22, 0x09,
     100,   22,   22,   22, 0x09,
     122,   22,   22,   22, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_DocumentPropertiesDlg[] = {
    "DocumentPropertiesDlg\0\0onOk()\0"
    "onAddScript()\0onRemoveScript()\0"
    "onAddScriptFile()\0onRemoveScriptFile()\0"
    "onScriptsSelChanged()\0onScriptFilesSelChanged()\0"
};

void DocumentPropertiesDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DocumentPropertiesDlg *_t = static_cast<DocumentPropertiesDlg *>(_o);
        switch (_id) {
        case 0: _t->onOk(); break;
        case 1: _t->onAddScript(); break;
        case 2: _t->onRemoveScript(); break;
        case 3: _t->onAddScriptFile(); break;
        case 4: _t->onRemoveScriptFile(); break;
        case 5: _t->onScriptsSelChanged(); break;
        case 6: _t->onScriptFilesSelChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData DocumentPropertiesDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DocumentPropertiesDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DocumentPropertiesDlg,
      qt_meta_data_DocumentPropertiesDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DocumentPropertiesDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DocumentPropertiesDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DocumentPropertiesDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DocumentPropertiesDlg))
        return static_cast<void*>(const_cast< DocumentPropertiesDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int DocumentPropertiesDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE

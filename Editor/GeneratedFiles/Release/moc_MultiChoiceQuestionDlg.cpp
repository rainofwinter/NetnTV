/****************************************************************************
** Meta object code from reading C++ file 'MultiChoiceQuestionDlg.h'
**
** Created: Thu Aug 6 11:16:34 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../MultiChoiceQuestionDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MultiChoiceQuestionDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MultiChoiceQuestionDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x08,
      32,   23,   23,   23, 0x08,
      43,   23,   23,   23, 0x08,
      50,   23,   23,   23, 0x08,
      65,   23,   23,   23, 0x08,
      85,   23,   23,   23, 0x08,
     103,   23,   23,   23, 0x08,
     122,   23,   23,   23, 0x08,
     144,   23,   23,   23, 0x08,
     162,   23,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MultiChoiceQuestionDlg[] = {
    "MultiChoiceQuestionDlg\0\0onAdd()\0"
    "onDelete()\0onOk()\0onFontBrowse()\0"
    "onUncheckedBrowse()\0onCheckedBrowse()\0"
    "onCheckAnsBrowse()\0onChoicesSelChanged()\0"
    "onCorrectBrowse()\0onIncorrectBrowse()\0"
};

void MultiChoiceQuestionDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MultiChoiceQuestionDlg *_t = static_cast<MultiChoiceQuestionDlg *>(_o);
        switch (_id) {
        case 0: _t->onAdd(); break;
        case 1: _t->onDelete(); break;
        case 2: _t->onOk(); break;
        case 3: _t->onFontBrowse(); break;
        case 4: _t->onUncheckedBrowse(); break;
        case 5: _t->onCheckedBrowse(); break;
        case 6: _t->onCheckAnsBrowse(); break;
        case 7: _t->onChoicesSelChanged(); break;
        case 8: _t->onCorrectBrowse(); break;
        case 9: _t->onIncorrectBrowse(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData MultiChoiceQuestionDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MultiChoiceQuestionDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MultiChoiceQuestionDlg,
      qt_meta_data_MultiChoiceQuestionDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MultiChoiceQuestionDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MultiChoiceQuestionDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MultiChoiceQuestionDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MultiChoiceQuestionDlg))
        return static_cast<void*>(const_cast< MultiChoiceQuestionDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int MultiChoiceQuestionDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

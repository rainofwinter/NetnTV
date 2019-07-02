/****************************************************************************
** Meta object code from reading C++ file 'ModelAnimationPanel.h'
**
** Created: Thu Aug 6 11:16:34 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../ModelAnimationPanel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ModelAnimationPanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ModelAnimationPanel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
      30,   20,   20,   20, 0x08,
      54,   48,   20,   20, 0x08,
      89,   80,   20,   20, 0x08,
     124,   20,   20,   20, 0x08,
     143,   20,   20,   20, 0x08,
     162,   20,   20,   20, 0x08,
     178,   20,   20,   20, 0x08,
     197,   20,   20,   20, 0x08,
     226,   20,   20,   20, 0x08,
     245,   20,   20,   20, 0x08,
     257,   20,   20,   20, 0x08,
     272,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ModelAnimationPanel[] = {
    "ModelAnimationPanel\0\0closed()\0"
    "syncToSelection()\0value\0"
    "onSliderValueChanged(int)\0document\0"
    "onDocumentChanged(EditorDocument*)\0"
    "onRestrictJoints()\0onStartJointEdit()\0"
    "onAddEndJoint()\0onRemoveEndJoint()\0"
    "onEndJointSelectionChanged()\0"
    "onRestrictMeshes()\0onAddMesh()\0"
    "onRemoveMesh()\0onMeshSelectionChanged()\0"
};

void ModelAnimationPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ModelAnimationPanel *_t = static_cast<ModelAnimationPanel *>(_o);
        switch (_id) {
        case 0: _t->closed(); break;
        case 1: _t->syncToSelection(); break;
        case 2: _t->onSliderValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onDocumentChanged((*reinterpret_cast< EditorDocument*(*)>(_a[1]))); break;
        case 4: _t->onRestrictJoints(); break;
        case 5: _t->onStartJointEdit(); break;
        case 6: _t->onAddEndJoint(); break;
        case 7: _t->onRemoveEndJoint(); break;
        case 8: _t->onEndJointSelectionChanged(); break;
        case 9: _t->onRestrictMeshes(); break;
        case 10: _t->onAddMesh(); break;
        case 11: _t->onRemoveMesh(); break;
        case 12: _t->onMeshSelectionChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ModelAnimationPanel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ModelAnimationPanel::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ModelAnimationPanel,
      qt_meta_data_ModelAnimationPanel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ModelAnimationPanel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ModelAnimationPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ModelAnimationPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ModelAnimationPanel))
        return static_cast<void*>(const_cast< ModelAnimationPanel*>(this));
    return QWidget::qt_metacast(_clname);
}

int ModelAnimationPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void ModelAnimationPanel::closed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE

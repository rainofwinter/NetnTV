/****************************************************************************
** Meta object code from reading C++ file 'EditorDocument.h'
**
** Created: Thu Aug 6 11:16:38 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../EditorDocument.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditorDocument.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EditorDocument[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      40,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      30,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      40,   15,   15,   15, 0x05,
      59,   15,   15,   15, 0x05,
      74,   15,   15,   15, 0x05,
      90,   15,   15,   15, 0x05,
     109,   15,   15,   15, 0x05,
     129,   15,   15,   15, 0x05,
     152,   15,   15,   15, 0x05,
     177,   15,   15,   15, 0x05,
     205,   15,   15,   15, 0x05,
     223,   15,   15,   15, 0x05,
     245,   15,   15,   15, 0x05,
     261,   15,   15,   15, 0x05,
     281,   15,   15,   15, 0x05,
     306,   15,   15,   15, 0x05,
     333,   15,   15,   15, 0x05,
     361,   15,   15,   15, 0x05,
     384,   15,   15,   15, 0x05,
     403,   15,   15,   15, 0x05,
     429,   15,   15,   15, 0x05,
     450,   15,   15,   15, 0x05,
     467,   15,   15,   15, 0x05,
     494,   15,   15,   15, 0x05,
     518,   15,   15,   15, 0x05,
     540,   15,   15,   15, 0x05,
     563,   15,   15,   15, 0x05,
     577,   15,   15,   15, 0x05,
     590,   15,   15,   15, 0x05,
     608,   15,   15,   15, 0x05,
     630,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
     681,  659,   15,   15, 0x08,
     749,   15,   15,   15, 0x08,
     769,   15,   15,   15, 0x08,
     794,   15,   15,   15, 0x08,
     845,  817,   15,   15, 0x08,
     878,   15,   15,   15, 0x08,
     902,   15,   15,   15, 0x08,
     924,   15,   15,   15, 0x08,
     946,   15,   15,   15, 0x08,
     971,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EditorDocument[] = {
    "EditorDocument\0\0sceneSelectionChanged()\0"
    "sceneListChanged()\0sceneChanged()\0"
    "objectChanged()\0appObjectChanged()\0"
    "objectListChanged()\0appObjectListChanged()\0"
    "objectSelectionChanged()\0"
    "appObjectSelectionChanged()\0"
    "listenerChanged()\0listenerListChanged()\0"
    "actionChanged()\0actionListChanged()\0"
    "actionSelectionChanged()\0"
    "listenerSelectionChanged()\0"
    "animationSelectionChanged()\0"
    "animationListChanged()\0animationChanged()\0"
    "channelSelectionChanged()\0"
    "channelListChanged()\0channelChanged()\0"
    "keyFrameSelectionChanged()\0"
    "commandHistoryChanged()\0timeLineTimeChanged()\0"
    "timeLineTimeChanging()\0toolChanged()\0"
    "sceneDrawn()\0fpsGuideChanged()\0"
    "resourceListChanged()\0"
    "channelTemplateListChanged()\0"
    "oldKeyFrames,newTimes\0"
    "onKeyFrameTimeChanged(std::vector<KeyFrameData>,std::vector<float>)\0"
    "onKeyFrameDeleted()\0onAnimationListChanged()\0"
    "onChannelListChanged()\0"
    "oldActionAddr,newActionAddr\0"
    "onActionChanged(Action*,Action*)\0"
    "onListenerListChanged()\0onActionListChanged()\0"
    "onObjectListChanged()\0onAppObjectListChanged()\0"
    "onSceneListChanged()\0"
};

void EditorDocument::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditorDocument *_t = static_cast<EditorDocument *>(_o);
        switch (_id) {
        case 0: _t->sceneSelectionChanged(); break;
        case 1: _t->sceneListChanged(); break;
        case 2: _t->sceneChanged(); break;
        case 3: _t->objectChanged(); break;
        case 4: _t->appObjectChanged(); break;
        case 5: _t->objectListChanged(); break;
        case 6: _t->appObjectListChanged(); break;
        case 7: _t->objectSelectionChanged(); break;
        case 8: _t->appObjectSelectionChanged(); break;
        case 9: _t->listenerChanged(); break;
        case 10: _t->listenerListChanged(); break;
        case 11: _t->actionChanged(); break;
        case 12: _t->actionListChanged(); break;
        case 13: _t->actionSelectionChanged(); break;
        case 14: _t->listenerSelectionChanged(); break;
        case 15: _t->animationSelectionChanged(); break;
        case 16: _t->animationListChanged(); break;
        case 17: _t->animationChanged(); break;
        case 18: _t->channelSelectionChanged(); break;
        case 19: _t->channelListChanged(); break;
        case 20: _t->channelChanged(); break;
        case 21: _t->keyFrameSelectionChanged(); break;
        case 22: _t->commandHistoryChanged(); break;
        case 23: _t->timeLineTimeChanged(); break;
        case 24: _t->timeLineTimeChanging(); break;
        case 25: _t->toolChanged(); break;
        case 26: _t->sceneDrawn(); break;
        case 27: _t->fpsGuideChanged(); break;
        case 28: _t->resourceListChanged(); break;
        case 29: _t->channelTemplateListChanged(); break;
        case 30: _t->onKeyFrameTimeChanged((*reinterpret_cast< const std::vector<KeyFrameData>(*)>(_a[1])),(*reinterpret_cast< const std::vector<float>(*)>(_a[2]))); break;
        case 31: _t->onKeyFrameDeleted(); break;
        case 32: _t->onAnimationListChanged(); break;
        case 33: _t->onChannelListChanged(); break;
        case 34: _t->onActionChanged((*reinterpret_cast< Action*(*)>(_a[1])),(*reinterpret_cast< Action*(*)>(_a[2]))); break;
        case 35: _t->onListenerListChanged(); break;
        case 36: _t->onActionListChanged(); break;
        case 37: _t->onObjectListChanged(); break;
        case 38: _t->onAppObjectListChanged(); break;
        case 39: _t->onSceneListChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EditorDocument::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditorDocument::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_EditorDocument,
      qt_meta_data_EditorDocument, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditorDocument::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditorDocument::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditorDocument::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditorDocument))
        return static_cast<void*>(const_cast< EditorDocument*>(this));
    return QObject::qt_metacast(_clname);
}

int EditorDocument::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 40)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 40;
    }
    return _id;
}

// SIGNAL 0
void EditorDocument::sceneSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void EditorDocument::sceneListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void EditorDocument::sceneChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void EditorDocument::objectChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void EditorDocument::appObjectChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void EditorDocument::objectListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void EditorDocument::appObjectListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void EditorDocument::objectSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void EditorDocument::appObjectSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void EditorDocument::listenerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void EditorDocument::listenerListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void EditorDocument::actionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, 0);
}

// SIGNAL 12
void EditorDocument::actionListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}

// SIGNAL 13
void EditorDocument::actionSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, 0);
}

// SIGNAL 14
void EditorDocument::listenerSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 14, 0);
}

// SIGNAL 15
void EditorDocument::animationSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}

// SIGNAL 16
void EditorDocument::animationListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, 0);
}

// SIGNAL 17
void EditorDocument::animationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 17, 0);
}

// SIGNAL 18
void EditorDocument::channelSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 18, 0);
}

// SIGNAL 19
void EditorDocument::channelListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 19, 0);
}

// SIGNAL 20
void EditorDocument::channelChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 20, 0);
}

// SIGNAL 21
void EditorDocument::keyFrameSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 21, 0);
}

// SIGNAL 22
void EditorDocument::commandHistoryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 22, 0);
}

// SIGNAL 23
void EditorDocument::timeLineTimeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 23, 0);
}

// SIGNAL 24
void EditorDocument::timeLineTimeChanging()
{
    QMetaObject::activate(this, &staticMetaObject, 24, 0);
}

// SIGNAL 25
void EditorDocument::toolChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 25, 0);
}

// SIGNAL 26
void EditorDocument::sceneDrawn()
{
    QMetaObject::activate(this, &staticMetaObject, 26, 0);
}

// SIGNAL 27
void EditorDocument::fpsGuideChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 27, 0);
}

// SIGNAL 28
void EditorDocument::resourceListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 28, 0);
}

// SIGNAL 29
void EditorDocument::channelTemplateListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 29, 0);
}
QT_END_MOC_NAMESPACE

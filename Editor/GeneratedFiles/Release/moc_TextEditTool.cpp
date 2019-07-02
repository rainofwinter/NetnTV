/****************************************************************************
** Meta object code from reading C++ file 'TextEditTool.h'
**
** Created: Thu Aug 6 11:16:31 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../TextEditTool.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TextEditTool.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TextEditWindow[] = {

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
      16,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      25,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TextEditWindow[] = {
    "TextEditWindow\0\0closed()\0onDone()\0"
};

void TextEditWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TextEditWindow *_t = static_cast<TextEditWindow *>(_o);
        switch (_id) {
        case 0: _t->closed(); break;
        case 1: _t->onDone(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData TextEditWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TextEditWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TextEditWindow,
      qt_meta_data_TextEditWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TextEditWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TextEditWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TextEditWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextEditWindow))
        return static_cast<void*>(const_cast< TextEditWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int TextEditWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void TextEditWindow::closed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_TextEditControl[] = {

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

static const char qt_meta_stringdata_TextEditControl[] = {
    "TextEditControl\0\0imeTextChanged()\0"
};

void TextEditControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TextEditControl *_t = static_cast<TextEditControl *>(_o);
        switch (_id) {
        case 0: _t->imeTextChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData TextEditControl::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TextEditControl::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_TextEditControl,
      qt_meta_data_TextEditControl, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TextEditControl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TextEditControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TextEditControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextEditControl))
        return static_cast<void*>(const_cast< TextEditControl*>(this));
    return QTextEdit::qt_metacast(_clname);
}

int TextEditControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void TextEditControl::imeTextChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_TextEditTool[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      33,   13,   13,   13, 0x08,
      50,   13,   13,   13, 0x08,
      71,   13,   13,   13, 0x08,
      94,   13,   13,   13, 0x08,
     120,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TextEditTool[] = {
    "TextEditTool\0\0cursorPosChanged()\0"
    "onWindowClosed()\0onLostObjSelection()\0"
    "textChangedInControl()\0imeTextChangedInControl()\0"
    "cursorChangedInControl()\0"
};

void TextEditTool::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TextEditTool *_t = static_cast<TextEditTool *>(_o);
        switch (_id) {
        case 0: _t->cursorPosChanged(); break;
        case 1: _t->onWindowClosed(); break;
        case 2: _t->onLostObjSelection(); break;
        case 3: _t->textChangedInControl(); break;
        case 4: _t->imeTextChangedInControl(); break;
        case 5: _t->cursorChangedInControl(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData TextEditTool::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TextEditTool::staticMetaObject = {
    { &Tool::staticMetaObject, qt_meta_stringdata_TextEditTool,
      qt_meta_data_TextEditTool, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TextEditTool::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TextEditTool::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TextEditTool::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextEditTool))
        return static_cast<void*>(const_cast< TextEditTool*>(this));
    return Tool::qt_metacast(_clname);
}

int TextEditTool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Tool::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void TextEditTool::cursorPosChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE

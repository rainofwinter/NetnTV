/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created: Thu Aug 6 11:16:30 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../MainWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MdiArea[] = {

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

static const char qt_meta_stringdata_MdiArea[] = {
    "MdiArea\0"
};

void MdiArea::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData MdiArea::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MdiArea::staticMetaObject = {
    { &QMdiArea::staticMetaObject, qt_meta_stringdata_MdiArea,
      qt_meta_data_MdiArea, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MdiArea::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MdiArea::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MdiArea::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MdiArea))
        return static_cast<void*>(const_cast< MdiArea*>(this));
    return QMdiArea::qt_metacast(_clname);
}

int MdiArea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMdiArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
     101,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      40,   11,   11,   11, 0x05,
      70,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     108,   11,  103,   11, 0x0a,
     120,   11,   11,   11, 0x0a,
     136,   11,   11,   11, 0x09,
     146,   11,   11,   11, 0x09,
     164,   11,   11,   11, 0x09,
     181,   11,   11,   11, 0x09,
     192,   11,   11,   11, 0x09,
     203,   11,   11,   11, 0x09,
     216,   11,   11,   11, 0x09,
     228,   11,   11,   11, 0x09,
     239,   11,   11,   11, 0x09,
     254,   11,   11,   11, 0x09,
     266,   11,   11,   11, 0x09,
     284,   11,   11,   11, 0x09,
     300,   11,   11,   11, 0x09,
     311,   11,   11,   11, 0x09,
     324,   11,   11,   11, 0x09,
     335,   11,   11,   11, 0x09,
     347,   11,   11,   11, 0x09,
     363,   11,   11,   11, 0x09,
     380,   11,   11,   11, 0x09,
     395,   11,   11,   11, 0x09,
     410,   11,   11,   11, 0x09,
     423,   11,   11,   11, 0x09,
     436,   11,   11,   11, 0x09,
     449,   11,   11,   11, 0x09,
     462,   11,   11,   11, 0x09,
     473,   11,   11,   11, 0x09,
     488,   11,   11,   11, 0x09,
     504,   11,   11,   11, 0x09,
     520,   11,   11,   11, 0x09,
     539,   11,   11,   11, 0x09,
     562,   11,   11,   11, 0x09,
     571,   11,   11,   11, 0x09,
     580,   11,   11,   11, 0x09,
     592,   11,   11,   11, 0x09,
     630,   11,   11,   11, 0x09,
     641,   11,   11,   11, 0x09,
     655,   11,   11,   11, 0x09,
     666,   11,   11,   11, 0x09,
     676,   11,   11,   11, 0x09,
     686,   11,   11,   11, 0x09,
     709,   11,   11,   11, 0x09,
     727,   11,   11,   11, 0x09,
     747,   11,   11,   11, 0x09,
     761,   11,   11,   11, 0x09,
     778,   11,   11,   11, 0x09,
     792,   11,   11,   11, 0x09,
     806,   11,   11,   11, 0x09,
     820,   11,   11,   11, 0x09,
     831,   11,   11,   11, 0x09,
     840,   11,   11,   11, 0x09,
     850,   11,   11,   11, 0x09,
     867,   11,   11,   11, 0x09,
     885,   11,   11,   11, 0x09,
     901,   11,   11,   11, 0x09,
     921,   11,   11,   11, 0x09,
     941,   11,   11,   11, 0x09,
     954,   11,   11,   11, 0x09,
     967,   11,   11,   11, 0x09,
     980,   11,   11,   11, 0x09,
     993,   11,   11,   11, 0x09,
    1008,   11,   11,   11, 0x09,
    1024,   11,   11,   11, 0x09,
    1039,   11,   11,   11, 0x09,
    1055,   11,   11,   11, 0x09,
    1073,   11,   11,   11, 0x09,
    1092,   11,   11,   11, 0x09,
    1110,   11,   11,   11, 0x09,
    1129,   11,   11,   11, 0x09,
    1145,   11,   11,   11, 0x09,
    1168,   11,   11,   11, 0x09,
    1194,   11,   11,   11, 0x09,
    1215,   11,   11,   11, 0x09,
    1225,   11,   11,   11, 0x09,
    1263,   11,   11,   11, 0x09,
    1290,   11,   11,   11, 0x09,
    1326, 1320,   11,   11, 0x09,
    1345,   11,   11,   11, 0x09,
    1371,   11,   11,   11, 0x09,
    1382,   11,   11,   11, 0x09,
    1418, 1409,   11,   11, 0x09,
    1437, 1409,   11,   11, 0x09,
    1456,   11,   11,   11, 0x09,
    1466,   11,   11,   11, 0x09,
    1477,   11,   11,   11, 0x09,
    1492,   11,   11,   11, 0x09,
    1511,   11,   11,   11, 0x09,
    1533,   11,   11,   11, 0x09,
    1549,   11,   11,   11, 0x09,
    1562,   11,   11,   11, 0x09,
    1583,   11,   11,   11, 0x09,
    1610,   11,   11,   11, 0x09,
    1634,   11,   11,   11, 0x09,
    1651,   11,   11,   11, 0x09,
    1666,   11,   11,   11, 0x09,
    1683,   11,   11,   11, 0x09,
    1700,   11,   11,   11, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0documentActivated(QWidget*)\0"
    "documentDeactivated(QWidget*)\0"
    "documentChanged(EditorDocument*)\0bool\0"
    "closeFile()\0onToolChanged()\0newFile()\0"
    "newFileWithAnim()\0openRecentFile()\0"
    "openFile()\0saveFile()\0saveAsFile()\0"
    "onPublish()\0textFont()\0textFontSize()\0"
    "textColor()\0onColorSelected()\0"
    "textUnderline()\0textBold()\0textCancle()\0"
    "textLeft()\0textRight()\0textAlignLeft()\0"
    "textAlignRight()\0textAlignMid()\0"
    "textAlignMix()\0toolTransX()\0toolTransY()\0"
    "toolScaleX()\0toolScaleY()\0toolRotZ()\0"
    "onNewLinkUrl()\0onNewLinkPage()\0"
    "onSlideEffect()\0onPageFlipEffect()\0"
    "onPageFlipOrigEffect()\0onUndo()\0"
    "onRedo()\0onPreview()\0"
    "onPlayerError(QProcess::ProcessError)\0"
    "onSelect()\0onTranslate()\0onRotate()\0"
    "onScale()\0onPivot()\0onAppObjectTransform()\0"
    "onFreeTransform()\0onVideoRegionTool()\0"
    "onImportPDF()\0onImportFolder()\0"
    "onImportGif()\0onImportXML()\0onExportXML()\0"
    "onDelete()\0onCopy()\0onPaste()\0"
    "onViewProperty()\0onViewAnimation()\0"
    "onViewAniList()\0onViewInteraction()\0"
    "onViewResetPanels()\0onAnimSet1()\0"
    "onAnimSet2()\0onAnimSet3()\0onAnimSet4()\0"
    "onNudgePlusX()\0onNudgeMinusX()\0"
    "onNudgePlusY()\0onNudgeMinusY()\0"
    "onNudgeTenPlusX()\0onNudgeTenMinusX()\0"
    "onNudgeTenPlusY()\0onNudgeTenMinusY()\0"
    "onPreferences()\0onDocumentProperties()\0"
    "onDocTemplateProperties()\0"
    "onPageTrackingUrls()\0onAbout()\0"
    "onSubWindowActivating(QMdiSubWindow*)\0"
    "onObjectSelectionChanged()\0"
    "onAppObjectSelectionChanged()\0index\0"
    "onModeChanged(int)\0onCommandHistoryChanged()\0"
    "onSetKey()\0subWindowClosing(QWidget*)\0"
    "fileName\0importPdf(QString)\0"
    "importGif(QString)\0onLogin()\0onLogout()\0"
    "onMenuLogout()\0onChangePassword()\0"
    "onImportPDFtemprary()\0onPanningTool()\0"
    "onZoomTool()\0onCanvasRotateTool()\0"
    "onResetCameraFitToScreen()\0"
    "onResetCameraOneToOne()\0onBringToFront()\0"
    "onSendToBack()\0onBringForward()\0"
    "onSendBackward()\0onExportHtml()\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->documentActivated((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 1: _t->documentDeactivated((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 2: _t->documentChanged((*reinterpret_cast< EditorDocument*(*)>(_a[1]))); break;
        case 3: { bool _r = _t->closeFile();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: _t->onToolChanged(); break;
        case 5: _t->newFile(); break;
        case 6: _t->newFileWithAnim(); break;
        case 7: _t->openRecentFile(); break;
        case 8: _t->openFile(); break;
        case 9: _t->saveFile(); break;
        case 10: _t->saveAsFile(); break;
        case 11: _t->onPublish(); break;
        case 12: _t->textFont(); break;
        case 13: _t->textFontSize(); break;
        case 14: _t->textColor(); break;
        case 15: _t->onColorSelected(); break;
        case 16: _t->textUnderline(); break;
        case 17: _t->textBold(); break;
        case 18: _t->textCancle(); break;
        case 19: _t->textLeft(); break;
        case 20: _t->textRight(); break;
        case 21: _t->textAlignLeft(); break;
        case 22: _t->textAlignRight(); break;
        case 23: _t->textAlignMid(); break;
        case 24: _t->textAlignMix(); break;
        case 25: _t->toolTransX(); break;
        case 26: _t->toolTransY(); break;
        case 27: _t->toolScaleX(); break;
        case 28: _t->toolScaleY(); break;
        case 29: _t->toolRotZ(); break;
        case 30: _t->onNewLinkUrl(); break;
        case 31: _t->onNewLinkPage(); break;
        case 32: _t->onSlideEffect(); break;
        case 33: _t->onPageFlipEffect(); break;
        case 34: _t->onPageFlipOrigEffect(); break;
        case 35: _t->onUndo(); break;
        case 36: _t->onRedo(); break;
        case 37: _t->onPreview(); break;
        case 38: _t->onPlayerError((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        case 39: _t->onSelect(); break;
        case 40: _t->onTranslate(); break;
        case 41: _t->onRotate(); break;
        case 42: _t->onScale(); break;
        case 43: _t->onPivot(); break;
        case 44: _t->onAppObjectTransform(); break;
        case 45: _t->onFreeTransform(); break;
        case 46: _t->onVideoRegionTool(); break;
        case 47: _t->onImportPDF(); break;
        case 48: _t->onImportFolder(); break;
        case 49: _t->onImportGif(); break;
        case 50: _t->onImportXML(); break;
        case 51: _t->onExportXML(); break;
        case 52: _t->onDelete(); break;
        case 53: _t->onCopy(); break;
        case 54: _t->onPaste(); break;
        case 55: _t->onViewProperty(); break;
        case 56: _t->onViewAnimation(); break;
        case 57: _t->onViewAniList(); break;
        case 58: _t->onViewInteraction(); break;
        case 59: _t->onViewResetPanels(); break;
        case 60: _t->onAnimSet1(); break;
        case 61: _t->onAnimSet2(); break;
        case 62: _t->onAnimSet3(); break;
        case 63: _t->onAnimSet4(); break;
        case 64: _t->onNudgePlusX(); break;
        case 65: _t->onNudgeMinusX(); break;
        case 66: _t->onNudgePlusY(); break;
        case 67: _t->onNudgeMinusY(); break;
        case 68: _t->onNudgeTenPlusX(); break;
        case 69: _t->onNudgeTenMinusX(); break;
        case 70: _t->onNudgeTenPlusY(); break;
        case 71: _t->onNudgeTenMinusY(); break;
        case 72: _t->onPreferences(); break;
        case 73: _t->onDocumentProperties(); break;
        case 74: _t->onDocTemplateProperties(); break;
        case 75: _t->onPageTrackingUrls(); break;
        case 76: _t->onAbout(); break;
        case 77: _t->onSubWindowActivating((*reinterpret_cast< QMdiSubWindow*(*)>(_a[1]))); break;
        case 78: _t->onObjectSelectionChanged(); break;
        case 79: _t->onAppObjectSelectionChanged(); break;
        case 80: _t->onModeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 81: _t->onCommandHistoryChanged(); break;
        case 82: _t->onSetKey(); break;
        case 83: _t->subWindowClosing((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 84: _t->importPdf((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 85: _t->importGif((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 86: _t->onLogin(); break;
        case 87: _t->onLogout(); break;
        case 88: _t->onMenuLogout(); break;
        case 89: _t->onChangePassword(); break;
        case 90: _t->onImportPDFtemprary(); break;
        case 91: _t->onPanningTool(); break;
        case 92: _t->onZoomTool(); break;
        case 93: _t->onCanvasRotateTool(); break;
        case 94: _t->onResetCameraFitToScreen(); break;
        case 95: _t->onResetCameraOneToOne(); break;
        case 96: _t->onBringToFront(); break;
        case 97: _t->onSendToBack(); break;
        case 98: _t->onBringForward(); break;
        case 99: _t->onSendBackward(); break;
        case 100: _t->onExportHtml(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 101)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 101;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::documentActivated(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainWindow::documentDeactivated(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MainWindow::documentChanged(EditorDocument * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE

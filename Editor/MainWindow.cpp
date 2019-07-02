#include "stdafx.h"
#include "MainWindow.h"
#include "EditorDocument.h"
#include "EditorScene.h"
#include "GLWidget.h"
#include "PropertyPane.h"
#include "AnimationPane.h"
#include "SceneObjectListPane.h"
#include "AppObjectListPane.h"
#include "EventActionPane.h"
#include "SceneListPane.h"
#include "Utils.h"
#include "FileUtils.h"
#include "SceneObject.h"
#include "EditorGlobal.h"
#include "DocumentTemplate.h"
#include "NewProjectDlg.h"
#include "AboutDlg.h"
#include "Command.h"
#include "Image.h"
#include "ImageAnimation.h"
#include "Root.h"
#include "PreferencesDlg.h"
#include "PageTrackingUrlsDlg.h"
#include "DocumentPropertiesDlg.h"
#include "EditorDocumentTemplate.h"
#include "PublishDlg.h"
#include "PublishingDlg.h"
#include "Animation.h"
#include "LoginDlg.h"
#include "ChangePasswordDlg.h"

#include "PdfToTextTemp.h"

#include "Xml.h"

//#include "Magick++.h"

using namespace std;

MdiArea::MdiArea(MainWindow * parent) : QMdiArea(parent)
{
	parent_ = parent;
	setViewMode(QMdiArea::TabbedView);

	QList<QTabBar*> tabBarList = findChildren<QTabBar*>();
	tabBar_ = tabBarList.at(0);
	tabBar_->setContextMenuPolicy(Qt::NoContextMenu);
	if(tabBar_)
	{
		tabBar_->setTabsClosable(true);
		tabBar_->setExpanding(true);
	}
}

///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags)
{
	document_ = 0;
	ui.setupUi(this);

	QFile File(":/data/StyleSheet/MdiArea.qss");
	File.open(QFile::ReadOnly);
	QString StyleSheet = QLatin1String(File.readAll());
	
	mdiArea_ = new MdiArea(this);
	mdiArea_->setObjectName(QString::fromUtf8("mdiArea"));
	mdiArea_->setFrameShape(QFrame::NoFrame);
	mdiArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	mdiArea_->setViewMode(QMdiArea::TabbedView);
	mdiArea_->setTabShape(QTabWidget::Rounded);
	mdiArea_->setTabPosition(QTabWidget::North);
	mdiArea_->setStyleSheet(StyleSheet);
	mdiArea_->setTabsClosable(true);
	mdiArea_->setTabsMovable(true);
	mdiArea_->setAcceptDrops(true);
	setCentralWidget(mdiArea_);
	delete ui.mdiArea;

	curMdiSubWindow_ = 0;
	objectMode_ = ModeSceneObject;

	statusMessage_ = new QLabel("");
	mousePosition_ = new QLabel("");
	
	//toolbar combo box
	ignoreModeChanged_ = false;
	modeSelectorComboBox_ = new QComboBox();
	modeSelectorComboBox_->setMinimumHeight(20);
	modeSelectorComboBox_->addItem(tr("Scene Object"));
	modeSelectorComboBox_->addItem(tr("App Object"));
	
	QWidget * comboBoxWidget = new QWidget;
	QHBoxLayout * layout = new QHBoxLayout;
	layout->setMargin(0);
	layout->setSpacing(0);
	comboBoxWidget->setLayout(layout);
//	layout->addSpacing(6);
//	layout->addWidget(modeSelectorComboBox_);
//	layout->addSpacing(6);

	ui.mainToolBar->insertWidget(ui.actionSelect, comboBoxWidget);
//	ui.mainToolBar->insertSeparator(ui.actionSelect);
//	connect(modeSelectorComboBox_, SIGNAL(currentIndexChanged(int)), 
//		this, SLOT(onModeChanged(int)));
	this->setDockNestingEnabled(true);

	ui.mainToolBar->setIconSize(QSize(30, 30));

	propertyPane_ = new PropertyPane;
	ui.propertiesDockWidget->setWidget(propertyPane_);

	animationPane_ = new AnimationPane(this);
	ui.animationDockWidget->setWidget(animationPane_);
	
	sceneListPane_ = new SceneListPane(this);
	ui.sceneListDockWidget->setWidget(sceneListPane_);

	sceneObjectListPane_ = new SceneObjectListPane(this);
	ui.objectListDockWidget->setWidget(sceneObjectListPane_);

	appObjectListPane_ = new AppObjectListPane(this);

	eventActionPane_ = new EventActionPane(this);
	ui.eventActionDockWidget->setWidget(eventActionPane_);

	tabifyDockWidget(ui.propertiesDockWidget, ui.eventActionDockWidget);
	ui.propertiesDockWidget->raise();
	ui.objectListDockWidget->raise();
	
	connect(mdiArea_, SIGNAL(subWindowActivated(QMdiSubWindow*)), 
		this, SLOT(onSubWindowActivating(QMdiSubWindow *)));

	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(newFile()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(ui.actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAsFile()));
	connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(closeFile()));
	connect(ui.actionPublish, SIGNAL(triggered()), this, SLOT(onPublish()));
		
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(ui.actionUndo, SIGNAL(triggered()), this, SLOT(onUndo()));
	connect(ui.actionRedo, SIGNAL(triggered()), this, SLOT(onRedo()));
	connect(ui.actionPreview, SIGNAL(triggered()), this, SLOT(onPreview()));
	connect(ui.actionSelect, SIGNAL(triggered()), this, SLOT(onSelect()));
	connect(ui.actionTranslate, SIGNAL(triggered()), this, SLOT(onTranslate()));
	connect(ui.actionRotate, SIGNAL(triggered()), this, SLOT(onRotate()));
	connect(ui.actionScale, SIGNAL(triggered()), this, SLOT(onScale()));
	connect(ui.actionPivot, SIGNAL(triggered()), this, SLOT(onPivot()));
	connect(ui.actionTransformAppObject, SIGNAL(triggered()), this, SLOT(onAppObjectTransform()));
	connect(ui.action_ImportPDF, SIGNAL(triggered()), this, SLOT(onImportPDF()));
	connect(ui.actionImportFolder, SIGNAL(triggered()), this, SLOT(onImportFolder()));
	connect(ui.actionImportGif, SIGNAL(triggered()), this, SLOT(onImportGif()));
	connect(ui.actionImportXML, SIGNAL(triggered()), this, SLOT(onImportXML()));
	connect(ui.actionExportXML, SIGNAL(triggered()), this, SLOT(onExportXML()));

	connect(ui.actionImportPDFtemp, SIGNAL(triggered()), this, SLOT(onImportPDFtemprary()));

	connect(ui.actionDelete, SIGNAL(triggered()), this, SLOT(onDelete()));
	connect(ui.actionCopy, SIGNAL(triggered()), this, SLOT(onCopy()));
	connect(ui.actionPaste, SIGNAL(triggered()), this, SLOT(onPaste()));

	connect(ui.actionNudgePlusX, SIGNAL(triggered()), this, SLOT(onNudgePlusX()));
	connect(ui.actionNudgeMinusX, SIGNAL(triggered()), this, SLOT(onNudgeMinusX()));
	connect(ui.actionNudgePlusY, SIGNAL(triggered()), this, SLOT(onNudgePlusY()));
	connect(ui.actionNudgeMinusY, SIGNAL(triggered()), this, SLOT(onNudgeMinusY()));
	connect(ui.actionNudge_10X, SIGNAL(triggered()), this, SLOT(onNudgeTenMinusX()));
	connect(ui.actionNudge_10X_2, SIGNAL(triggered()), this, SLOT(onNudgeTenPlusX()));
	connect(ui.actionNudge_10Y, SIGNAL(triggered()), this, SLOT(onNudgeTenPlusY()));
	connect(ui.actionNudge_10Y_2, SIGNAL(triggered()), this, SLOT(onNudgeTenMinusY()));
	connect(ui.actionPreferences, SIGNAL(triggered()), this, SLOT(onPreferences()));
	connect(ui.actionDocumentProperties, SIGNAL(triggered()), this, SLOT(onDocumentProperties()));
	connect(ui.actionDocumentTemplateProperties, SIGNAL(triggered()), this, SLOT(onDocTemplateProperties()));
	connect(ui.actionPageTrackingUrls, SIGNAL(triggered()), this, SLOT(onPageTrackingUrls()));
	connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));
	connect(ui.actionChange_Account_Password, SIGNAL(triggered()), this, SLOT(onChangePassword()));
	connect(ui.actionLogout, SIGNAL(triggered()), this, SLOT(onMenuLogout()));
	//connect(ui.actionDeactivate, SIGNAL(triggered()), this, SLOT(onDeactivate()));
	//connect(ui.actionInsertAd, SIGNAL(triggered()), this, SLOT(onInsertAd()));
	
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

	ui.menu_View->addAction(ui.sceneListDockWidget->toggleViewAction());
	ui.menu_View->addAction(ui.objectListDockWidget->toggleViewAction());
	ui.menu_View->addAction(ui.animationDockWidget->toggleViewAction());
	ui.menu_View->addAction(ui.propertiesDockWidget->toggleViewAction());
	ui.menu_View->addAction(ui.eventActionDockWidget->toggleViewAction());

	ui.animationDockWidget->setWindowFlags(Qt::FramelessWindowHint);

	for (int i = 0; i < sMaxRecentFiles; ++i)
	{
		recentFileActions_[i] = new QAction(this);
		recentFileActions_[i]->setVisible(false);
		connect(recentFileActions_[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}	

	for (int i = 0; i < sMaxRecentFiles; ++i)
		ui.menuFile->insertAction(ui.actionExit, recentFileActions_[i]);

	ui.menuFile->insertSeparator(ui.actionExit);

//	statusBar_ = this->statusBar();
//	statusBar_->addWidget(statusMessage_	, 1);
//	statusBar_->addWidget(mousePosition_, 2);
	this->statusBar()->setVisible(FALSE);
	
	setContextMenuPolicy(Qt::NoContextMenu);
	setAcceptDrops(true);
	readSettings();
	updateRecentFilesActions();
	updateActions();

	QPalette p(palette());
	p.setColor(QPalette::Window, QColor(0xf0, 0xf0, 0xf0));
	p.setColor(QPalette::Disabled, QPalette::Base, QColor(0xf0, 0xf0, 0xf0));
	p.setColor(QPalette::Active, QPalette::Base, QColor(0xff, 0xff, 0xff));
	setPalette(p);

	setDocument(0);

	timer_.start(10, this);

	isCtrlKey_ = false;
	
	isMenuLogout_ = false;
	isLogin_ = false;
	isLogout_ = false;
	isClosingLogout_ = false;
	loginDlg_ = new LoginDlg(this);
	
	connect(loginDlg_, SIGNAL(dialogClosed()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
	timer_.stop();

	mdiArea_->closeAllSubWindows();

	for(int i = 0; i < sMaxRecentFiles; ++i)
		delete recentFileActions_[i];

	ui.objectListDockWidget->setWidget(0);

	delete propertyPane_;
	delete animationPane_;
	delete sceneObjectListPane_;
	delete appObjectListPane_;
	delete eventActionPane_;
	delete sceneListPane_;
	
	delete modeSelectorComboBox_;
	
	delete statusMessage_;
	delete mousePosition_;
	delete loginDlg_;
}

//REMOVE
#include "ModelFile.h"

ostream & operator << (ostream & out, const Vector3 & vec3)
{
	out << "(" << vec3.x << ", " << vec3.y << ", " << vec3.z << ")";
	return out;
}

//REMOVE
/*
#include "SpatialPartitioning.h"
#include "BoundingBoxObject.h"
KDTree * kdTree = NULL;
*/
void MainWindow::timerEvent(QTimerEvent * event)
{	
	QApplication::processEvents();
	GLWidget * glWidget = curGlWidget();
	
	if (glWidget && document_ && this->windowState() != Qt::WindowMinimized)
	{	
		//printf("update\n");
		glWidget->update();			
		Document * sceneDoc = document_->document();
		sceneDoc->documentTemplate()->asyncLoadUpdate();
		sceneListPane_->asyncLoadUpdate();		
	}

	if (!isLogin_ && !isClosingLogout_ && !loginDlg_->isDialogShow_ && !isSplashShow_) onLogin();
	if (isClosingLogout_ && isLogout_) close();
}


void MainWindow::readSettings()
{	
	recentFiles_ = EditorGlobal::instance().settings()->
		value("recentFiles").toStringList();

	QSettings * settings = EditorGlobal::instance().settings();
	QRect lastGeo = settings->value("windowGeometry").toRect();
	bool isMaximized = settings->value("maximized").toBool();
	
	if(!lastGeo.isEmpty()) setGeometry(lastGeo);
	if(isMaximized)
	{
		/*
		QDesktopWidget desk;
		int screenNo = desk.screenNumber(this);
		QRect availGeo = desk.availableGeometry(screenNo);
		lastNormalGeo_ = QRect(availGeo.x() + 0.125*availGeo.width(), availGeo.y() + 0.125*availGeo.height(),
			0.75*availGeo.width(), 0.75*availGeo.height());
		*/
		showMaximized();
	}
	else
		lastNormalGeo_ = geometry();

	/*
	QByteArray byteArray = settings->value("windowLayout").toByteArray();

	if(byteArray.size () > 0)
	{
		string layoutFile = qStringToStdString(QDir::tempPath());
		layoutFile += "/__layout__.dat";

		FILE * file = fopen(layoutFile.c_str(), "wb");

		fwrite(byteArray.data(), 1, byteArray.size(), file);
		fclose(file);
	}
	int val = settings->value("objectMode").toInt();
	setObjectMode((ObjectMode)val);
	*/
}

void MainWindow::onToolChanged()
{
	updateActions();
}

void MainWindow::onObjectSelectionChanged()
{
	updateActions();
}

void MainWindow::onAppObjectSelectionChanged()
{
	updateActions();
}

void MainWindow::onModeChanged(int index)
{
	if (ignoreModeChanged_) return;
	setObjectMode((ObjectMode)index);
}

void MainWindow::onCommandHistoryChanged()
{
	updateActions();
}

GLWidget * MainWindow::curGlWidget() const
{	
	if (!curMdiSubWindow_) return 0;
	return (GLWidget *)curMdiSubWindow_->widget();
}

void MainWindow::onPlayerError(QProcess::ProcessError err)
{
	if(err == QProcess::FailedToStart)
		QMessageBox::information(0, tr("Error"), tr("Error launching player"));

	if(err == QProcess::Crashed)
		QMessageBox::information(0, tr("Error"), tr("Error player crashed"));
}

MainWindow::ObjectMode MainWindow::toolObjectMode() const
{
	GLWidget * glWidget = curGlWidget();
	ObjectMode toolObjectMode = ModeSceneObject;
	
	if (glWidget)
	{
		if (glWidget->toolMode() == GLWidget::AppObjectTransform)
			toolObjectMode = ModeAppObject;
	}

	return toolObjectMode;
}


void MainWindow::setObjectMode(ObjectMode mode)
{
	objectMode_ = mode;

	if (modeSelectorComboBox_->currentIndex() != (int)mode)
	{
		ignoreModeChanged_ = true; //prevent stack overflow
		modeSelectorComboBox_->setCurrentIndex((int)mode);
		ignoreModeChanged_ = false;
	}
		
	if (document_)
	{	
		GLWidget * glWidget = curGlWidget();
		ObjectMode toolMode = toolObjectMode();

		if (mode == ModeSceneObject)
		{
			document_->deselectAllAppObjects();		
			ui.objectListDockWidget->setWidget(sceneObjectListPane_);
			propertyPane_->setObjectMode(PropertyPane::ModeSceneObject);		

			if (glWidget && toolMode != ModeSceneObject)
			{
				glWidget->setToolMode(GLWidget::Translate);
			}
		}
		else
		{
			document_->deselectAll();
			ui.objectListDockWidget->setWidget(appObjectListPane_);
			propertyPane_->setObjectMode(PropertyPane::ModeAppObject);		

			if (glWidget && toolMode != ModeAppObject)
			{
				glWidget->setToolMode(GLWidget::AppObjectTransform);
			}		
		}
	}	

	updateActions();
}

void MainWindow::newFile()
{
	QWidget * dlgParent = this;
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();
	NewProjectDlg dlg(dlgParent);
	if (dlg.exec() == QDialog::Accepted)
	{				
		GLWidget * glWidget = new GLWidget(this);
		glWidget->initNewFile(dlg.absFileName(), dlg.width(), dlg.height());
		glWidget->setAttribute(Qt::WA_DeleteOnClose);	

		QMdiSubWindow * subWindow = mdiArea_->addSubWindow(glWidget);
		glWidget->showMaximized();

		QString fileName = dlg.absFileName();
		
		subWindowData_.push_back(SubWindowData(subWindow, glWidget, fileName));
		glWidget->setWindowTitle(fileName);
		glWidget->saveAs(fileName);
		doRecentFiles(glWidget->fileName());

		glWidget->resetCamera();
		glWidget->update();
	}
}

void MainWindow::openFile()
{
	QString fileName = getOpenFileName(
		QObject::tr("Open"), QString(), QObject::tr("Studio Files (*.st)"));

	if (fileName.isEmpty()) return;
	
	openFile(fileName);	
}

void MainWindow::openFile(const	QString & pFileName)
{	
	GLWidget * glWidget;

	QString fileName = pFileName;
	for (int i = 0; i < (int)fileName.size(); ++i)
		if (fileName[i] == '\\') fileName[i] = '/';

	try
	{
		glWidget = new GLWidget(this);
		glWidget->initExistingFile(fileName);
	}
	catch(Exception & e)
	{
		QMessageBox::information(0, tr("Error"), QString::fromStdString(e.what()));
		return;
	}
	catch(...)
	{
		QMessageBox::information(0, tr("Error"), tr("Could not real file"));
		return;
	}

	vector<SubWindowData>::iterator iter = subWindowData_.begin();
	for(; iter != subWindowData_.end(); ++iter)
	{
		if((*iter).fileName == fileName)
		{
			mdiArea_->setActiveSubWindow((*iter).subWindow);
			return;
		}
	}
	
	glWidget->setWindowTitle(fileName);
	glWidget->setAttribute(Qt::WA_DeleteOnClose);
	QMdiSubWindow * subWindow = mdiArea_->addSubWindow(glWidget);
	subWindow->show();
	doRecentFiles(fileName);
	subWindowData_.push_back(SubWindowData(subWindow, glWidget, fileName));

	glWidget->resetCamera();
	glWidget->update();
}

void MainWindow::openRecentFile()
{
	QAction * action = qobject_cast<QAction *>(sender());	
	if (action)
	{
		openFile(action->data().toString());
	}
}

void MainWindow::saveFile()
{
	GLWidget * glWidget = curGlWidget();
	if (!glWidget) return;
	if (glWidget->hasBeenSaved())
	{
		glWidget->save();
		doRecentFiles(glWidget->fileName());
	}
	else
		saveAsFile();
}

void MainWindow::saveAsFile()
{
	GLWidget * glWidget = curGlWidget();
	if (!glWidget) return;
	
	QString fileName = getSaveFileName(tr("Save As"), QString(), 
		tr("Studio Files (*.st)"), "st");

	if (!fileName.isEmpty())
	{	
		curMdiSubWindow_->setWindowTitle(fileName);
		glWidget->saveAs(fileName);
		
		doRecentFiles(fileName);

		vector<SubWindowData>::iterator iter = subWindowData_.begin();
		for(; iter != subWindowData_.end(); ++iter)
		{
			if((*iter).subWindow == curMdiSubWindow_)
				(*iter).fileName = fileName;
		}
	}
}

bool MainWindow::closeFile()
{
	if (!curMdiSubWindow_) 	return false;
	return 	curMdiSubWindow_->close();
}

void MainWindow::subWindowClosing(QWidget * obj)
{
	setDocument(0);
	curMdiSubWindow_ = NULL;

	vector<SubWindowData>::iterator iter = subWindowData_.begin();
	for (; iter != subWindowData_.end(); ++iter)
	{
		if ((*iter).glWidget == obj) 
		{
			subWindowData_.erase(iter);
			break;
		}
	}	
	
	if(!subWindowData_.empty())
	{
		GLWidget * widget = subWindowData_.at(subWindowData_.size()-1).glWidget;
		onSubWindowActivating(subWindowData_[subWindowData_.size()-1].subWindow);		
		mdiArea_->setActiveSubWindow(curMdiSubWindow_);
		setDocument(widget->document());
		Global::instance().setCurDocument(document_->document());
	}
		
	updateActions();
}

void MainWindow::onAbout()
{
	AboutDlg(this).exec();	
}

void MainWindow::onSelect()
{
	curGlWidget()->setToolMode(GLWidget::Select);
	setObjectMode(ModeSceneObject);
	updateActions();
}

void MainWindow::onTranslate()
{
	curGlWidget()->setToolMode(GLWidget::Translate);
	setObjectMode(ModeSceneObject);
	updateActions();
}

void MainWindow::onRotate()
{
	curGlWidget()->setToolMode(GLWidget::Rotate);
	setObjectMode(ModeSceneObject);
	updateActions();
}

void MainWindow::onScale()
{
	curGlWidget()->setToolMode(GLWidget::Scale);
	setObjectMode(ModeSceneObject);
	updateActions();
}

void MainWindow::onPivot()
{
	curGlWidget()->setToolMode(GLWidget::Pivot);
	setObjectMode(ModeSceneObject);
	updateActions();
}

void MainWindow::onAppObjectTransform()
{
	curGlWidget()->setToolMode(GLWidget::AppObjectTransform);
	setObjectMode(ModeAppObject);
	updateActions();
}

void MainWindow::importPdf(const QString & fileName)
{
	int importedPDFCount = 0;
	QString outDir;// = QString("./Res/pdf_%1").arg(importedPDFCount);
	QDir dir;
	do
	{
		importedPDFCount++;
		outDir = QString("./Res/pdf_%1").arg(importedPDFCount);
		dir.setPath(outDir);
	}while (dir.exists());
	dir.mkpath(dir.absolutePath());
	//dir.mkpath(dir.absolutePath() + QObject::tr("/zoom"));

	float width = document_->selectedScene()->screenWidth();
	float height = document_->selectedScene()->screenHeight();

	// Create a progress dialog
	QProgressDialog progress1(QObject::tr("Converting file"),
		QObject::tr("Cancel"), 0, 100, this);
	progress1.setWindowTitle(QObject::tr("Progress"));
	progress1.setWindowModality(Qt::WindowModal);
	QProgressBar bar(&progress1);
	bar.setTextVisible(false);
	progress1.setBar(&bar);
	progress1.show();
	
	int roop = 0;

	progress1.setValue(0);
	qApp->processEvents();

	QVariant qwidth(width);
	QVariant qheight(height);

	QString program = EditorGlobal::instance().pdf2imgLocation();//"C:/git_working/pdf2img/Debug/pdf2img.exe";
	QDir tempdir("C:/temp");
	bool hasdir = tempdir.exists();
	if (!hasdir) _mkdir("C:/temp");
	QString newFileName = "C:/temp/380a558a-9f74-4f60-be48-0123181812ce.pdf";
	QFile::copy(fileName, newFileName);
	QStringList arguments;
	arguments << qStringToStdString(newFileName).c_str() << qStringToStdString(dir.absolutePath()).c_str()
			  << qStringToStdString(qwidth.toString()).c_str() << qStringToStdString(qheight.toString()).c_str();

	QProcess *myProcess = new QProcess(this);
	myProcess->start(program, arguments);

	while(myProcess->state() != QProcess::NotRunning)
	{
		progress1.setValue(roop++);
		if (roop > 99) roop = 0;
		qApp->processEvents();
		if (progress1.wasCanceled())
		{
			myProcess->kill();
			myProcess->waitForFinished(-1);
			QMessageBox::information(
				0, 
				QObject::tr("Cancel"), 
				QObject::tr("Cancel"));
			return;
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}
	progress1.setValue(100);

	myProcess->kill();
	myProcess->waitForFinished(-1);

	//convertPng2Jpg(dir);
	
	QFileInfoList fileList = dir.entryInfoList();

	//int sceneNum = document_->scenes().size();

	std::vector<SceneObject *> Objs;
	Global::instance().sceneObjects(&Objs);
	double ratio = 0;

	int scenesSize = fileList.size();
	// Create a progress dialog
	QProgressDialog progress(QObject::tr("Scene make"),
		QObject::tr("Cancel"), 0, scenesSize, this);
	progress.setWindowTitle(QObject::tr("Progress"));
	progress.setWindowModality(Qt::WindowModal);
	
	for (int nCnt = 2; nCnt < scenesSize; ++nCnt) {
		progress.setValue(nCnt);
		qApp->processEvents();
		QFileInfo fileInfo = fileList.at(nCnt);

		if (!fileInfo.isFile())
			continue;

		document_->doCommand(new NewSceneCmd(document_, width, height));

		Scene* s = document_->selectedScene();

		QString fileName = fileInfo.absolutePath() + tr("/") + fileInfo.fileName();

		Image * img = new Image();
		img->setFileName(qStringToStdString(fileName));		
		img->init(document_->renderer());

		while(!img->isLoaded())
		{
			img->asyncLoadUpdate();
			boost::this_thread::sleep(boost::posix_time::milliseconds(15));
		}
		
		if (img->width() > 2048 || img->height() > 2048)
		{
			QMessageBox::information(
				0, 
				QObject::tr("Error"), 
				QObject::tr("Image width or height cannot exceed 2048 pixels"));
			delete img;
			++nCnt;
			continue;
		}

		img->setId(getFileTitle(qStringToStdString(fileName)));
		ratio = width/img->texture()->width();
		if (ratio > height/img->texture()->height())
			ratio = height/img->texture()->height();
		float w = img->texture()->width();
		float h = img->texture()->height();
		//img->setWidth(img->texture()->width());
		//img->setHeight(img->texture()->height());

		img->setWidth(w*ratio);
		img->setHeight(h*ratio);

		Transform t = img->transform();
		t.setTranslation((width/2) - (img->width()/2), (height/2) - (img->height()/2), t.translation().z);
		img->setTransform(t);

		vector<SceneObjectSPtr> newObjs;
		newObjs.push_back( SceneObjectSPtr(img));
		document_->doCommand(new NewObjectsCmd(
			document_, s->root(), -1, newObjs));

		if (progress.wasCanceled())
		{
			QMessageBox::information(
				0, 
				QObject::tr("Cancel"), 
				QObject::tr("Cancel"));
			return;
		}
	}
	progress.setValue(scenesSize);

	QFile::remove(newFileName);
	if (!hasdir) _rmdir("C:/temp");
}

void MainWindow::onImportPDF()
{
	QString fileName = getOpenFileName(
		QObject::tr("Import"), QString(), QObject::tr("PDF Files (*.pdf)"));

	if (fileName.isEmpty()) return;

	importPdf(fileName);
}

void MainWindow::onImportFolder()
{
	QString folder = getExistingDirectory(
		QObject::tr("Import"), QString());

	if (folder.isEmpty()) return;

	int folderCheckCount = 0;
	QString outDir;// = QString("./Res/pdf_%1").arg(importedPDFCount);
	QDir dir;
	do
	{
		folderCheckCount++;
		outDir = QString("./Res/folder_%1").arg(folderCheckCount);
		dir.setPath(outDir);
	}while (dir.exists());
	dir.mkpath(dir.absolutePath());
	dir.mkpath(dir.absolutePath() + QObject::tr("/zoom"));


	QDir origDir(folder);

	QFileInfoList fileList = origDir.entryInfoList();
	float width = document_->selectedScene()->screenWidth();
	float height = document_->selectedScene()->screenHeight();

	//int sceneNum = document_->scenes().size();

	std::vector<SceneObject *> Objs;
	Global::instance().sceneObjects(&Objs);
	double ratio = 0;

	int pageNumbers = fileList.size();

	QProgressDialog progress(QObject::tr("import file"),
		QObject::tr("Cancel"), 0, pageNumbers, this);
	progress.setWindowTitle(QObject::tr("Progress"));
	progress.setWindowModality(Qt::WindowModal);

	for (int nCnt = 0; nCnt < pageNumbers; ++nCnt) {
		progress.setValue(nCnt);
		qApp->processEvents();

		if (progress.wasCanceled())
		{
			QMessageBox::information(
				0, 
				QObject::tr("Cancel"), 
				QObject::tr("Cancel"));
			return;
		}

		QFileInfo fileInfo = fileList.at(nCnt);

		QString fileName = fileInfo.absoluteFilePath();
		std::string fileExt = getFileExtension(qStringToStdString(fileName));	

		if (!boost::iequals(fileExt, "jpg") &&
			!boost::iequals(fileExt, "jpeg") &&
			!boost::iequals(fileExt, "png") &&
			!boost::iequals(fileExt, "bmp"))
			continue;

		QString translatedImageFileName = dir.absolutePath() + tr("/") + fileInfo.fileName();
		QString zoomtranslatedImageFileName = dir.absolutePath() + tr("/zoom/") + fileInfo.fileName() + tr(".zoom.jpg");
		QImage qimage;
		qimage.load(fileName);
		float qwidth = qimage.width();
		float qheight = qimage.height();
		if (qwidth > width || qheight > height)
		{
			QSize qsize(width, height);
			QImage trqimage = qimage.scaled(qsize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			trqimage.save(translatedImageFileName);
			if (qwidth > width*2 || qheight > height*2)
			{
				qsize = QSize(width * 2, height * 2);
				trqimage = qimage.scaled(qsize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				trqimage.save(zoomtranslatedImageFileName);
			}
			else
				qimage.save(zoomtranslatedImageFileName);
		}
		else
			qimage.save(translatedImageFileName);


		document_->doCommand(new NewSceneCmd(document_, width, height));

		Scene* s = document_->selectedScene();

		Image * img = new Image();
		img->setFileName(qStringToStdString(translatedImageFileName));		
		img->init(document_->renderer());

		while(!img->isLoaded())
		{
			img->asyncLoadUpdate();
			boost::this_thread::sleep(boost::posix_time::milliseconds(15));
		}
		
		if (img->width() > 2048 || img->height() > 2048)
		{
			QMessageBox::information(
				0, 
				QObject::tr("Error"), 
				QObject::tr("Image width or height cannot exceed 2048 pixels"));
			delete img;
			++nCnt;
			continue;
		}

		img->setId(getFileTitle(qStringToStdString(fileName)));
		//ratio = width/img->texture()->width();
		//if (ratio > height/img->texture()->height())
		//	ratio = height/img->texture()->height();
		//float w = img->texture()->width();
		//float h = img->texture()->height();
		//img->setWidth(img->texture()->width());
		//img->setHeight(img->texture()->height());

		//img->setWidth(w*ratio);
		//img->setHeight(h*ratio);

		Transform t = img->transform();
		t.setTranslation((width/2) - (img->width()/2), (height/2) - (img->height()/2), t.translation().z);
		img->setTransform(t);

		vector<SceneObjectSPtr> newObjs;
		newObjs.push_back( SceneObjectSPtr(img));
		document_->doCommand(new NewObjectsCmd(
			document_, s->root(), -1, newObjs));
	}
	progress.setValue(pageNumbers);
}

void MainWindow::importGif(const QString & fileName)
{
	int folderCheckCount = 0;
	QString outDir;// = QString("./Res/pdf_%1").arg(importedPDFCount);
	QDir dir;
	do
	{
		folderCheckCount++;
		outDir = QString("./Res/images_%1").arg(folderCheckCount);
		dir.setPath(outDir);
	}while (dir.exists());
	if(!dir.mkpath(dir.absolutePath())) return;

	FILE * giffile;
	char *cbuf, *refbuf;
	long lSize;
	float delay = 0;
	if ((giffile = fopen(qStringToStdString(fileName).c_str(), "rb")) != NULL)
	{
		fseek (giffile , 0 , SEEK_END);
		lSize = ftell (giffile);
		rewind(giffile);

		cbuf = (char*) malloc (sizeof(char)*lSize);
		refbuf = cbuf;

		fread(cbuf,1,lSize,giffile);

		for(int i = 0 ; i < lSize ; i++)
		{
			if (*refbuf == 33 && *(refbuf+1) == -7 && *(refbuf+2) == 4)
			{
				delay = (int)*(refbuf+4);
				break;
			}
			refbuf++;
		}
		free(cbuf);
		fclose(giffile);
	}

	QMovie gif(fileName);

	std::vector<std::string> fileNames;

	if (delay < 0) delay = 256 + delay;
	else if (delay == 0) delay = 10;
	float fps = 100.0 / delay;

	for(int i = gif.currentFrameNumber() ; i < gif.frameCount() ; i++)
	{
		QImage qimage = gif.currentImage();
		if (qimage.isNull())
		{
			gif.jumpToNextFrame();
			continue;
		}
		QString savename = dir.path() + QString("/image_%1.jpg").arg(i);
		//QString path = dir.path();
		qimage.save(savename);
		fileNames.push_back(qStringToStdString(savename));
		gif.jumpToNextFrame();
	}

	Scene* s = document_->selectedScene();

	ImageAnimation * img = new ImageAnimation;
	img->setFileNames(fileNames);		
	img->init(document_->renderer());
	img->setFps(fps);
	img->setRepeat(true);

	const vector<TextureSPtr> & buffers = img->buffers();
	if (!buffers.empty())
	{
		int trWidth, trHeight;
		Global::instance().getImageDims(buffers[0]->fileName(), &trWidth, &trHeight);

		img->setWidth(trWidth);
		img->setHeight(trHeight);
	}

	vector<SceneObjectSPtr> newObjs;
	newObjs.push_back( SceneObjectSPtr(img));
	document_->doCommand(new NewObjectsCmd(
		document_, s->root(), -1, newObjs));
}

void MainWindow::onImportGif()
{
	QString fileName = getOpenFileName(
		QObject::tr("Import"), QString(), QObject::tr("GIF Files (*.gif)"));

	if (fileName.isEmpty()) return;

	importGif(fileName);
}

void MainWindow::onNudgePlusX()
{
	curGlWidget()->nudgeObjects(Vector3(1, 0, 0));
}

void MainWindow::onNudgeMinusX()
{
	curGlWidget()->nudgeObjects(Vector3(-1, 0, 0));
}

void MainWindow::onNudgePlusY()
{
	curGlWidget()->nudgeObjects(Vector3(0, 1, 0));
}

void MainWindow::onNudgeMinusY()
{
	curGlWidget()->nudgeObjects(Vector3(0, -1, 0));
}

void MainWindow::onNudgeTenPlusX()
{
	curGlWidget()->nudgeObjects(Vector3(10, 0, 0));
}

void MainWindow::onNudgeTenMinusX()
{
	curGlWidget()->nudgeObjects(Vector3(-10, 0, 0));
}

void MainWindow::onNudgeTenPlusY()
{
	curGlWidget()->nudgeObjects(Vector3(0, 10, 0));
}

void MainWindow::onNudgeTenMinusY()
{
	curGlWidget()->nudgeObjects(Vector3(0, -10, 0));
}

void MainWindow::onPaste()
{
	EditorDocument * document = curGlWidget()->document();


	vector<SceneObjectSPtr> pasteObjs = EditorGlobal::instance().pasteObjects(
		document->document(), document->selectedScene());

	document->doCommand(new PasteObjectsCmd(
		document, pasteObjs));
}

void MainWindow::onDelete()
{
	if(objectMode_ == ModeSceneObject)
	{
		document_->doCommand(new DeleteObjectsCmd(document_, document_->selectedScene(),
			document_->selectedObjects()));
	}

	else if(objectMode_ == ModeAppObject)
	{
		document_->doCommand(new DeleteAppObjectsCmd(document_, document_->selectedScene(),
			document_->selectedAppObjects()));
	}
}

void MainWindow::onCopy()
{
	EditorDocument * document = curGlWidget()->document();
	EditorGlobal::instance().copyObjects(document->selectedObjects());
	updateActions();
}

void MainWindow::onPreferences()
{
	PreferencesDlg dlg(this);
	dlg.exec();

	if (curGlWidget())
		curGlWidget()->timerCheck();
	updateActions();
}

void MainWindow::onDocumentProperties()
{
	EditorDocument * document = curGlWidget()->document();
	DocumentPropertiesDlg dlg(document, this);
	if (dlg.exec())
	{
		document->doCommand(
			new ChangeDocumentPropertiesCmd(document,
				dlg.orientation(), dlg.width(), dlg.height(), 
				dlg.newDocumentTemplate(), dlg.scriptFiles(), dlg.scriptAccessibleFiles(),
				dlg.remoteReadServer(), dlg.allowMultitouch(), dlg.preDownload()));
	}
}

void MainWindow::onDocTemplateProperties()
{
	EditorDocument * document = curGlWidget()->document();
	Document * sceneDoc = document->document();
	DocumentTemplate * docTemplate = sceneDoc->documentTemplate().get();
	EditorDocumentTemplate * edDoc = EditorGlobal::instance().
		editorDocumentTemplate(docTemplate);

	Command * cmd = edDoc->doProperties(docTemplate, document, this);
	if (cmd)
	{
		document->doCommand(cmd);
	}
}

void MainWindow::onPageTrackingUrls()
{
	PageTrackingUrlsDlg dlg(this);
	if (dlg.exec())
	{
		std::string url = dlg.url();
		vector<Scene *> scenes;
		document_->document()->documentTemplate()->pageScenes(&scenes);
		document_->doCommand(new SetTrackingUrlsCmd(scenes, url));
	}
}

void MainWindow::onPreview()
{
	GLWidget * glWidget = curGlWidget();
	if (!glWidget) return;
	QString fileName = glWidget->saveTemporary();
	playerProcess_.reset(new PlayerProcessData);
	playerProcess_->fileName = fileName;
	connect(&playerProcess_->process, SIGNAL(error(QProcess::ProcessError)), 
		this, SLOT(onPlayerError(QProcess::ProcessError)));

	QString playerProg = EditorGlobal::instance().playerLocation();
	playerProcess_->process.start(playerProg, QStringList(fileName));	
}

void MainWindow::onUndo()
{
	if (curGlWidget()) curGlWidget()->onUndo();
	
}

void MainWindow::onRedo()
{
	if (curGlWidget()) curGlWidget()->onRedo();
}

void MainWindow::onPublish()
{
	Document * sceneDoc = document_->document();
	const vector<SceneSPtr> & scenes = sceneDoc->scenes();

	QString errorMsg;
	bool wasError = false;

	BOOST_FOREACH(SceneSPtr scene, scenes)
	{
		vector<string> refFiles;
		scene->referencedFiles(&refFiles);

		BOOST_FOREACH(string file, refFiles)
		{
			file = getAbsFileName(Global::instance().readDirectory(), file);
			if (!QFile::exists(stdStringToQString(file)))
			{
				wasError = true;			
				stringstream ss;
				ss << "In scene \"" << scene->name() << "\", the file \"" << file << "\" could not be found.\n";
				errorMsg += stdStringToQString(ss.str());
			}
		}
	}


	if (wasError)
	{
		QMessageBox::information(0, tr("Error"), 
			errorMsg);
		return;
	}


	PublishDlg dlg(document_->document(), this);

	if (dlg.exec())
	{
		QString dir = dlg.directory();
		QString file = dlg.file();
		bool isCatalogDocument = dlg.isCatalogDocument();

		PublishingDlg pDlg(document_->document(), 
			dir, file, isCatalogDocument, this);
		pDlg.exec();		
	}

}

bool MainWindow::handleAlreadyOpen(const QString & fileName)
{
	bool alreadyOpen = false;
	vector<SubWindowData>::iterator iter = subWindowData_.begin();
	for(; iter != subWindowData_.end(); ++iter)
	{
		if((*iter).fileName == fileName)
		{
			alreadyOpen = true;
			mdiArea_->setActiveSubWindow((*iter).subWindow);
			break;
		}
	}
	return alreadyOpen;
}

void MainWindow::doRecentFiles(const QString & fileName)
{
	recentFiles_.removeAll(fileName);
	recentFiles_.prepend(fileName);
	updateRecentFilesActions();
}

void MainWindow::updateRecentFilesActions()
{
	for (int i = 0; i < sMaxRecentFiles; ++i)
	{
		if (i < recentFiles_.count() && QFile::exists(recentFiles_[i]))
		{
			recentFileActions_[i]->setText(recentFiles_[i]);
			recentFileActions_[i]->setData(recentFiles_[i]);				
			recentFileActions_[i]->setVisible(true);
		}
		else
		{
			recentFileActions_[i]->setVisible(false);
		}
	}
}

void MainWindow::updateActions()
{
	GLWidget * glWidget = curGlWidget();
	
	ui.actionRedo->setEnabled(
		glWidget && glWidget->document()->redoAvailable());
	ui.actionUndo->setEnabled(
		glWidget && glWidget->document()->undoAvailable());
	
	bool isDocOpen = curMdiSubWindow_ != NULL;

	ui.actionClose->setEnabled(isDocOpen);
	ui.actionPreview->setEnabled(isDocOpen);
	ui.actionSave->setEnabled(isDocOpen);
	ui.actionSaveAs->setEnabled(isDocOpen);	
	ui.actionPublish->setEnabled(isDocOpen);
	ui.actionPageTrackingUrls->setEnabled(isDocOpen);

	ui.actionDocumentProperties->setEnabled(isDocOpen);
	ui.actionDocumentTemplateProperties->setEnabled(isDocOpen);
	
	ui.actionSelect->setEnabled(isDocOpen);
	ui.actionTranslate->setEnabled(isDocOpen);
	ui.actionScale->setEnabled(isDocOpen);
	ui.actionRotate->setEnabled(isDocOpen);
	ui.actionPivot->setEnabled(isDocOpen);
	ui.actionTransformAppObject->setEnabled(isDocOpen);

	ui.action_ImportPDF->setEnabled(isDocOpen);
	ui.actionImportFolder->setEnabled(isDocOpen);
	ui.actionImportGif->setEnabled(isDocOpen);
	ui.actionExportXML->setEnabled(isDocOpen);
	ui.actionImportPDFtemp->setEnabled(isDocOpen);

	modeSelectorComboBox_->setEnabled(isDocOpen);

	if (glWidget)
	{
		GLWidget::ToolMode toolMode = glWidget->toolMode();
		ui.actionSelect->setChecked(toolMode == GLWidget::Select);
		ui.actionTranslate->setChecked(toolMode == GLWidget::Translate);
		ui.actionScale->setChecked(toolMode == GLWidget::Scale);
		ui.actionRotate->setChecked(toolMode == GLWidget::Rotate);
		ui.actionPivot->setChecked(toolMode == GLWidget::Pivot);
		ui.actionTransformAppObject->setChecked(
			toolMode == GLWidget::AppObjectTransform);
	}


	bool nudgeableObjSel =
		glWidget && 
		glWidget->document()->selectedObject() &&
		glWidget->document()->selectedObject()->visualAttrib() && glWidget->toolMode_ == GLWidget::Translate;

	ui.actionNudgePlusX->setEnabled(nudgeableObjSel);
	ui.actionNudgePlusY->setEnabled(nudgeableObjSel);
	ui.actionNudgeMinusX->setEnabled(nudgeableObjSel);
	ui.actionNudgeMinusY->setEnabled(nudgeableObjSel);
	ui.actionNudge_10X->setEnabled(nudgeableObjSel);
	ui.actionNudge_10X_2->setEnabled(nudgeableObjSel);
	ui.actionNudge_10Y->setEnabled(nudgeableObjSel);
	ui.actionNudge_10Y_2->setEnabled(nudgeableObjSel);

	bool pasteAvailable = 
		glWidget && EditorGlobal::instance().pasteObjectsAvailable();
	ui.actionPaste->setEnabled(pasteAvailable && objectMode_ == ModeSceneObject);
	bool copyAvailable = 
		glWidget && !glWidget->document()->selectedObjects().empty();
	ui.actionCopy->setEnabled(copyAvailable && objectMode_ == ModeSceneObject);
	ui.actionDelete->setEnabled(isDocOpen);
	
	/*
	ui.actionDelete->setEnabled(
		(glWidget && !glWidget->document()->selectedObjects().empty() &&
		objectMode_ == ModeSceneObject) ||
		(glWidget && !glWidget->document()->selectedAppObjects().empty() &&
		objectMode_ == ModeAppObject));
	*/

	/*
	ui.actionDelete->setEnabled(glWidget && !glWidget->document()->selectedObjects().empty() &&
		objectMode_ == ModeSceneObject);
	*/
}

void MainWindow::setDocument(EditorDocument * document)
{
	if(document && document == document_) return;
	document_ = document;

	if(document_){
		Global::instance().setCurDocument(document_->document());
	}

	setPanesDocument(document);
}

void MainWindow::setPanesDocument(EditorDocument * document)
{
	GLWidget * widget = curGlWidget();
	if(widget) widget->makeCurrent();
	propertyPane_->setDocument(document);	
	animationPane_->setDocument(document);
	sceneObjectListPane_->setDocument(document);
	appObjectListPane_->setDocument(document);
	eventActionPane_->setDocument(document);
	sceneListPane_->setDocument(document);

	emit documentChanged(document);
}

void MainWindow::convertPng2Jpg(const QDir & dir)
{
	QFileInfoList fileList = dir.entryInfoList();

	vector<string> pngFiles;
	for (int nCnt = 2; nCnt < fileList.size(); nCnt++) {
		QFileInfo fileInfo = fileList.at(nCnt);
		if (fileInfo.isFile())
		{
			if (fileInfo.suffix() != tr("png"))
				continue;
			QString origQImageFile = fileInfo.filePath();
			pngFiles.push_back(qStringToStdString(origQImageFile));
			QString qImageFile = fileInfo.absolutePath() + tr("/") + fileInfo.baseName() + tr(".jpg");
			QImage qimage;
			qimage.load(origQImageFile);
			qimage = qimage.scaled(QSize(qimage.width(), qimage.height()), Qt::KeepAspectRatio, Qt::SmoothTransformation);
			qimage.save(qImageFile, "jpg");
		}
		else if(fileInfo.isDir())
		{
			QDir qdir;
			qdir.setPath(fileInfo.filePath());
			convertPng2Jpg(qdir);
		}
	}
	for (int i = 0 ; i < pngFiles.size() ; i++)
	{
		remove(pngFiles[i].c_str());
	}
}

void MainWindow::onSetKey()
{
	const vector<AnimationChannel *> & chans = document_->selectedChannels();

	Animation * anim = document_->selectedAnimation();

	SceneObject * obj = document_->selectedObject();

	if (anim != NULL && obj != NULL)
	{
		vector<AnimationChannelSPtr> setKeyChans;
		foreach(AnimationChannel * chan, chans)
		setKeyChans.push_back(anim->findChannel(chan));	
	
		try {
			document_->doCommand(new SetKeyFramesCmd(document_,
				anim, obj, setKeyChans, document_->timeLineTime())); 
		} catch(exception & e) {
			QMessageBox::information(0, tr("Error"), tr(e.what()));
		}	
	}
}

void MainWindow::setView(GLWidget * widget)
{
	propertyPane_->setView(widget);
}

void MainWindow::keyPressEvent(QKeyEvent * keyEvent)
{
	if(keyEvent->key() == Qt::Key_Control) isCtrlKey_ = true;

	if(isCtrlKey_ && keyEvent->key() == Qt::Key_D)
	{
		document_->deselectAll();
		document_->deselectAllAppObjects();
	}

//	else if (keyEvent->key() == Qt::Key_Return &&
//		 document_->selectedAnimation())
//  {
//		if (animationPane_->timeLinePane()->isPlaying())
//			animationPane_->timeLinePane()->onStop();
//		else
//			animationPane_->timeLinePane()->onPlay();
//	}
//	else if (keyEvent->key() == Qt::Key_F6 &&
//		!document_->selectedChannels().empty())
//  {
//		animationPane_->timeLinePane()->onSetKey();
//  }
	else if (keyEvent->key() == Qt::Key_F7)
		ui.propertiesDockWidget->raise();
	else if (keyEvent->key() == Qt::Key_F8)
		ui.eventActionDockWidget->raise();
	else if (keyEvent->key() == Qt::Key_F9)
		setObjectMode(ModeSceneObject);
	else if (keyEvent->key() == Qt::Key_F10)
		setObjectMode(ModeAppObject);
//  else if (keyEvent->key() >= Qt::Key_1 && keyEvent->key() <= Qt::Key_9 &&
//		  isKeyDown(VK_CONTROL))
//	setCameraIndex(keyEvent->key());
//	else
//		QWidget::keyPressEvent(keyEvent);

	if(curGlWidget())
	{	
		if(curGlWidget()->isGLWidgetKeyPressed_ == false)
			curGlWidget()->keyPressEvent(keyEvent);
		curGlWidget()->isGLWidgetKeyPressed_ = false;
	}
}

void MainWindow::keyReleaseEvent(QKeyEvent * keyEvent)
{
	if(keyEvent->key() == Qt::Key_Control) isCtrlKey_ = false;

	if(curGlWidget())
		curGlWidget()->keyReleaseEvent(keyEvent);
}

bool MainWindow::eventFilter(QObject * obj, QEvent * event)
{
	if((obj == ui.menuBar && event->type() == QEvent::MouseButtonPress) ||
		(obj == ui.menuFile && event->type() == QEvent::MouseMove) || 
		(obj == ui.menu_Edit && event->type() == QEvent::MouseMove) ||
		(obj == ui.menu_View && event->type() == QEvent::MouseMove) ||
		(obj == ui.menuTool && event->type() == QEvent::MouseMove) ||
		(obj == ui.menuExtra && event->type() == QEvent::MouseMove) ||
		(obj == ui.menu_Help && event->type() == QEvent::MouseMove))
	{
		isCtrlKey_ = false;
		if(curGlWidget())
		{
			curGlWidget()->isShiftKey_ = false;
			curGlWidget()->isAltKey_ = false;
			curGlWidget()->isCtrlKey_ = false;
		}
	}
	
	return QMainWindow::eventFilter(obj, event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent * event)
{
	const QMimeData * data = event->mimeData();

	if(data->hasUrls())
		event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent * event)
{
	const QMimeData * data = event->mimeData();

	if (data->hasUrls())
	{
		QList<QUrl> urls = data->urls();         

		vector<SceneObjectSPtr> newObjs;
		BOOST_FOREACH(QUrl url, urls)
		{
			QString localFile = url.toLocalFile();
			if (checkFileExtension(qStringToStdString(localFile), "st"))
				openFile(localFile);
			else if (curMdiSubWindow_ != NULL)
			{
				if (checkFileExtension(qStringToStdString(localFile), "pdf"))
					importPdf(localFile);
				else if (checkFileExtension(qStringToStdString(localFile), "gif"))
					importGif(localFile);
			}
		}
	}
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	isClosingLogout_ = true;

	while(curMdiSubWindow_)
	{
		if(!closeFile())
		{
			event->ignore();
			isClosingLogout_ = false;
			break;
		}
	}

	if(loginDlg_->checkTrialState() && isClosingLogout_)
	{
		event->ignore();
		onLogout();
		isLogout_ = true;
	}

	if(isClosingLogout_)
	{
		EditorGlobal::instance().settings()->setValue("recentFiles", recentFiles_);
		QRect geo = geometry();
		EditorGlobal::instance().settings()->setValue("windowGeometry", geo);
		EditorGlobal::instance().settings()->setValue("maximized", isMaximized());
	}
}

void MainWindow::onSubWindowActivating(QMdiSubWindow * subWindow)
{
	if(!subWindow) return;
	if(curMdiSubWindow_ == subWindow) return;

	if(curMdiSubWindow_)
	{
		GLWidget * prevGlWidget = (GLWidget *)curMdiSubWindow_->widget();
		
		prevGlWidget->setToolMode(GLWidget::Select);
		setObjectMode(ModeSceneObject);

		EditorDocument * prevDoc = prevGlWidget->document();
		bool ret = prevDoc->disconnect(this);
		prevGlWidget->disconnect(this);
		prevGlWidget->makeCurrent();
		
		setDocument(0);
	}
	curMdiSubWindow_ = subWindow;
	GLWidget * glWidget = (GLWidget *)subWindow->widget();
	glWidget->makeCurrent();

	EditorDocument * document = glWidget->document();

	std::string dir = 
		getDirectory(qStringToStdString(glWidget->fileName())).c_str();
	Global::instance().setDirectories(dir, dir, "");
	chdir(dir.c_str());

	setView(glWidget);

	connect(glWidget, SIGNAL(toolChanged()), this, SLOT(onToolChanged()));

	connect(glWidget->document(), SIGNAL(objectSelectionChanged()), 
		this, SLOT(onObjectSelectionChanged()));

	connect(glWidget->document(), SIGNAL(appObjectSelectionChanged()),
		this, SLOT(onAppObjectSelectionChanged()));

	connect(glWidget->document(), SIGNAL(commandHistoryChanged()), 
		this, SLOT(onCommandHistoryChanged()));	

	glWidget->repaint();
	setDocument(glWidget->document());

	updateActions();
}

void MainWindow::onImportXML()
{
	QString fileName = getOpenFileName(
		QObject::tr("Open"), QString(), QObject::tr("XML Files (*.xml)"));

	if (fileName.isEmpty()) return;

	openFile(fileName);
	/*XmlReader xmlReader;
	Document* doc = xmlReader.read(qStringToStdString(fileName).c_str());	

	std::string stfilename = getDirectory(qStringToStdString(fileName)) + 
							 getFileTitle(qStringToStdString(fileName)) + "_xml.st";

	Writer(stfilename.c_str(), 0).
		write(doc);

	openFile(stdStringToQString(stfilename));*/
}

void MainWindow::onExportXML()
{
	std::string fileName = qStringToStdString(curGlWidget()->fileName());
	std::string xmlfilename = getDirectory(fileName) + 
							 getFileTitle(fileName) + ".xml";

	XmlWriter xmlWriter;
	xmlWriter.write(document_->document(), xmlfilename.c_str());
}

void MainWindow::onChangePassword()
{
	ChangePasswordDlg * dlg = new ChangePasswordDlg(this);
	dlg->exec();
	delete dlg;
}

void MainWindow::onLogin()
{
	loginDlg_->isDialogShow_ = true;
	loginDlg_->initDialog();
	loginDlg_->exec();
	loginDlg_->isDialogShow_ = false;
	if(loginDlg_->checkTrialState()) isLogin_ = true;
	else isLogin_ = false;
}

void MainWindow::onLogout()
{
	if(isLogin_)
	{
		if(curGlWidget() && isMenuLogout_)
		{
			QMessageBox msgBox;
			msgBox.setText("The document will be lost.");
			msgBox.setInformativeText("Are you sure?");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::No);
			int ret = msgBox.exec();

			switch(ret)
			{
			case QMessageBox::Yes:
				loginDlg_->onLogout();
				isLogin_ = false;
				break;
			case QMessageBox::No:
				break;
			default:
				break;
			}
			isMenuLogout_ = false;
		}
		else
		{	
			loginDlg_->onLogout();
			isLogin_ = false;
		}
	}
}

void MainWindow::onMenuLogout()
{
	isMenuLogout_ = true;
	onLogout();
}

void MainWindow::onImportPDFtemprary()
{
	QStringList fileNames = getOpenFileNames(
		QObject::tr("Open"), QString(), QObject::tr("PDF Files (*.pdf)"));

	if (fileNames.isEmpty()) return;
	int cnt = 1;
	for (int i = 0 ; i< fileNames.size() ; i++)
	{
		QString fileName = fileNames.at(i);
		QString newFileName;
		std::string stddir = getDirectory(qStringToStdString(fileName));
		do 
		{
			newFileName = QString("a%1.pdf").arg(cnt++);
			newFileName = stdStringToQString(stddir) + newFileName;
		} while (QFile::exists(newFileName));

		QFile::copy(fileName, newFileName);

		fileName = newFileName;

		std::string xml = getDirectory(qStringToStdString(fileName)) + getFileTitle(qStringToStdString(fileName)) + ".xml";


		QString program = EditorGlobal::instance().pdf2textLocation();
		QStringList arguments;
		arguments << qStringToStdString(fileName).c_str();

		// Create a progress dialog
		QProgressDialog progress1(QObject::tr("Converting file"),
			QObject::tr("Cancel"), 0, 100, this);
		progress1.setWindowTitle(QObject::tr("Progress"));
		progress1.setWindowModality(Qt::WindowModal);
		QProgressBar bar(&progress1);
		bar.setTextVisible(false);
		progress1.setBar(&bar);
		progress1.show();

		int roop = 0;

		progress1.setValue(0);
		qApp->processEvents();

		QString xpdfrc = EditorGlobal::instance().xpdfrcLocation();
		QString newxpdfrc = QDir::currentPath() + "/xpdfrc";
		QFile::copy(xpdfrc, newxpdfrc);

		QProcess *myProcess = new QProcess(this);
		myProcess->start(program, arguments);

		while(myProcess->state() != QProcess::NotRunning)
		{
			progress1.setValue(roop++);
			if (roop > 99) roop = 0;
			qApp->processEvents();
			if (progress1.wasCanceled())
			{
				myProcess->kill();
				myProcess->waitForFinished(-1);
				QMessageBox::information(
					0, 
					QObject::tr("Cancel"), 
					QObject::tr("Cancel"));
				QFile::remove(newxpdfrc);
				return;
			}
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		}

		myProcess->kill();
		myProcess->waitForFinished(-1);

		PdfToTextTemp pdf2text(document_, fileName, this);
		pdf2text.readXml(stdStringToQString(xml), &progress1);

		QFile::remove(newxpdfrc);
		QFile::remove(fileName);
	}
}
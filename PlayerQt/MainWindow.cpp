#include "stdafx.h"
#include "MainWindow.h"
#include "Document.h"
#include "STWindow.h"
#include "TextFeatures.h"
#include "PcVideoPlayer.h"
#include "PcPlayerSupport.h"
#include "PcScene.h"
#include "GfxRenderer.h"
#include "TextEditObject.h"
#include "LogDlg.h"
#include "TextSelectUI.h"
#include "Utils.h"
#include "FileUtils.h"
#include "ImgButton.h"

using namespace std;

BgWindow::BgWindow(MainWindow * mainWindow)
{
	setParent(mainWindow);
	setStyleSheet("background-color: #555555;");
}

bool BgWindow::initDocument(const std::string & fileName)
{
	return true;
}

void BgWindow::paintEvent(QPaintEvent * event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool BgWindow::resizeToFitContent(int * width, int * height) const
{
	return false;
}

void BgWindow::setPreserveAspect(bool preserveAspect) {}

void BgWindow::onNextPage() {}

void BgWindow::onPreviousPage() {}

void BgWindow::uninitDocument() {}

void initMediaEnv(HWND);

TitleBar::TitleBar(MainWindow * mainWindow) : QWidget(mainWindow)
{
	title_ = QPixmap(":/PlayerQt/Resources/title.png");
	this->setMouseTracking(true);
	mainWindow_ = mainWindow;
	isDragging_ = false;	

	minimizeButton_ = new ImgButton(":/PlayerQt/Resources/bt_small_size.png", this);
	maximizeButton_ = new ImgButton(":/PlayerQt/Resources/bt_bigsize.png", this);
	restoreButton_ = new ImgButton(":/PlayerQt/Resources/bt_bigsize.png", this);
	closeButton_ = new ImgButton(":/PlayerQt/Resources/bt_close.png", this);

	connect(minimizeButton_, SIGNAL(clicked()), this, SLOT(onMinimize()));
	connect(maximizeButton_, SIGNAL(clicked()), this, SLOT(onMaximize()));
	connect(closeButton_, SIGNAL(clicked()), this, SLOT(onClose()));
	connect(restoreButton_, SIGNAL(clicked()), this, SLOT(onRestore()));

	state_ = Normal;
	stateBeforeMinimized_ = Normal;
	setButtonsVisibility();
}


QSize TitleBar::sizeHint() const
{
	return QSize(0, title_.height() + 5);
}

void TitleBar::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	p.drawPixmap(QPoint(0, 0), title_);
}

void TitleBar::mousePressEvent(QMouseEvent * event)
{
	if (state_ == Normal)
	{
		startDelta_ = mainWindow_->pos() - event->globalPos();
		isDragging_ = true;
	}
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (state_ == Normal)
	{
		onMaximize();
	}
	else if (state_ == Maximized)
	{
		onRestore();
	}
}

void TitleBar::mouseMoveEvent(QMouseEvent * event)
{
	if (isDragging_ && !mainWindow_->isResizing())
	{		
		mainWindow_->move(startDelta_ + event->globalPos());		
	}
}

void TitleBar::mouseReleaseEvent(QMouseEvent * event)
{
	isDragging_ = false;
}

void TitleBar::resizeEvent(QResizeEvent * event)
{
	int margin = 5;
	int y = height()/2 - closeButton_->sizeHint().height()/2;
	int x = width() - closeButton_->sizeHint().width() - margin;
	closeButton_->move(x, y);
	x -= maximizeButton_->sizeHint().width() + margin;
	maximizeButton_->move(x, y);
	restoreButton_->move(x, y);
	x -= minimizeButton_->sizeHint().width() + margin;
	minimizeButton_->move(x, y);	

	posBeforeMax_ = QRect(pos().x(), pos().y(), width(), height());
}

void TitleBar::showEvent(QShowEvent * event)
{
	if (state_ == Minimized)
		state_ = stateBeforeMinimized_;
		
	setButtonsVisibility();
}

void TitleBar::onMinimize()
{
	state_ = Minimized;	
	mainWindow_->showMinimized();
	stateBeforeMinimized_ = state_;	
	setButtonsVisibility();
}

void TitleBar::onMaximize()
{
	state_ = Maximized;
	mainWindow_->showMaximized();	
	setButtonsVisibility();
}

void TitleBar::onRestore()
{
	state_ = Normal;
	mainWindow_->showNormal();
	/*
	mainWindow_->move(posBeforeMax_.x(), posBeforeMax_.y());
	mainWindow_->resize(posBeforeMax_.width(), posBeforeMax_.height());*/	
	setButtonsVisibility();
}

void TitleBar::onClose()
{
	mainWindow_->close();
}

void TitleBar::setButtonsVisibility()
{
	switch(state_)
	{
	case Normal:
		maximizeButton_->setVisible(true);
		restoreButton_->setVisible(false);
		break;
	case Minimized:
		maximizeButton_->setVisible(false);
		restoreButton_->setVisible(true);
		break;
	case Maximized:
		maximizeButton_->setVisible(false);
		restoreButton_->setVisible(true);
		break;
	}
}

MainWindow::MainWindow(bool isStartFileOpen)
{	
	isStartFileOpen_ = isStartFileOpen;
	
	if(!isStartFileOpen_)
		setAcceptDrops(true);
	else
		setAcceptDrops(false);

	setWindowFlags(Qt::FramelessWindowHint); 
	setWindowTitle("");

	std::string homeDir = qStringToStdString(QDir::homePath());
	std::string contentDataSubDir = "Player4UX_contentData";

	decompressDir_ = stdStringToQString(homeDir + "/" + contentDataSubDir +"/");
	_chdir(homeDir.c_str());
	_mkdir(contentDataSubDir.c_str());

	initMediaEnv(winId());

	actionOpen_ = new QAction(this);
	actionOpen_->setObjectName(QString::fromUtf8("actionOpen"));
	actionExit_ = new QAction(this);
	actionExit_->setObjectName(QString::fromUtf8("actionExit"));
	actionPreviousPage_ = new QAction(this);
	actionPreviousPage_->setObjectName(QString::fromUtf8("actionPreviousPage"));
	actionNextPage_ = new QAction(this);
	actionNextPage_->setObjectName(QString::fromUtf8("actionNextPage"));

	actionOpen_->setText(tr("&Open"));
	actionExit_->setText(tr("E&xit"));

	//statusbar_ = new QStatusBar(this);
	//statusbar_->setObjectName(QString::fromUtf8("statusbar"));
	//setStatusBar(statusbar_);

	menu_File_ = new QMenu;
	if(!isStartFileOpen_)
		menu_File_->addAction(actionOpen_);
	menu_File_->addSeparator();
	menu_File_->addAction(actionExit_);

	connect(actionOpen_, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(actionExit_, SIGNAL(triggered()), this, SLOT(close()));
	connect(actionNextPage_, SIGNAL(triggered()), this, SLOT(onNextPage()));
	connect(actionPreviousPage_, SIGNAL(triggered()), this, SLOT(onPreviousPage()));

	menu_File_->addSeparator();
	for (int i = 0; i < sMaxRecentFiles; ++i)
	{
		recentFileActions_[i] = new QAction(this);
		recentFileActions_[i]->setVisible(false);	
		if(!isStartFileOpen_)
			menu_File_->insertAction(actionExit_, recentFileActions_[i]);
		connect(recentFileActions_[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}
	menu_File_->insertSeparator(actionExit_);

	contentWindow_ = new BgWindow(this);
	borderSize_ = 4;
	
	QHBoxLayout * menuBarLayout = new QHBoxLayout();
	menuBarLayout->setMargin(0);
	menuBarLayout->setSpacing(0);
	fileButton_ = new ImgButton(":/PlayerQt/Resources/bt_file.png");
	prevButton_ = new ImgButton(":/PlayerQt/Resources/bt_Previous.png");
	nextButton_ = new ImgButton(":/PlayerQt/Resources/bt_next.png");
	menuBarLayout->addWidget(fileButton_, 0);
	menuBarLayout->addWidget(new QWidget, 1);
	menuBarLayout->addWidget(prevButton_, 0);
	menuBarLayout->addWidget(nextButton_, 0);

	menuBar_ = new QWidget(this);
	menuBar_->setLayout(menuBarLayout);
		
	connect(fileButton_, SIGNAL(clicked()), this, SLOT(onMenu()));
	connect(prevButton_, SIGNAL(clicked()), this, SLOT(onPreviousPage()));
	connect(nextButton_, SIGNAL(clicked()), this, SLOT(onNextPage()));

	titleBar_ = new TitleBar(this);

	readSettings();
	updateRecentFilesActions();
	resizeMode_ = None;
	isResizing_ = false;

	timer_.start(60, this);

	newTargetRect_ = false;

	resizeCursorSet_ = false;
	setMouseTracking(true);

	topPixmap_ = QPixmap(":/PlayerQt/Resources/navi_bg.png");
	sideBorderPixmap_ = QPixmap(":/PlayerQt/Resources/height.png");
	bottomBorderPixmap_ = QPixmap(":/PlayerQt/Resources/bottom_line.png");
}

MainWindow::~MainWindow()
{
	uninitDocument();

	for (int i = 0; i < sMaxRecentFiles; ++i)
		delete recentFileActions_[i];

	delete menu_File_;
}

void MainWindow::uninitDocument()
{
	if (contentWindow_) {delete contentWindow_; contentWindow_ = NULL;}
	contentWindow_ = new BgWindow(this);		
	contentWindow_->show();
	resizeChildren();
}

void MainWindow::initDocument(const std::string & fileName)
{	
	int extraHeight = fileButton_->sizeHint().height() + titleBar_->sizeHint().height();

	lastOpenDir_ = stdStringToQString(getDirectory(fileName));
	uninitDocument();

	std::string ext = getFileExtension(fileName);

	ContentWindow * newWindow = NULL;

	if (boost::iequals(ext, "zip") || boost::iequals(ext, "st") || boost::iequals(ext, "tmp"));
	{
		newWindow = new STWindow(lastOpenDir_, decompressDir_, logDlg_);
	}
	/*
	else if (boost::iequals(ext, "epub"))
	{
		newWindow = new EpubWindow(lastOpenDir_, decompressDir_);
	}
	*/
	// newWindow = new STWindow(lastOpenDir_, decompressDir_, logDlg_);
	if (!newWindow) return;
	if (!newWindow->initDocument(fileName))
	{
		delete newWindow;
		return;
	}

	delete contentWindow_;
	contentWindow_ = newWindow;
	contentWindow_->setParent(this);
	contentWindow_->show();


	if (!isMaximized())
	{
		int origDocWidth, origDocHeight;
		if (contentWindow_->resizeToFitContent(&origDocWidth, &origDocHeight))
		{		
			resize(
				origDocWidth + 2*borderSize_, 
				origDocHeight + extraHeight + borderSize_);
		}			
	}

	resizeChildren();

	recentFiles_.removeAll(stdStringToQString(fileName));
	recentFiles_.prepend(stdStringToQString(fileName));
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
			recentFileActions_[i]->setVisible(false);
	}
}

void MainWindow::showMaximized()
{
	newTargetRect_ = false;
	QWidget::showMaximized();
}

void MainWindow::showNormal()
{
	newTargetRect_ = false;
	QWidget::showNormal();
}

void MainWindow::showMinimized()
{
	newTargetRect_ = false;
	QWidget::showMinimized();
}

void MainWindow::setLog(LogDlg * logDlg)
{
	logDlg_ = logDlg;
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
	resizeChildren();
}

void MainWindow::resizeChildren()
{
	titleBar_->move(0, 0);
	titleBar_->resize(width(), titleBar_->sizeHint().height());

	menuBar_->move(0, titleBar_->sizeHint().height());
	menuBar_->resize(width(), menuBar_->sizeHint().height());

	int contentTop = titleBar_->sizeHint().height() + menuBar_->sizeHint().height();
	if (titleBar_->windowState() == TitleBar::Maximized)
	{
		contentWindow_->move(0, contentTop); 
		contentWindow_->resize(width(), height() - contentTop);
	}
	else
	{
		contentWindow_->move(borderSize_, contentTop);
		contentWindow_->resize(width() - 2*borderSize_, height() - borderSize_ - contentTop);
	}
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	writeSettings();
	event->accept();
}


void MainWindow::timerEvent(QTimerEvent * event)
{		
	//printf("MainWindow::timerEvent\n");
	if (newTargetRect_)
	{
		setGeometry(targetRect_);		
		newTargetRect_ = false;
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData * data = event->mimeData();

	if (data->hasUrls())
		event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent * event)
{
	const QMimeData * data = event->mimeData();

	if(data->hasUrls())
	{
		QList<QUrl> urls = data->urls();
		BOOST_FOREACH(QUrl url, urls)
		{
			QString localFile = url.toLocalFile();
			if (checkFileExtension(qStringToStdString(localFile), "st"))
				initDocument(qStringToStdString(localFile));

			else if (checkFileExtension(qStringToStdString(localFile), "tmp"))
				initDocument(qStringToStdString(localFile));
		}
	}
}

void MainWindow::doResizeMode(int cx, int cy)
{
	resizeMode_ = None;
	if (isMaximized())
	{
		resizeMode_ = None;
	}
	else if (cx < 0 || cy < 0 || cx > width() || cy > height())
	{
		resizeMode_ = None;
	}
	else if (cx < borderSize_)	
	{
		if (cy < borderSize_ * 2)
			resizeMode_ = NW;
		else if (cy >= height() - borderSize_ * 2)
			resizeMode_ = SW;
		else
			resizeMode_ = W;
	}
	else if (cx > width() - borderSize_)
	{
		if (cy < borderSize_ * 2)
			resizeMode_ = NE;
		else if (cy > height() - borderSize_ * 2)
			resizeMode_ = SE;
		else
			resizeMode_ = E;
	}
	else if (cy < borderSize_)
	{
		if (cx < borderSize_ * 2)
			resizeMode_ = NW;
		else if (cx >= width() - borderSize_ * 2)
			resizeMode_ = NE;
		else
			resizeMode_ = N;
	}
	else if (cy > height() - borderSize_)
	{
		if (cx < borderSize_ * 2)
			resizeMode_ = SW;
		else if (cx >= width() - borderSize_ * 2)
			resizeMode_ = SE;
		else
			resizeMode_ = S;
	}

	switch(resizeMode_)
	{
	case NW: 
	case SE:
		setCursor(Qt::SizeFDiagCursor);
		resizeCursorSet_ = true;
		break;
	case W:
	case E:
		setCursor(Qt::SizeHorCursor);
		resizeCursorSet_ = true;
		break;
	case NE:
	case SW:
		setCursor(Qt::SizeBDiagCursor);
		resizeCursorSet_ = true;
		break;
	case N:
	case S:
		setCursor(Qt::SizeVerCursor);
		resizeCursorSet_ = true;
		break;
	default:
		if (resizeCursorSet_)
		{
			setCursor(Qt::ArrowCursor);
			resizeCursorSet_ = false;
		}

		break;
	}
}

bool MainWindow::eventFilter(QObject * watched, QEvent * event)
{
	if (event->type() == QEvent::Enter || 
		event->type() == QEvent::Leave)
	{		
		QPoint cursorPos = mapFromGlobal(QCursor::pos());
		doResizeMode(cursorPos.x(), cursorPos.y());		
	}


	else if (event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent * mouseEvent = (QMouseEvent *)event;
		if (mouseEvent->button() == Qt::LeftButton)
		{
			if (resizeMode_ != None) isResizing_ = true;

			initCursorPos_ = QCursor::pos();
			startRect_ = QRect(pos().x(), pos().y(), width(), height());
		}
	}
	else if (event->type() == QEvent::MouseMove)
	{
		if (isResizing_)
		{	
			int minWidth = 320;
			int minHeight = 200;
			int w, h;
			QRect newRect = startRect_;
			qDebug() << resizeMode_;
			switch(resizeMode_)
			{
			case N:				
				newRect.setY(QCursor::pos().y() - initCursorPos_.y() + startRect_.y());
				if (newRect.height() < minHeight) newRect.setY(startRect_.bottom() - minHeight);
				break;
			case NW:
				newRect.setX(QCursor::pos().x() - initCursorPos_.x() + startRect_.x());
				newRect.setY(QCursor::pos().y() - initCursorPos_.y() + startRect_.y());		

				if (newRect.height() < minHeight) newRect.setY(startRect_.bottom() - minHeight);
				if (newRect.width() < minWidth)	newRect.setX(startRect_.right() - minWidth);

				break;
			case NE:
				newRect.setWidth(QCursor::pos().x() - initCursorPos_.x() + startRect_.width());
				newRect.setY(QCursor::pos().y() - initCursorPos_.y() + startRect_.y());

				if (newRect.height() < minHeight) newRect.setY(startRect_.bottom() - minHeight);				
				if (newRect.width() < minWidth)	newRect.setRight(startRect_.left() + minWidth);					
				break;
			case W:
				newRect.setX(QCursor::pos().x() - initCursorPos_.x() + startRect_.x());
				if (newRect.width() < minWidth)	newRect.setX(startRect_.right() - minWidth);
				break;
			case E:
				newRect.setWidth(QCursor::pos().x() - initCursorPos_.x() + startRect_.width());
				if (newRect.width() < minWidth)	newRect.setRight(startRect_.left() + minWidth);
				break;
			case S:
				newRect.setHeight(QCursor::pos().y() - initCursorPos_.y() + startRect_.height());
				if (newRect.height() < minHeight) newRect.setBottom(startRect_.top() + minHeight);
				break;
			case SW:
				newRect.setX(QCursor::pos().x() - initCursorPos_.x() + startRect_.x());
				newRect.setHeight(QCursor::pos().y() - initCursorPos_.y() + startRect_.height());
				if (newRect.width() < minWidth)	newRect.setX(startRect_.right() - minWidth);
				if (newRect.height() < minHeight) newRect.setBottom(startRect_.top() + minHeight);
				break;
			case SE:
				newRect.setWidth(QCursor::pos().x() - initCursorPos_.x() + startRect_.width());
				newRect.setHeight(QCursor::pos().y() - initCursorPos_.y() + startRect_.height());
				if (newRect.width() < minWidth)	newRect.setRight(startRect_.left() + minWidth);
				if (newRect.height() < minHeight) newRect.setBottom(startRect_.top() + minHeight);
				break;
			}


			if (newRect != targetRect_)
			{
				newTargetRect_ = true;
				targetRect_ = newRect;				
			}
		}
		else
		{
			QPoint cursorPos = mapFromGlobal(QCursor::pos());
			doResizeMode(cursorPos.x(), cursorPos.y());		
		}

	}
	else if (event->type() == QEvent::MouseButtonRelease)
	{
		isResizing_ = false;
	}



	return false;
}

void MainWindow::onOpen()
{
	if (!lastOpenDir_.isEmpty())
		_chdir(qStringToStdString(lastOpenDir_).c_str());
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), 
		"", tr("Content files (*.zip *.st)"), 0, 0);

	if (!fileName.isEmpty()) 
	{
		initDocument(qStringToStdString(fileName));
	}	
}

void MainWindow::onPreviousPage()
{
	if (contentWindow_)
		contentWindow_->onPreviousPage();
}


void MainWindow::onNextPage()
{
	if (contentWindow_)
		contentWindow_->onNextPage();
}

void MainWindow::onMenu()
{
	QPoint pt(0, titleBar_->sizeHint().height());
	//pt.setY(pt.y() + fileButton_->size().height());
	menu_File_->popup(mapToGlobal(pt));
}


void MainWindow::openRecentFile()
{
	QAction * action = qobject_cast<QAction *>(sender());
	if (action)
		initDocument(qStringToStdString(action->data().toString()));

}

void MainWindow::readSettings()
{
	QSettings settings("Net&TV", "Player");
	recentFiles_ = settings.value("recentFiles").toStringList();
	lastOpenDir_ = settings.value("lastOpenDir").toString();
	
	QRect lastGeo;
	if(settings.value("windowGeometry").isNull()) lastGeo = QRect(0, 0, 768, 1024);
	else lastGeo = settings.value("windowGeometry").toRect();
	setGeometry(lastGeo);

	/*
	QRect logRect;
	if(settings.value("logGeometry").isNull()) logRect = QRect(0, 0, 640, 480);
	else logRect = settings.value("logGeometry").toRect();
	logDlg_->setGeometry(logRect.x(), logRect.y(), logRect.width(), logRect.height());
	*/
}

void MainWindow::writeSettings()
{
	QSettings settings("Net&TV", "Player");

	QRect geo = geometry();
	settings.setValue("windowGeometry", geo);
//	settings.setValue("logGeometry", logDlg_->geometry());
	settings.setValue("recentFiles", recentFiles_);
	settings.setValue("lastOpenDir", lastOpenDir_);
}

void MainWindow::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	p.drawPixmap(0, 0, width(), topPixmap_.height(), topPixmap_);

	int bottomHeight = height() - topPixmap_.height();
	p.drawPixmap(0, topPixmap_.height(),
		borderSize_, bottomHeight - borderSize_, sideBorderPixmap_);
	p.drawPixmap(width() - borderSize_, 
		topPixmap_.height(), borderSize_, 
		bottomHeight - borderSize_, sideBorderPixmap_);

	p.drawPixmap(
		0, height() - borderSize_, 
		width(), borderSize_,
		bottomBorderPixmap_);
}
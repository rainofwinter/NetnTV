#include "stdafx.h"
#include "STWindow.h"
#include "Document.h"
#include "PcAudioPlayer.h"
#include "PcVideoPlayer.h"
#include "PcPlayerSupport.h"
#include "PcScene.h"
#include "GfxRenderer.h"
#include "Utils.h"
#include "FileUtils.h"
#include "TextEditObject.h"
#include "TextSelectUI.h"
#include "LogDlg.h"
#include "MagazineDocumentTemplate.h"
#include "SceneChangerDocumentTemplate.h"
// #include <direct.h>

using namespace std;



STWindow::STWindow(const QString & lastOpenDir, const QString & decompressDir, LogDlg * logDlg)
{
	lastOpenDir_ = lastOpenDir;
	decompressDir_ = decompressDir;

	logDlg_ = logDlg;

	pcScene_ = 0;
	playerSupport_ = 0;
	videoPlayer_ = 0;
	audioPlayer_ = 0;
	document_ = 0;
	
	setStyleSheet("background-color: #555555;"); 	

	viewport_ = new QGLWidget;
	graphicsView_ = new GraphicsView(this);	
	graphicsView_->setStyleSheet("QGraphicsView { border-style: none; }"); 
	graphicsView_->setViewport(viewport_);
	graphicsView_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	viewport_->makeCurrent();
	graphicsView_->hide();

	fitToDocumentSize_ = true;
	preserveAspect_ = true;
}

STWindow::~STWindow()
{	
	uninitDocument();
}

bool STWindow::resizeToFitContent(int * width, int * height) const
{
	*width = origDocWidth_;
	*height = origDocHeight_;
	return true;
}

void STWindow::timerEvent(QTimerEvent * event)
{		
	if (((QWidget *)(this->parent()))->windowState() == Qt::WindowMinimized)
		return;

	//printf("STWindow::timerEvent\n");
	if (pcScene_->update())
	{
		viewport_->update();
	}
}

void STWindow::closeEvent(QCloseEvent * event)
{
	event->accept();
}



void STWindow::onNextPage()
{
	if (!document_) return;		
	MagazineDocumentTemplate * templ = 
		dynamic_cast<MagazineDocumentTemplate *>(document_->documentTemplate().get());

	if (templ)
	{
		if(templ->curScene()->isLoaded())
		{
			templ->sceneChangeTo(templ->pageRight());
			document_->triggerRedraw();
		}
	}
	else
	{
		SceneChangerDocumentTemplate * templ = 
			dynamic_cast<SceneChangerDocumentTemplate *>(document_->documentTemplate().get());

		if (templ)
		{			
			if(templ->curScene()->isLoaded())
			{
				templ->sceneChangeTo(templ->pageRight());
				document_->triggerRedraw();			
			}
		}
	}
}

void STWindow::onPreviousPage()
{
	if (!document_) return;
	MagazineDocumentTemplate * templ = 
		dynamic_cast<MagazineDocumentTemplate *>(document_->documentTemplate().get());

	if (templ)
	{
		if(templ->curScene()->isLoaded())
		{
			templ->sceneChangeTo(templ->pageLeft());
			document_->triggerRedraw();
		}
	}
	else
	{
		SceneChangerDocumentTemplate * templ = 
			dynamic_cast<SceneChangerDocumentTemplate *>(document_->documentTemplate().get());

		if (templ)
		{	
			if(templ->curScene()->isLoaded())
			{
				templ->sceneChangeTo(templ->pageLeft());
				document_->triggerRedraw();			
			}
		}
	}
}


void STWindow::uninitDocument()
{
	if (document_)
	{
		
		delete videoPlayer_;
		delete audioPlayer_;
		delete playerSupport_;

		delete document_;
		document_ = 0;

		Global::uninit();

		delete textui_;
		delete pcScene_;

		delete contextMenu_;

		timer_.stop();

		deleteDirFiles(decompressDir_, false);

		graphicsView_->hide();
	}
}


bool STWindow::initDocument(const std::string & fileName)
{
	lastOpenDir_ = stdStringToQString(getDirectory(fileName));
	uninitDocument();
	
	std::string ext = getFileExtension(fileName);

	bool ret = false;
	
	if (boost::iequals(ext, "zip"))
	{
		ret = initZipDocument(fileName);
	}
	else if (boost::iequals(ext, "st") || boost::iequals(ext, "tmp"))
	{
		ret = initStDocument(fileName);
	}
	
	// ret = initStDocument(fileName);
	if (!ret) return false;

	origDocWidth_ = document_->width();
	origDocHeight_ = document_->height();

	videoPlayer_ = new PcVideoPlayer(document_);
	audioPlayer_ = new PcAudioPlayer(document_);
	playerSupport_ = new PcPlayerSupport;	
	playerSupport_->setLog(logDlg_);
	playerSupport_->setSTWindow(this);
	Global::init(videoPlayer_, audioPlayer_, playerSupport_);

	pcScene_ = new PcScene(viewport_, document_, videoPlayer_, audioPlayer_, playerSupport_);
	pcScene_->init();
	
	lineEdit_ = new QLineEdit();
	lineEdit_->setVisible(false);
	QVBoxLayout * layout = new QVBoxLayout;
	layout->addWidget(lineEdit_);
	setLayout(layout);

	textui_ = new TextSelectUI(this);

	contextMenu_ = new QMenu(this);
	memosAction_ = new QAction(tr("Memos..."), this);
	connect(memosAction_, SIGNAL(triggered()), this, SLOT(onMemos()));
	contextMenu_->addAction(memosAction_);
	searchAction_ = new QAction(tr("Search..."), this);
	connect(searchAction_, SIGNAL(triggered()), this, SLOT(onSearch()));
	contextMenu_->addAction(searchAction_);

	graphicsView_->setScene(pcScene_);
	graphicsView_->show();
	
	resize(document_->width(), document_->height());

	timer_.start(0, this);	

	return true;
}

bool STWindow::initZipDocument(const std::string & fileName)
{
	if (!decompress(decompressDir_, stdStringToQString(fileName)))
		return false;
	
	try
	{	
		std::string docFileName = 
			qStringToStdString(decompressDir_) + "document.st";
		Reader(docFileName.c_str()).read(document_);
		string directory = getDirectory(docFileName);
		chdir(directory.c_str());
		Global::instance().setDirectories(directory, directory, document_->remoteReadServer());
		document_->readTextFeatures(directory + "__textFeatures.xml");
	}
	catch(const Exception & e)
	{		
		QString msg;
		msg.sprintf("Could not open file: %s", e.what());
		QMessageBox::information(0, QObject::tr("Error"), msg);
		return false;
	}
	catch(...)
	{
		QMessageBox::information(0, QObject::tr("Error"), QObject::tr("Could not open file"));
		return false;
	}

	return true;
}

bool STWindow::initStDocument(const std::string & fileName)
{
	try
	{
		std::string docFileName = fileName;
		Reader(docFileName.c_str()).read(document_);
		string directory = getDirectory(docFileName);
		chdir(directory.c_str());
		Global::instance().setDirectories(directory, directory, document_->remoteReadServer());
		document_->readTextFeatures(directory + "__textFeatures.xml");
	}
	catch(const Exception & e)
	{		
		QString msg;
		msg.sprintf("Could not open file: %s", e.what());
		QMessageBox::information(0, QObject::tr("Error"), msg);
		return false;
	}
	catch(...)
	{
		QMessageBox::information(0, QObject::tr("Error"), QObject::tr("Could not open file"));
		return false;
	}

	return true;
}

void STWindow::setPreserveAspect(bool preserveAspect)
{
	preserveAspect_ = preserveAspect;
	doResize(width(), height());
}

void STWindow::resizeEvent(QResizeEvent * event)
{
	doResize(event->size().width(), event->size().height());
}

void STWindow::doResize(int width, int height)
{
	int sw = width;
	int sh = height;
	int sx = 0;
	int sy = 0;

	if (preserveAspect_)
	{
		float aspect = (float)origDocWidth_ / origDocHeight_;
		if ((float)width / height > aspect)
		{		
			sw = roundFloat(height * aspect);
			sh = height;		
		}
		else
		{
			sw = width;
			sh = roundFloat(width / aspect);
		}

		sx = (width - sw)/2;
		sy = (height - sh)/2;
	}
	graphicsView_->resize(QSize(sw, sh));	
	graphicsView_->move(sx, sy);
	pcScene_->resize(sw, sh);
}

void STWindow::paintEvent(QPaintEvent * event)
{
	QStyleOption o;
	o.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}

void STWindow::keyPressEvent(QKeyEvent * event)
{
	if (isFocusTextEdit_)
	{
		if (event->key() == Qt::Key_Return)
		{
			playerSupport_->setTextEditObjTextString(lineEdit_->text().toStdWString(), true);
			isFocusTextEdit_ = false;
			lineEdit_->clearFocus();
			lineEdit_->setVisible(false);
		}
	}
}

void STWindow::setIsFocusTextEdit(bool var)
{
	lineEdit_->setVisible(var);
	playerSupport_->setTextEditObjTextString(lineEdit_->text().toStdWString(), false);
	lineEdit_->setText(QString(""));
	if (var)
	{
		lineEdit_->move(playerSupport_->handledObj()->posX(), playerSupport_->handledObj()->posY());
		lineEdit_->resize(playerSupport_->handledObj()->width(), playerSupport_->handledObj()->height());
		lineEdit_->setText(QString::fromStdWString(playerSupport_->handledObj()->textString()));
		lineEdit_->setFocus();
	}
	else 
	{
		lineEdit_->clearFocus();
	}
	isFocusTextEdit_ = var;
}

void STWindow::selectedTextPart()
{
	if (pcScene_->isShowAppLayer())
	{
		Vector2 pos = document_->textFeatures()->selectedTextPos();		

		QPoint qpos = QPoint(
			pos.x, 
			pos.y);	

		textui_->showUi(qpos);
	}	
}

void STWindow::clearTextSelect()
{
	textui_->hideUi();	
}

void STWindow::setHighlight()
{
	QColorDialog  colorDlg(this);
	QColor qc = colorDlg.getColor();
	document_->textFeatures()->addHighlight(Color(qc.red() / 255.0,qc.green() / 255.0,qc.blue() / 255.0,qc.alpha() / 255.0));	
}

void STWindow::memoClicked(Memo * memo)
{
	textui_->memoClicked(memo);
}

void STWindow::showAppLayer(bool show)
{
	if (show)
	{
		TextFeatures * tf = document_->textFeatures();
		Vector2 pos = tf->selectedTextPos();		

		QPoint qpos = QPoint(pos.x, pos.y);	

		int startIndex, endIndex;
		SceneObject * textObj = tf->selTextObj(&startIndex, &endIndex);

		if (textObj)
		{
			textui_->showUi(qpos);
		}

	}
	else
	{
		textui_->hideUi();
	}
}
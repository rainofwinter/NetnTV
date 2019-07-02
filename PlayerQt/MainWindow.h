#pragma once
#include <QGLWidget>
#include "ContentWindow.h"
class MainWindow;

class BgWindow : public ContentWindow
{
	Q_OBJECT
public:
	BgWindow(MainWindow * mainWindow);
	bool initDocument(const std::string & fileName);
	void uninitDocument();

	bool resizeToFitContent(int * width, int * height) const;

	void setPreserveAspect(bool preserveAspect);
	virtual void paintEvent(QPaintEvent * event);
public slots:
	void onNextPage();
	void onPreviousPage();
};

///////////////////////////////////////////////////////////////////////////////
class ImgButton;
class MainWindow;
class ContentWindow;
class LogDlg;

class TitleBar : public QWidget
{
	Q_OBJECT
public:
	enum State
	{
		Normal,
		Minimized,
		Maximized
	};

public:

	State windowState() const {return state_;}


	TitleBar(MainWindow * mainWindow);
	virtual QSize sizeHint() const;
	virtual void paintEvent(QPaintEvent * event);

	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseDoubleClickEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void resizeEvent(QResizeEvent * event);
	virtual void showEvent(QShowEvent * event);
	private slots:
		void onMinimize();
		void onMaximize();
		void onRestore();
		void onClose();

private:
	void setButtonsVisibility();

private:
	QPixmap title_;
	MainWindow * mainWindow_;

	bool isDragging_;
	QPoint startDelta_;
	QPoint mainWindowPos_;

	ImgButton * minimizeButton_;
	ImgButton * maximizeButton_;
	ImgButton * restoreButton_; 
	ImgButton * closeButton_;

	QRect posBeforeMax_;

	State state_;
	State stateBeforeMinimized_;

};

///////////////////////////////////////////////////////////////////////////////

class GLWidget;
class Document;
class PcVideoPlayer;
class PcAudioPlayer;
class PcPlayerSupport;
class PcScene;
class LogDlg;
class TextSelectUI;
struct Memo;
///////////////////////////////////////////////////////////////////////////////
/*
class GraphicsView : public QGraphicsView
{
public:
	GraphicsView(QWidget * parent) : QGraphicsView(parent) {}
private:
	
 void resizeEvent(QResizeEvent *event) {
     if (scene())
         scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
     QGraphicsView::resizeEvent(event);
 }

};
*/
///////////////////////////////////////////////////////////////////////////////

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	enum ResizeMode
	{
		NW,
		N,
		NE,
		E,
		SE,
		S,
		SW,
		W,
		None
	};

public:
	MainWindow(bool isStartFileOpen);
	~MainWindow();

	void initDocument(const std::string & fileName);
	void uninitDocument();

	void showMaximized();
	void showNormal();
	void showMinimized();
	bool isResizing() const { return isResizing_; }

	int borderSize() const { return borderSize_; }

	void setLog(LogDlg * logDlg);

private slots:
	void onOpen();
	void onNextPage();
	void onPreviousPage();
	void onMenu();
	void openRecentFile();

private:
	virtual void resizeEvent(QResizeEvent * event);
	virtual void closeEvent(QCloseEvent * event);
	virtual bool eventFilter(QObject *, QEvent *);
	virtual void timerEvent(QTimerEvent * event);
	virtual void paintEvent(QPaintEvent * event);
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *);

	void updateRecentFilesActions();
	void readSettings();
	void writeSettings();
	void showFileMenu();

	void doResizeMode(int cx, int cy);
	void resizeChildren();
private:

	ResizeMode resizeMode_;
	bool isResizing_;
	QPoint initCursorPos_;
	QRect startRect_;
	bool newTargetRect_;
	QRect targetRect_;

	QWidget * menuBar_;
	QWidget * contentWidget_;
	ContentWindow * contentWindow_;
	
	float origDocWidth_, origDocHeight_;

	QString decompressDir_;

	static const int sMaxRecentFiles = 5;
	QAction * recentFileActions_[sMaxRecentFiles];
	QStringList recentFiles_;
	QString lastOpenDir_;

	QMenu * menu_File_;
	QAction *actionOpen_;
	QAction *actionExit_;
	QAction *actionPreviousPage_;
	QAction *actionNextPage_;
	QStatusBar *statusbar_;

	ImgButton * fileButton_, *prevButton_, *nextButton_;
	TitleBar * titleBar_;

	int borderSize_;

	QBasicTimer timer_;
	bool resizeCursorSet_;

	QPixmap topPixmap_;
	QPixmap sideBorderPixmap_;
	QPixmap bottomBorderPixmap_;
	LogDlg * logDlg_;

	bool isStartFileOpen_;

};
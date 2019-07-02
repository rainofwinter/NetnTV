#pragma once
#include <QGLWidget>
#include "ContentWindow.h"

class GLWidget;
class Document;
class PcVideoPlayer;
class PcAudioPlayer;
class PcPlayerSupport;
class PcScene;
class TextSelectUI;

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

///////////////////////////////////////////////////////////////////////////////
class LogDlg;

class STWindow : public ContentWindow
{
	Q_OBJECT
public:
	STWindow(const QString & lastOpenDir, const QString & decompressDir, LogDlg * logDlg);
	~STWindow();
	bool initDocument(const std::string & fileName);
	void uninitDocument();

	Document * document() const {return document_;}

	bool resizeToFitContent(int * width, int * height) const;

	void setPreserveAspect(bool preserveAspect);
	void doResize(int width, int height);
	void setIsFocusTextEdit(bool var);

	void selectedTextPart();
	void clearTextSelect();
	void setHighlight();

	void memoClicked(Memo * memo);

	QGraphicsView * graphicsView() const {return graphicsView_;}

	PcScene * scene() const {return pcScene_;}
	void showAppLayer(bool show);

public slots:
	void onNextPage();
	void onPreviousPage();

private:

	bool initZipDocument(const std::string & fileName);
	bool initStDocument(const std::string & fileName);

	virtual void resizeEvent(QResizeEvent * event);	
	virtual void timerEvent(QTimerEvent * event);
	virtual void closeEvent(QCloseEvent * event);
	virtual void STWindow::paintEvent(QPaintEvent * event);
	virtual void keyPressEvent(QKeyEvent * event);

private:

	bool fitToDocumentSize_;

	Document * document_;

	QWidget * centralWidget_;
	QGLWidget * viewport_;
	QBasicTimer timer_;
	QGraphicsView * graphicsView_;
	PcScene * pcScene_;

	PcPlayerSupport * playerSupport_;
	PcVideoPlayer * videoPlayer_;
	PcAudioPlayer * audioPlayer_;

	QString decompressDir_;
	QString lastOpenDir_;

	bool preserveAspect_;
	int origDocWidth_, origDocHeight_;

	LogDlg * logDlg_;

	bool isFocusTextEdit_;
	QLineEdit * lineEdit_;
	
	TextSelectUI * textui_;
	QMenu * contextMenu_;
	QAction * memosAction_, * searchAction_;

	bool resized_;
};
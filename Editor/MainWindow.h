#pragma once

#include <QtGui/QMainWindow>
#include "ui_main.h"


//////////////////////////////////////////////////////////////////////////////
class MainWindow;

class MdiArea : public QMdiArea
{
	Q_OBJECT
public:
	MdiArea(MainWindow * parent = 0);
private:
	MainWindow * parent_;
	QTabBar * tabBar_;
};

///////////////////////////////////////////////////////////////////////////////
class GLWidget;
class GLPreview;
class PropertyPane;
class AnimationPane;
class SceneObjectListPane;
class AppObjectListPane;
class EventActionPane;
class SceneListPane;
class EditorDocument;
class LoginDlg;
class ChangePasswordDlg;

class MainWindow : public QMainWindow
{
	friend class GLWidget;
	friend class MdiArea;

	Q_OBJECT
public:
	enum ObjectMode
	{
		ModeSceneObject,
		ModeAppObject
	};

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

	void setDocument(EditorDocument * document);
	void setView(GLWidget * widget);

	void setObjectMode(ObjectMode mode);
	ObjectMode objectMode() const {return objectMode_;}
	
	void splashShow(bool isSplahShow){ isSplashShow_ = isSplahShow; }

	/**
	@return what object mode (App / Scene) does the current tool correspond to?
	*/
	ObjectMode toolObjectMode() const;

	GLWidget * curGlWidget() const;
	void openFile(const QString & fileName);

protected:
	virtual void keyPressEvent(QKeyEvent * keyEvent);
	virtual void keyReleaseEvent(QKeyEvent * keyEvent);
	virtual void dragEnterEvent(QDragEnterEvent * event);
	virtual void dropEvent(QDropEvent * event);
	virtual void closeEvent(QCloseEvent * event);
	bool eventFilter(QObject * obj, QEvent * event);
signals:
	void documentActivated(QWidget *);
	void documentDeactivated(QWidget *);
	void documentChanged(EditorDocument *);

public slots:
	bool closeFile();
	void onToolChanged();

protected slots:
	void newFile();
	void openRecentFile();
	void openFile();
	void saveFile();
	void saveAsFile();

	void onPublish();
	void onUndo();
	void onRedo();
	void onPreview();
	void onPlayerError(QProcess::ProcessError);
	void onSelect();
	void onTranslate();
	void onRotate();
	void onScale();
	void onPivot();
	void onAppObjectTransform();
	void onImportPDF();
	void onImportFolder();
	void onImportGif();
	void onImportXML();
	void onExportXML();
	void onDelete();
	void onCopy();
	void onPaste();

	void onNudgePlusX();
	void onNudgeMinusX();
	void onNudgePlusY();
	void onNudgeMinusY();
	void onNudgeTenPlusX();
	void onNudgeTenMinusX();
	void onNudgeTenPlusY();
	void onNudgeTenMinusY();
	
	void onPreferences();
	void onDocumentProperties();
	void onDocTemplateProperties();
	void onPageTrackingUrls();

	void onAbout();

	void onSubWindowActivating(QMdiSubWindow *);

	void onObjectSelectionChanged();
	void onAppObjectSelectionChanged();
	void onModeChanged(int index);

	void onCommandHistoryChanged();
	void onSetKey();

	void subWindowClosing(QWidget *);

	// import file
	void importPdf(const QString & fileName);
	void importGif(const QString & fileName);

	void onLogin();
	void onLogout();
	void onMenuLogout();
	void onChangePassword();

	void onImportPDFtemprary();
private:
	virtual void timerEvent(QTimerEvent *);

	void setPanesDocument(EditorDocument * document);	
	void doRecentFiles(const QString & fileName);
	void updateRecentFilesActions();
	void updateActions();
	void updateStatusBar(QString statusMessage, int x, int y);
	void readSettings();
	void convertPng2Jpg(const QDir & dir);
	bool handleAlreadyOpen(const QString & fileName);

private:
	Ui::MainWindowClass ui;

	static const int sMaxRecentFiles = 5;
	QAction * recentFileActions_[sMaxRecentFiles];
	QStringList recentFiles_;

	PropertyPane * propertyPane_;
	AnimationPane * animationPane_;
	SceneObjectListPane * sceneObjectListPane_;
	AppObjectListPane *appObjectListPane_;
	EventActionPane * eventActionPane_;
	SceneListPane * sceneListPane_;
	ObjectMode objectMode_;
	
	QBasicTimer timer_;

	QMdiArea * mdiArea_;
	QMdiSubWindow * curMdiSubWindow_;

	struct SubWindowData
	{
		SubWindowData(QMdiSubWindow * subWindow, GLWidget * glWidget, const QString & fileName)
		{
			this->subWindow = subWindow;
			this->glWidget = glWidget;
			this->fileName = fileName;
		}
		QMdiSubWindow * subWindow;
		GLWidget * glWidget;
		QString fileName;
	};
	std::vector<SubWindowData> subWindowData_;

	EditorDocument * document_;

	QStatusBar * statusBar_;

	QComboBox * modeSelectorComboBox_;
	bool ignoreModeChanged_;

	struct PlayerProcessData
	{		
		~PlayerProcessData()
		{
			process.kill();
			QFile::remove(fileName);
		}
		QProcess process;
		QString fileName;
	};

	boost::scoped_ptr<PlayerProcessData> playerProcess_;

	QRect lastNormalGeo_;

	QLabel * statusMessage_;
	QLabel * mousePosition_;

	bool isCtrlKey_;

	LoginDlg * loginDlg_;
	bool isLogin_;
	bool isLogout_;
	bool isClosingLogout_;
	bool isSplashShow_;
	bool isMenuLogout_;
};
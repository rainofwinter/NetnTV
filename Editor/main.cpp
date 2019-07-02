#include "stdafx.h"
#include "MainWindow.h"
#include "ImageReader.h"
#include "VideoPlayer.h"
#include "AudioPlayer.h"
#include "Global.h"
#include <QtGui/QApplication>
#include <QGLWidget>
#include "Utils.h"
#include "Activation.h"
#include "FileUtils.h"
#include "LoginDlg.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
class NullPlayerSupport : public PlayerSupport
{
public:

	NullPlayerSupport();

	virtual void openUrl(const std::string & str, bool isMobile) {}

	virtual float currentTime();

	virtual void receiveMessage(const std::string & message) {}

	virtual void initAppObject(AppObject * appObject) {}
	virtual void uninitAppObject(AppObject * appObject) {}
	virtual void sendAppObjectToFront(AppObject * appObject) {}

	virtual void setPreserveAspectRatio(bool preserveAspectRatio) {}

	virtual void launchApp(const std::string & params) {}

	virtual void logMessage(const char * message) {
		//TODO TODO Implement a proper logging solution
		cout << message << "\n";
	}

	virtual unsigned int totalDiskSpace() const
	{
		ULARGE_INTEGER freeBytes, totalBytes, totalFreeBytes;
		GetDiskFreeSpaceEx(0, &freeBytes, &totalBytes, &totalFreeBytes);


		return (unsigned int)(totalBytes.QuadPart / 1024 / 1024);
	}

	virtual unsigned int freeDiskSpace() const
	{
		ULARGE_INTEGER freeBytes, totalBytes, totalFreeBytes;
		GetDiskFreeSpaceEx(0, &freeBytes, &totalBytes, &totalFreeBytes);

		return (unsigned int)(freeBytes.QuadPart / 1024 / 1024);
	}

	void msgBox(const char * msg)
	{
		//ModelessDlg(msg).show();
		QMessageBox::information(0, QObject::tr("Error"), msg);
	}

	virtual void showAppLayer(bool show) {}

	virtual void handledTextEditObj(TextEditObject * handledObj) {}
	virtual void focusOutTextEditObj() {}

	virtual void selectedTextPart() {}
	virtual const std::string getOs() { return "not found"; }
	virtual const std::string getModelName() { return "not found"; }
	virtual const std::string getOsVersion() { return "not found"; }

	virtual void shareMsg(int type, std::string msg) {}
	virtual void vibrateAction(int time) {}

private:
	UINT64 ticks, ticksAtStart, ticksPerSecond;
};

NullPlayerSupport::NullPlayerSupport()
{
	QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond);
	QueryPerformanceCounter((LARGE_INTEGER *)&ticksAtStart);
}

float NullPlayerSupport::currentTime()
{				
	QueryPerformanceCounter((LARGE_INTEGER *)&ticks);
	return (float)(ticks - ticksAtStart) / (ticksPerSecond);
}



class NullPlayer : public VideoPlayer
{
public:
	virtual void activate(int, const std::string & fileName, bool, bool, float) {}

	virtual void activateRegion(int, const std::string & fileName, 
		int x, int y, int width, int height, bool, bool, float) {}

	virtual void setRegion(int x, int y, int w, int h){}

	virtual void deactivate() {}

	virtual float currentTime() const {return 0.0f;}
};

class NullAudioPlayer : public AudioPlayer
{
public:
	virtual void play(const std::string & source, float startTime) {}
	virtual void stopAll() {}
	virtual void resumeAll() {}
	virtual void pauseAll() {}
	virtual void play(int id, const std::string & source, bool repeat, float startTime) {}
	virtual void stop(int id) {}
	virtual void pause(int id) {}
	virtual void resume(int id) {}
	virtual float currentTime(int id) {return 0.0f;}	
	virtual void setVolume(int id, float val) {}
	virtual void setAllVolume(float val) {}

	virtual void startRecord() {}
	virtual void playRecorded(int id, bool repeat) {}
	virtual void endRecord() {}
	virtual float curRecordVolume() {return 0.0f;}
};


///////////////////////////////////////////////////////////////////////////////

class SplashScreen : public QSplashScreen
{
public:
	SplashScreen(const QPixmap & pixmap) : QSplashScreen(pixmap)
	{
		setMask(pixmap.mask());
	}
};

///////////////////////////////////////////////////////////////////////////////

class CheckReqWin : public QGLWidget
{
public:
	CheckReqWin() : QGLWidget(new QGLContext(QGLFormat(/*QGL::SampleBuffers*/)))
	{
	}
	
	bool checkReq(std::string * msg)
	{
		*msg = msg_;
		return meets_;
	}

private:
	virtual void initializeGL()
	{		
		glewInit();
		GLenum err = glewInit();

		//glBindTexture
		//glClear
		//glDepthFunc
		//glDisable
		//glGetInteger
		//glReadPixels
		//glTexImage2D
		bool v11 = GLEW_VERSION_1_1;

		//glGenBuffers
		//glBindBuffer
		//glBufferData
		//glBufferSubData
		//glDeleteBuffers
		bool v15 = GLEW_VERSION_1_5;
		
		//shader related stuff
		//glEnableVertexAttribArray
		//glDisableVertexAttribArray
		bool v20 = GLEW_VERSION_2_0;
		
		//frame buffer stuff
		bool frameBuffer = GLEW_ARB_framebuffer_object;

		//frame buffer ext. Sometimes when ARB is unavailable, EXT is
		bool frameBufferEXT = GLEW_EXT_framebuffer_object;

		stringstream ss;
		
		if (!v11)
			ss << "No support for GL 1.1 functionality\n";
		if (!v15)
			ss << "Does not support GL 1.5 functionality\n";
		if (!v20)
			ss << "Does not support GL 2.0 functionality\n";
		if (!frameBufferEXT)
			ss << "Does not support frame buffer objects\n";

		msg_ = ss.str();

		meets_ = msg_.empty();
		if (meets_)
		{		
			ss << L"Meets GL requirements";	
			msg_ = ss.str();
		}

	}
	std::string msg_;
	bool meets_;

};

///////////////////////////////////////////////////////////////////////////////
#include <default.h>
#include <hex.h>
#include <files.h>
#include <gzip.h>
using namespace CryptoPP;

#include "MainWindow.h"

/*
#include "podofo/podofo.h"
using namespace PoDoFo;
*/
int main(int argc, char *argv[])
{
	QSharedMemory shared("Studio4UX_Unique_Key");
	if(!shared.create(512, QSharedMemory::ReadWrite)) return 0;

	QApplication a(argc, argv);	

	//std::string dir = *argv;
	
	//Check system requirements
	CheckReqWin * cw = new CheckReqWin;
	cw->resize(0, 0);
	cw->show();
	std::string msg;
	bool meets = cw->checkReq(&msg);
	delete cw;

	if (!meets)
	{
		QMessageBox::information(0, QObject::tr("Error"), stdStringToQString(msg));
		return -1;
	}

#ifdef _DEBUG
	AllocConsole();
	freopen("CONOUT$", "wb", stdout);	
#endif
	

	/*
	PdfMemDocument * pdfDoc = new PdfMemDocument("c:\\st\\princess.pdf");


	int numPages = pdfDoc->GetPageCount();

	const PdfVecObjects & pdfVecObjects = pdfDoc->GetObjects();

	int numPdfObjects = pdfVecObjects.GetSize();


	for (int i = 0; i < numPages; ++i)
	{
		PdfPage * page = pdfDoc->GetPage(i);

		page->GetObject();
		PdfContentsTokenizer tokenizer(page);
		
	}

	delete pdfDoc;
	*/

	//MagickCoreGenesis(argv[0], MagickFalse);

	NullPlayer videoPlayer;
	NullAudioPlayer audioPlayer;
	NullPlayerSupport playerSupport;


	Global::init(&videoPlayer, &audioPlayer, &playerSupport);

	Q_INIT_RESOURCE(test);
	//QApplication::setStyle(new QPlastiqueStyle);
	
	QTranslator qtTranslator;
	qtTranslator.load("base_trans");
	a.installTranslator(&qtTranslator);
	QTranslator myappTranslator;
	myappTranslator.load("trans");
	a.installTranslator(&myappTranslator);

//	if (!checkTrialState()) return 0;

#if 0	
	
	MainWindow w;	
	a.installEventFilter(&w);
	w.show();
	int ret = a.exec();	
	
	Global::uninit();

	//MagickCoreTerminus();
	return ret;


#else

	float startTime = Global::currentTime();
	
	QPixmap pixmap(":/data/Resources/splash.png");
	QSplashScreen splash(pixmap);
	splash.setWindowFlags(splash.windowFlags() | Qt::WindowStaysOnTopHint);
	splash.setMask(pixmap.mask());
	splash.show();

	MainWindow * w = new MainWindow;	
	a.installEventFilter(w);
	QFile File(":/data/StyleSheet/MainWindow.qss");
	File.open(QFile::ReadOnly);
	QString StyleSheet = QLatin1String(File.readAll());
	w->setStyleSheet(StyleSheet);
	w->splashShow(true);
	w->show();	
	
	if (Global::currentTime() - startTime < 1)
	{
		w->setEnabled(false);
		while(Global::currentTime() - startTime < 1)
		{

			a.processEvents();
		}
		w->setEnabled(true);
	}
	
	splash.finish(w);
	w->splashShow(false);

	if (argc > 1)
	{
		std::string fileNameStr = argv[1];

		//remove starting and trailing quotes
		string::size_type qPos1 = fileNameStr.find_first_of("\"");
		string::size_type qPos2 = fileNameStr.find_last_of("\"");
		if (qPos1 == 0 && qPos2 == fileNameStr.size() - 1)
		{
			fileNameStr = fileNameStr.substr(1, fileNameStr.size() - 2);
		}	

		w->openFile(stdStringToQString(fileNameStr));		
	}

	
	int ret = a.exec();	

	delete w;
	
	Global::uninit();

	//MagickCoreTerminus();
	return ret;
#endif
	
	
}

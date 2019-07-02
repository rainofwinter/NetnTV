#include "stdafx.h"
#include "PcPlayerSupport.h"
#include "AppImage.h"
#include "AppWebContent.h"
#include "FileUtils.h"
#include "Utils.h"
#include "PcScene.h"
#include "STWindow.h"
#include "MainWindow.h"
#include "LogDlg.h"
#include "ScriptProcessor.h"
#include "TextEditObject.h"
#include <QtGui/QMessageBox>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

PcPlayerSupport::PcPlayerSupport()
{
	QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond);
	QueryPerformanceCounter((LARGE_INTEGER *)&ticksAtStart);
	pcScene_ = 0;
	stWindow_ = 0;
	prevHandledObj_ = NULL;
	handledObj_ = NULL;
}

void PcPlayerSupport::setPcScene(PcScene * pcScene)
{
	pcScene_ = pcScene;
}

void PcPlayerSupport::openUrl(const std::string & url, bool isMobile)
{
	ShellExecuteA(NULL, "open", url.c_str(),
		NULL, NULL, SW_SHOWNORMAL);
}

void PcPlayerSupport::launchApp(const std::string & params)
{
	QMessageBox::information(NULL, 
		QObject::tr("Message"), 
		QObject::tr("Launch App action is not supported on PC"));
}

void PcPlayerSupport::showAppLayer(bool show)
{
	pcScene_->showAppLayer(show);
	stWindow_->showAppLayer(show);
}

void PcPlayerSupport::logMessage(const char * msg)
{
	//cout << msg << "\n";
	logDlg_->printLogMsg(msg);
}

void PcPlayerSupport::setLog(LogDlg * logDlg)
{
	logDlg_ = logDlg;
}

float PcPlayerSupport::currentTime()
{				
	QueryPerformanceCounter((LARGE_INTEGER *)&ticks);
	return (float)(ticks - ticksAtStart) / (ticksPerSecond);
}



unsigned int PcPlayerSupport::totalDiskSpace() const
{
	ULARGE_INTEGER freeBytes, totalBytes, totalFreeBytes;
	GetDiskFreeSpaceEx(0, &freeBytes, &totalBytes, &totalFreeBytes);


	return (unsigned int)(totalBytes.QuadPart / 1024 / 1024);
}

unsigned int PcPlayerSupport::freeDiskSpace() const
{
	ULARGE_INTEGER freeBytes, totalBytes, totalFreeBytes;
	GetDiskFreeSpaceEx(0, &freeBytes, &totalBytes, &totalFreeBytes);

	return (double)freeBytes.QuadPart / 1024 / 1024;
}

inline void addToPixel(float * scanLine, int x, int width, float factor, float delta[4])
{
	if (x < 0 || x >= width) return;
	float * pixel = scanLine + x*4;
	pixel[0] += factor * delta[0];
	pixel[1] += factor * delta[1];
	pixel[2] += factor * delta[2];
	pixel[3] += factor * delta[3];
	
}


void PcPlayerSupport::receiveMessage(const std::string & message)
{
	if (message == "__close")
	{
		PostQuitMessage(0);
	}
}

boost::uuids::uuid gAppImageType = AppImage().type();
boost::uuids::uuid gAppWebContentType = AppWebContent().type();

void PcPlayerSupport::initAppObject(AppObject * appObject)
{
	if (appObject->type() == gAppImageType)
	{		
		pcScene_->addAppImage((AppImage *)appObject);
	}	
	else if (appObject->type() == gAppWebContentType)
	{
		pcScene_->addAppWebContent((AppWebContent *)appObject);
	}
}

void PcPlayerSupport::uninitAppObject(AppObject * appObject)
{
	if (appObject->type() == gAppImageType)
	{
		pcScene_->removeAppObject(appObject);
	}
	else if (appObject->type() == gAppWebContentType)
	{
		pcScene_->removeAppObject(appObject);
	}
}

void PcPlayerSupport::sendAppObjectToFront(AppObject * appObject)
{
	pcScene_->sendAppObjectToFront(appObject);
}

void PcPlayerSupport::setPreserveAspectRatio(bool preserveAspectRatio)
{
	if (stWindow_)stWindow_->setPreserveAspect(preserveAspectRatio);
}

void PcPlayerSupport::setSTWindow(STWindow * stWindow)
{
	stWindow_ = stWindow;
}

class ModelessDlg
{
public:
	ModelessDlg(const std::string & msg) {msg_ = msg;}

	void show()
	{
		boost::thread(boost::ref(*this), msg_);
	};
	void operator()(const std::string & str)
	{
		MessageBoxA(0, str.c_str(), "Message", MB_OK);
		}
private:
	std::string msg_;
};


void PcPlayerSupport::msgBox(const char * msg)
{
	//ModelessDlg(msg).show();
	QMessageBox::information(0, QObject::tr("Error"), QString::fromLocal8Bit(msg));
}

void PcPlayerSupport::handledTextEditObj(TextEditObject * handledObj)
{
	prevHandledObj_ = handledObj_;
	handledObj_ = handledObj;
	stWindow_->setIsFocusTextEdit(true);
}

void PcPlayerSupport::focusOutTextEditObj()
{
	prevHandledObj_ = handledObj_; 
	stWindow_->setIsFocusTextEdit(false);
	handledObj_ = NULL;
}

void PcPlayerSupport::setTextEditObjTextString(const std::wstring & str, bool isCurrent)
{
	if (isCurrent)
	{
		handledObj_->setTextString(str);
		handledObj_->inputTextDraw();
	}
	else if (prevHandledObj_ != NULL)
	{
		prevHandledObj_->setTextString(str);
		prevHandledObj_->inputTextDraw();
	}
}

void PcPlayerSupport::selectedTextPart()
{
	stWindow_->selectedTextPart();
}

void PcPlayerSupport::clearTextSelect()
{
	stWindow_->clearTextSelect();
}

void PcPlayerSupport::textMemoClicked(Memo * memo)
{
	stWindow_->memoClicked(memo);
}

const std::string PcPlayerSupport::getModelName()
{
	return "PcModel";
}

const std::string PcPlayerSupport::getOsVersion()
{
#ifdef Q_WS_WIN 
	switch(QSysInfo::windowsVersion()) 
	{
		case QSysInfo::WV_98:
			return "Windows98";
		case QSysInfo::WV_2000:
			return "Windows 2000"; 
		case QSysInfo::WV_Me:
			return "Windows me";
		case QSysInfo::WV_XP: 
			return "Windows XP"; 
		case QSysInfo::WV_VISTA:
			return "Windows Vista";
		case QSysInfo::WV_WINDOWS7:
			return "Windows7";
		case QSysInfo::WV_WINDOWS8:
			return "Windows8";
		default: 
			return "Windows"; 
	}
#endif
}

const std::string PcPlayerSupport::getOs()
{
#ifdef Q_WS_WIN 
	return "Windows";
#endif
}

void PcPlayerSupport::shareMsg(int type, std::string msg)
{
	QMessageBox::information(NULL, 
		QObject::tr("Message"), 
		QObject::tr("Share action is not supported on PC"));
}

void PcPlayerSupport::vibrateAction(int time)
{
	QMessageBox::warning(NULL, 
		QObject::tr("Message"), 
		QObject::tr("Vibrate Action is not supported on PC"));
}
#pragma once
#include "PlayerSupport.h"

class AppObject;
class PcScene;
class STWindow;
class MainWindow;
class LogDlg;
class TextEditObject;
class Memo;

class PcPlayerSupport : public PlayerSupport
{
public:
	PcPlayerSupport();

	void setPcScene(PcScene * pcScene);

	virtual void openUrl(const std::string & str, bool isMobile);

	virtual float currentTime();

	virtual void receiveMessage(const std::string & message);

	virtual void initAppObject(AppObject * appObject);
	virtual void uninitAppObject(AppObject * appObject);

	virtual void sendAppObjectToFront(AppObject * appObject);

	virtual void setPreserveAspectRatio(bool preserveAspectRatio);

	virtual void launchApp(const std::string & params);

	void setSTWindow(STWindow * stWindow);

//	void setMainWindow(MainWindow * mainWindow);

	virtual void msgBox(const char * msg);

	virtual void showAppLayer(bool show);

	virtual void logMessage(const char * msg);
	void setLog(LogDlg * logDlg);
	/**
	@return total disk space in megabytes
	*/
	virtual unsigned int totalDiskSpace() const;

	/**
	@return free disk space in megabytes
	*/
	virtual unsigned int freeDiskSpace() const;

	virtual void handledTextEditObj(TextEditObject * handledObj);
	virtual void focusOutTextEditObj();
	void setTextEditObjTextString(const std::wstring & str, bool isCurrent);
	TextEditObject* handledObj() {return handledObj_;}


	virtual void selectedTextPart();
	virtual void clearTextSelect();

	virtual void textMemoClicked(Memo * memo);

	virtual const std::string getModelName();
	virtual const std::string getOsVersion();
	virtual const std::string getOs();

	virtual void shareMsg(int type, std::string msg);

	virtual void vibrateAction(int time);
private:
	UINT64 ticks, ticksAtStart, ticksPerSecond;
	PcScene * pcScene_;
	STWindow * stWindow_;
//	MainWindow * mainWindow_;
	LogDlg * logDlg_;
	TextEditObject * handledObj_;
	TextEditObject * prevHandledObj_;
};
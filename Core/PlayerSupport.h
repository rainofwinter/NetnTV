#pragma once

class AudioPlayer;
class VideoPlayer;
class AppObject;
class TextEditObject;
class PhotoObject;
struct Memo;
class PlayerSupport
{
public:

	virtual ~PlayerSupport() {}

	virtual void openUrl(const std::string & str, bool isMobile) = 0;	

	virtual float currentTime() = 0;
	
	virtual void receiveMessage(const std::string & message) = 0;

	virtual void initAppObject(AppObject * appObject) = 0;
	virtual void uninitAppObject(AppObject * appObject) = 0;
	virtual void sendAppObjectToFront(AppObject * appObject) = 0;

	virtual void setPreserveAspectRatio(bool preserveAspectRatio) = 0;

	virtual void launchApp(const std::string & params) = 0;

	virtual void showAppLayer(bool show) = 0;

	virtual void logMessage(const char * msg) {}
	
	virtual void shareMsg(int type, std::string msg) = 0;

	/**
	@return total diskspace in megabytes
	*/
	virtual unsigned int totalDiskSpace() const = 0;

	/**
	@return free diskspace in megabytes
	*/
	virtual unsigned int freeDiskSpace() const = 0;

	virtual void msgBox(const char * msg) = 0;

	// for texteditobject
	virtual void handledTextEditObj(TextEditObject * handledObj) = 0;
	virtual void focusOutTextEditObj() = 0;

	// for text select
	virtual void selectedTextPart() {}
	virtual void clearTextSelect() {}
	virtual void textMemoClicked(Memo * memo) {}

	//photo 
	virtual void photoSave(float x, float y, float width, float height){}  
	virtual void photoCameraSwitch() {} 
	virtual void photoAlbum(){}
	virtual void photoCameraOn(float x, float y, float width, float height){}  
	virtual void photoTakePicture(){} 
	virtual void photoCameraOff(){}
	virtual void cameraOff(){}

	virtual void appObjectisVisible(){}

	virtual const std::string getOs() = 0; 
	virtual const std::string getModelName() = 0;
	virtual const std::string getOsVersion() = 0;

	virtual void callbackPageChanged(std::string pageName) {}

	virtual void vibrateAction(int time) = 0;

private:
};
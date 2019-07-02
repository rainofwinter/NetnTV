#pragma once

#include "pthread.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Types.h"
#include "MatrixStack.h"
#include "PlayerSupport.h"
#include "ScriptEventListener.h"
#include "Texture.h"

class MatrixStack;
class SceneObject;
class AppObject;
class AnimationChannel;
class Action;
class Event;
class ImageReader;
class VideoPlayer;
class AudioPlayer;
class DocumentTemplate;
class PlayerSupport;
class Model;
class NetworkRequest;
class Reader;
class Writer;
class Document;
class TextEditObject;
class Color;
struct Memo;

struct LoadRequest
{
	enum ObjectType {LoadIRequestmage, LoadIRequestmage2, LoadRequestZoomImage, LoadRequestFullImage, LoadRequestModel} objectType;

	LoadRequest()
	{
		objectType = LoadRequestModel;
		requester = 0;
	}

	void * requester;
	std::string fileName;
	bool isNeedToLoad;

	bool operator == (const LoadRequest & rhs) const 
	{
		return 
			requester == rhs.requester && 
			fileName == rhs.fileName;
	}

	bool matches (const LoadRequest & rhs) const 
	{
		return requester == rhs.requester && fileName == rhs.fileName;
	}

};

struct LoadedImage
{
	enum Type
	{
		Full,		
		Progressive1,
		Progressive2,
		Failed,
		HasNot
	} imgType;

	Texture::Format comprType;

	LoadRequest request;
	unsigned char * bits;
	int width;
	int height;
};

struct LoadedModel
{
	enum Status
	{
		Loaded,
		Failed		
	} status;
	LoadRequest request;
	Model * model;
	unsigned int size;
};

struct BookmarkData
{
	int pageIndex;
	std::string filename;

	std::string time;
	virtual unsigned char version() const {return 1;}
	virtual void read(Reader & reader, unsigned char version);
	virtual void write(Writer & writer) const;
};

class Global
{
public:
	static void * loadThreadStatic(void * arg);
	static void * saveThreadStatic(void * arg);
public:
	static Global & instance()
	{
		static Global global;
		return global;		
	}

	static void init(		
		VideoPlayer * videoPlayer,
		AudioPlayer * audioPlayer,
		PlayerSupport * playerSupport
		);
	
	static void uninit();	

	static bool useStencilBuffer() {return useStencilBuffer_;}

	///Should be called before init
	static void setUseStencilBuffer(bool use) {useStencilBuffer_ = use;}

	SceneObject * createSceneObject(const boost::uuids::uuid & type) const; 
	SceneObject * createSceneObject(const std::string & typeStr) const;
	AppObject * createAppObject(const boost::uuids::uuid & type) const;
		
	Event * createEvent(const boost::uuids::uuid & type) const;
	Event * createEvent(SceneObject * srcObject, const std::string & typeStr) const;
	/**
	Return a pointer to the event object corresponding to the given string.
	The pointer is owned by this class and should not be modified.
	*/
	Event * event(SceneObject * srcObject, const std::string & eventStr) const;

	Action * createAction(const boost::uuids::uuid & type) const;
	AnimationChannel * createAnimationChannel(const boost::uuids::uuid & type) const;
	AnimationChannel * createAnimationChannel(const std::string & type) const;
	DocumentTemplate * createDocumentTemplate(const boost::uuids::uuid & type) const;

	void supportedAnimationChannels(
		std::vector<AnimationChannel *> * channels, SceneObject * sceneObj) const;

	void sceneObjects(
		std::vector<SceneObject *> * sceneObjects) const;
	const std::vector<SceneObject *> & sceneObjectTypes() const {return sceneObjectTypes_;}

	const std::vector<Event *> & eventTypes() const {return eventTypes_;}
	const std::vector<Action *> & actionTypes() const {return actionTypes_;}
	const std::vector<AnimationChannel *> & channelTypes() const {return channelTypes_;}
	const std::vector<AppObject *> & appObjectTypes() const {return appObjectTypes_;}

	void appObjects(
		std::vector<AppObject *> * appObjects) const;

	void documentTemplates(
		std::vector<DocumentTemplate *> * sceneObjects) const;

	void supportedEvents(std::vector<Event *> * objs, SceneObject * sceneObj) const;
	void supportedEvents(std::vector<Event *> * objs, AppObject * appObj) const;

	void supportedActions(std::vector<Action *> * objs, SceneObject * sceneObj) const;
	void supportedActions(std::vector<Action *> * objs, AppObject * appObj) const;


	void setVideoPlayer(VideoPlayer * videoPlayer);
	VideoPlayer * videoPlayer() const;

	void setAudioPlayer(AudioPlayer * audioPlayer);
	AudioPlayer * audioPlayer() const;

	void getImageDims(const std::string & fileName, int * width, int * height) const;
	
	static void readImage(const std::string & fileName, 
		Texture::Format * comprType, unsigned char ** bits, int * width, int * height);

	static void initAppObject(
		const ContainerObjects & containerObjects, AppObject * appObject);

	static void sendAppObjectToFront(AppObject * appObject)
	{
		playerSupport_->sendAppObjectToFront(appObject);
	}

	static void uninitAppObject(AppObject * appObject)
	{
		playerSupport_->uninitAppObject(appObject);
	}
	
	void openUrl(const std::string & url, bool isMobile) 
	{
		playerSupport_->openUrl(url, isMobile);
	}

	static void launchApp(const std::string & params)
	{
		playerSupport_->launchApp(params);
	}

	void callbackPageChanged(std::string pageName)
	{
		playerSupport_->callbackPageChanged(pageName);
	}

	void shareMsg(int type, std::string msg)
	{
		playerSupport_->shareMsg(type, msg);
	}

	static void vibrateAction(int time)
	{
		playerSupport_->vibrateAction(time);
	}

	static float currentTime();
	static void msgBox(const char * msg);
	static void logMessage(const char * msg);
	static void showAppLayer(bool show);
	static unsigned int totalDiskSpace();
	static unsigned int freeDiskSpace();
	static void broadcastExternalMessage(const std::string & message);
	static void setPreserveAspectRatio(bool preserveAspectRatio);
	static unsigned int version() {return sVersion_;}

	const FT_Library & ftLibrary() const {return ftLibrary_;}

	static void setIsTegra(bool isTegra);
	static unsigned int extraGlClearOptions() {return extraGlClearOptions_;}


	void resetLoadThread();
	void resetSaveThread();

	void requestObjectLoad(void * requester, const std::string & fileName, 
		LoadRequest::ObjectType objType);	
	void requestLocalSaveObject(void * requester, const std::string & fileName, 
		LoadRequest::ObjectType objType);
	void requestLocalSaveObjectFirst(const std::string & fileName);

	void loadImageLocal(const LoadRequest & l, const LoadedImage::Type & type, const std::string & argLocalFileName);
	void loadImageRemote(const LoadRequest & l, const LoadedImage::Type & type, const std::string & argFileName);

	void cancelObjectLoad(void * requester, const std::string & fileName);

	bool getLoadedImage(void * requester, const std::string & filename,
		Texture::Format * compressionType, 
		unsigned char ** bits, int * width, int * height, LoadedImage::Type * progType, bool * isLoadedimgempty);

	bool getLoadedModel(void * requester, const std::string & filename,
		Model ** model);	

	static float flickTime() {return sFlickTime_;}

	void setDirectories(
		const std::string & readDir, 
		const std::string & writeDir, 
		const std::string & remoteReadServer);

	const std::string & readDirectory() const {return readDir_;}
	const std::string & writeDirectory() const {return writeDir_;}
	const std::string & remoteReadServer() const {return remoteReadServer_;}

	void setMagnification(float mag) {magnification_ = mag;}
	const float magnification() {return magnification_;}

	void setCurSceneMagnification(float mag) {curSceneMagnification_ = mag;}
	const float curSceneMagnification() {return curSceneMagnification_;}
	void setGlobalCurMagnification(float mag) {globalCurMagnification_ = mag;}
	const float globalCurMagnification() {return globalCurMagnification_;}

	static std::vector<BookmarkData> readBookmark(const char * filename);
	static void deleteBookmark(int index, const char * filename);

	void requestDeleteFile(std::string filename);
    void resetSaveLocalRequest();

	const bool isCurrentSceneLoaded() {return isCurrentSceneLoaded_;}
	void setIsCurrentSceneLoaded(bool var) {isCurrentSceneLoaded_ = var;}

	void setCurDocument(Document* doc) {curdoc_ = doc;}
	Document* curDocument() {return curdoc_;}

	void setIsNetworkConnected(bool var) {isNetworkConnected_ = var;}

	void * idToPtr(int id);
	int ptrToId(void *);
	void unregisterPtr(void *);

	void handledTextEditObj(TextEditObject * handledObj);
	void focusOutTextEditObj()
	{
		playerSupport_->focusOutTextEditObj();
	}

	
	void selectedTextPart();
	void clearTextSelect();
	void textMemoClicked(Memo * memo);

	void photoTakePicture();
	void photoCameraSwitch();
	void photoSave(float x, float y, float width, float height);
	void photoAlbum();
	void photoCameraOn(float x, float y, float width, float height);
	void photoCameraOff();
	void cameraOff();

	const std::string getOs();
	const std::string getModelName();
	const std::string getOsVersion();

	void appObjectisVisible();
private:	
	Global();	
	~Global();

	bool init_;

	void loadThread();
	void saveThread();


	/**
	must have locked loadMutex_ through the duration of this function
	*/
	void cancelCurDownload();
	std::string fullFile(const std::string & fileName) const;
	std::string progressive1File(const std::string & fileName) const;
	std::string progressive2File(const std::string & fileName) const;
	std::string progressiveFullFile(const std::string & fileName) const;

	std::string zoomFile(const std::string & fileName) const;
	/**
	@param fileName the original filename as given in the scene description file
	@return filename of the merged file. Empty string if there was an error
	*/
	std::string mergeProgressiveImg(const std::string & fileName) const;
	
	

	/**
	List of available animation channel object types
	*/
	std::vector<AnimationChannel *> channelTypes_;

	/**
	List of available scene object types
	*/
	std::vector<SceneObject *> sceneObjectTypes_;
	std::vector<boost::uuids::uuid> blockedSceneObjectTypes_;

	std::vector<AppObject *> appObjectTypes_;
	std::vector<boost::uuids::uuid> blockedAppObjectTypes_;

	std::vector<Event *> eventTypes_;
	std::vector<boost::uuids::uuid> blockedEventTypes_;

	std::vector<Action *> actionTypes_;
	std::vector<boost::uuids::uuid> blockedActionTypes_;

	std::vector<DocumentTemplate *> documentTemplateTypes_;

	VideoPlayer * videoPlayer_;
	AudioPlayer * audioPlayer_;
	static PlayerSupport * playerSupport_;

	static float sFlickTime_;

	static unsigned int sVersion_;

	FT_Library ftLibrary_;

	

	static bool useStencilBuffer_;
	static unsigned int extraGlClearOptions_;

	pthread_t loadThread_;	
	bool loadThreadTerminate_;

	void removeLoadedImages(const LoadRequest & request);
	void removeLoadedModels(const LoadRequest & request);

	bool checkLastestCancel(const LoadRequest & request);

	/*	
	accesed by multiple threads (ui thread + Global class threads) 
	must be synchronized through loadMutex_
	*/
	//----------------------------

	pthread_mutex_t loadMutex_;
	std::deque<LoadRequest> loadRequests_;
	std::vector<LoadRequest> latestCancelRequests_;
	std::deque<LoadRequest> saveLocalRequests_;
	std::deque<LoadRequest> prioritySaveRequests_;
	std::vector<LoadedImage> loadedImgs_;
	std::vector<LoadedModel> loadedModels_;	

	//----------------------------
	////end must be synchronized through loadMutex_

	unsigned int currentLoadedMemory_;
	

	
	std::string readDir_, writeDir_;
	std::string remoteReadServer_;
	bool downloadCancel_;

	//These should only be accessed in this class's own thread
	LoadedImage downloadImg_;
	NetworkRequest * networkRequest_;
	std::string downloadingFile_;	
	std::string downloadUrl_;

	//std::vector<LoadRequest> failedRequests_;
	//int requestcount_;	

	float magnification_;
	float curSceneMagnification_;
	float globalCurMagnification_;

	std::string savelocalFile_;
	pthread_t saveThread_;
	pthread_mutex_t saveMutex_;
	bool saveThreadTerminate_;

	bool isCurrentSceneLoaded_;

	//use for appobject event
	Document* curdoc_;

	///keep track of pointers by integer id
	///Before, we used to do (Type *)id, but this doesn't work in 64 bit...
	std::map<void *, int> ptrToIdMap_;
	std::map<int, void *> idToPtrMap_;
	int curPtrId_;

	bool isNetworkConnected_;

	bool newLoadRequest_;
};
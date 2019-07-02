#include "stdafx.h"
#include "Global.h"
#include "Image.h"
#include "Scene.h"
#include "SubScene.h"
#include "SceneChanger.h"
#include "BoundingBoxObject.h"
#include "CustomObject.h"
#include "ImageGrid.h"
#include "TurnTable.h"
#include "Panorama.h"
#include "Group.h"
#include "Rectangle.h"
#include "Text.h"
#include "Text_2.h"
#include "TextTable.h"
#include "Model.h"
#include "CameraObject.h"
#include "LightObject.h"
#include "AudioObject.h"
#include "VideoObject.h"
#include "ModelFile.h"
#include "Map.h"
#include "ButtonObject.h"
#include "PhotoObject.h"
#include "ImageAnimation.h"
#include "TAnimation.h"
#include "ImageChanger.h"
#include "AnimationChannel.h"
#include "Event.h"
#include "SceneStartEvent.h"
#include "ScenePreStartEvent.h"
#include "SceneStopEvent.h"
#include "ClickEvent.h"
#include "DoubleClickEvent.h"
#include "ReceiveMessageEvent.h"
#include "ImageGridImageClickedEvent.h"
#include "PageChangedEvent.h"
#include "PageChangingEvent.h"
#include "VideoStoppedEvent.h"
#include "AudioPlayTimeEvent.h"
#include "VideoPlayTimeEvent.h"
#include "AudioPlayedEvent.h"
#include "AudioStoppedEvent.h"
#include "AudioResumedEvent.h"
#include "AudioPausedEvent.h"
#include "VideoObjectPlayedEvent.h"
#include "VideoObjectStoppedEvent.h"
#include "PressEvent.h"
#include "KeyPressEvent.h"
#include "KeyReleaseEvent.h"
#include "LoadedEvent.h"
#include "MoveEvent.h"
#include "ReleaseEvent.h"
#include "PhotoSaveEvent.h"
#include "PhotoTakePictureEvent.h"
#include "PhotoCameraOnEvent.h"
#include "PhotoCameraOffEvent.h"
#include "ElementMapping.h"
#include "TextEditObject.h"
#include "MaskObject.h"


#include "Document.h"
#include "TextFeatures.h"

#include "AppImage.h"
#include "AppWebContent.h"

#include "Action.h"
#include "PlayAnimationAction.h"
#include "StopActionsAction.h"
#include "ResumeAnimationAction.h"
#include "ShowOnlyAction.h"
#include "ShowAllAction.h"
#include "WaitUntilAction.h"
#include "EffectAction.h"
#include "ResetAnimationAction.h"
#include "StopAnimationAction.h"
#include "SceneChangeAction.h"
#include "PlayVideoAction.h"
#include "PlayAudioAction.h"
#include "ResumeAudioAction.h"
#include "PauseAudioAction.h"
#include "StopAllAudioAction.h"
#include "SubsceneResetPositionAction.h"
#include "StopVideoAction.h"
#include "OpenUrlAction.h"
#include "LaunchAppAction.h"
#include "BroadcastMessageAction.h"
#include "SceneChangerShowSceneAction.h"
#include "SceneChangerSceneSetAction.h"
#include "ImageChangerShowImageAction.h"
#include "ImageChangerSetImageAction.h"
#include "ImageAnimationAction.h"
#include "TAnimationAction.h"
#include "ImageSetFileAction.h"
#include "SetVisibilityAction.h"
#include "SetOpacityAction.h"
#include "SetCameraAction.h"
#include "AudioPlayAudioAction.h"
#include "AudioStopAudioAction.h"
#include "AudioPauseAudioAction.h"
#include "AudioResumeAudioAction.h"
#include "VideoPlayVideoAction.h"
#include "VideoStopVideoAction.h"
#include "ImageAnimationResumeAction.h"
#include "ImageAnimationPauseAction.h"
#include "StartRecordAudioAction.h"
#include "StopRecordAudioAction.h"
#include "PhotoAction.h"
#include "ShareAction.h"
#include "VibrateAction.h"

#include "DocumentTemplate.h"
#include "SingleSceneDocumentTemplate.h"
#include "SceneChangerDocumentTemplate.h"
#include "MagazineDocumentTemplate.h"
//#include "CatalogDocumentTemplate.h"

#include "Root.h"
#include "ImageReader.h"
#include "VideoPlayer.h"

#include "Reader.h"
#include "Writer.h"
#include "Exception.h"
#include "FileUtils.h"

#include <libxml/parser.h>

#include "AppObject.h"

#include "Network.h"
#include <boost/lexical_cast.hpp>

#ifdef MSVC
#include <boost/thread.hpp>
#else
#include <unistd.h>
#endif

#include <time.h>
#include <curl/curl.h>

using namespace std;

//version
unsigned int Global::sVersion_ = 26;
////////


PlayerSupport * Global::playerSupport_ = 0;
float Global::sFlickTime_ = 0.25f;
bool Global::useStencilBuffer_ = true;
unsigned int Global::extraGlClearOptions_ = 0;

unsigned int gMaxLoadedMemory = 8*2048*2048*4;

bool checkFileExists(const std::string & fileName)
{
	FILE * file = fopen(fileName.c_str(), "rb");
	bool exists = file != NULL;
	if (file) fclose(file);
	return exists;
}


void Global::setIsTegra(bool isTegra)
{
	if (isTegra)
		extraGlClearOptions_ = GL_COVERAGE_BUFFER_BIT_NV;
	else
		extraGlClearOptions_ = 0;
}

bool sceneObjectSort(SceneObject * lhs, SceneObject * rhs)
{
	return string(lhs->typeStr()) < string(rhs->typeStr());
}

bool actionSort(Action * lhs, Action * rhs)
{
	return string(lhs->typeStr()) < string(rhs->typeStr());
}

bool eventSort(Event * lhs, Event * rhs)
{
	return string(lhs->typeStr()) < string(rhs->typeStr());
}


Global::Global()
{
	curl_global_init(CURL_GLOBAL_ALL);

	FT_Error err;
	err = FT_Init_FreeType(&ftLibrary_);
	
	channelTypes_.push_back(new TransformChannel);
	channelTypes_.push_back(new OpacityChannel);
	channelTypes_.push_back(new VisibleChannel);

	appObjectTypes_.push_back(new AppImage);
	appObjectTypes_.push_back(new AppWebContent);

	sceneObjectTypes_.push_back(new Image);
	sceneObjectTypes_.push_back(new Group);
	sceneObjectTypes_.push_back(new Studio::Rectangle);
	sceneObjectTypes_.push_back(new Subscene);
	sceneObjectTypes_.push_back(new TurnTable);
	sceneObjectTypes_.push_back(new Panorama);
	sceneObjectTypes_.push_back(new SceneChanger);
	sceneObjectTypes_.push_back(new Text);
	sceneObjectTypes_.push_back(new Text_2);
	sceneObjectTypes_.push_back(new TextTable);
	sceneObjectTypes_.push_back(new ModelFile);
	sceneObjectTypes_.push_back(new Map);
	sceneObjectTypes_.push_back(new ImageGrid);
	sceneObjectTypes_.push_back(new ImageChanger);
	sceneObjectTypes_.push_back(new ImageAnimation);
	sceneObjectTypes_.push_back(new TAnimationObject);
	sceneObjectTypes_.push_back(new CameraObject);
	sceneObjectTypes_.push_back(new LightObject);
	sceneObjectTypes_.push_back(new AudioObject);
	sceneObjectTypes_.push_back(new VideoObject);
	sceneObjectTypes_.push_back(new BoundingBoxObject);
	sceneObjectTypes_.push_back(new TextEditObject);
	sceneObjectTypes_.push_back(new MaskObject);
	sceneObjectTypes_.push_back(new ButtonObject);
	sceneObjectTypes_.push_back(new CustomObject);
	sceneObjectTypes_.push_back(new PhotoObject);
	sort(sceneObjectTypes_.begin(), sceneObjectTypes_.end(),
		sceneObjectSort);
	blockedSceneObjectTypes_.push_back(CustomObject().type());
	blockedSceneObjectTypes_.push_back(Text().type());


	eventTypes_.push_back(new SceneStartEvent);
	eventTypes_.push_back(new SceneStopEvent);
	eventTypes_.push_back(new ClickEvent);
	eventTypes_.push_back(new DoubleClickEvent);
	eventTypes_.push_back(new ReceiveMessageEvent);
	eventTypes_.push_back(new ImageGridImageClickedEvent);
	eventTypes_.push_back(new SceneChangerPageChangedEvent);
	eventTypes_.push_back(new PageChangingEvent);
	eventTypes_.push_back(new VideoStoppedEvent);
	eventTypes_.push_back(new AudioPlayTimeEvent);
	eventTypes_.push_back(new VideoPlayTimeEvent);
	eventTypes_.push_back(new PressEvent);
	eventTypes_.push_back(new LoadedEvent);
	eventTypes_.push_back(new ReleaseEvent);
	eventTypes_.push_back(new KeyPressEvent);
	eventTypes_.push_back(new KeyReleaseEvent);
	eventTypes_.push_back(new MoveEvent);
	eventTypes_.push_back(new AudioPlayedEvent);
	eventTypes_.push_back(new AudioStoppedEvent);
	eventTypes_.push_back(new AudioPausedEvent);
	eventTypes_.push_back(new AudioResumedEvent);
	eventTypes_.push_back(new VideoObjectPlayedEvent);
	eventTypes_.push_back(new VideoObjectStoppedEvent);
	eventTypes_.push_back(new PhotoSaveEvent);
	eventTypes_.push_back(new PhotoTakePictureEvent);
	eventTypes_.push_back(new PhotoCameraOnEvent);
	eventTypes_.push_back(new PhotoCameraOffEvent);

	blockedEventTypes_.push_back(MoveEvent().type());
	blockedEventTypes_.push_back(LoadedEvent().type());
	blockedEventTypes_.push_back(ScenePreStartEvent().type());
	blockedEventTypes_.push_back(KeyPressEvent().type());
	blockedEventTypes_.push_back(KeyReleaseEvent().type());

	sort(eventTypes_.begin(), eventTypes_.end(),
		eventSort);
	
	actionTypes_.push_back(new PlayAnimationAction);
	actionTypes_.push_back(new StopActionsAction);
	actionTypes_.push_back(new ResumeAnimationAction);
	actionTypes_.push_back(new ShowOnlyAction);
	actionTypes_.push_back(new ShowAllAction);
	actionTypes_.push_back(new WaitUntilAction);
	actionTypes_.push_back(new EffectAction);
	actionTypes_.push_back(new ResetAnimationAction);
	actionTypes_.push_back(new StopAnimationAction);
	actionTypes_.push_back(new SceneChangeAction);
	actionTypes_.push_back(new PlayVideoAction);
	actionTypes_.push_back(new StopVideoAction);
	actionTypes_.push_back(new OpenUrlAction);
	actionTypes_.push_back(new LaunchAppAction);
	actionTypes_.push_back(new BroadcastMessageAction);
	actionTypes_.push_back(new SceneChangerShowSceneAction);
	actionTypes_.push_back(new SceneChangerSceneSetAction);
	actionTypes_.push_back(new SubsceneResetPositionAction);
	actionTypes_.push_back(new ImageSetFileAction);
	actionTypes_.push_back(new ImageChangerSetImageAction);
	actionTypes_.push_back(new ImageAnimationPlayAction);
	actionTypes_.push_back(new ImageAnimationStopAction);
	actionTypes_.push_back(new ImageAnimationResetAction);
	actionTypes_.push_back(new ImageChangerShowImageAction);
	actionTypes_.push_back(new PlayAudioAction);
	actionTypes_.push_back(new ResumeAudioAction);
	actionTypes_.push_back(new StopAllAudioAction);
	actionTypes_.push_back(new PauseAudioAction);
	actionTypes_.push_back(new SetVisibilityAction);
	actionTypes_.push_back(new SetOpacityAction);
	actionTypes_.push_back(new SetCameraAction);
	actionTypes_.push_back(new AudioPlayAudioAction);
	actionTypes_.push_back(new AudioPauseAudioAction);
	actionTypes_.push_back(new AudioResumeAudioAction);
	actionTypes_.push_back(new AudioStopAudioAction);
	actionTypes_.push_back(new VideoPlayVideoAction);
	actionTypes_.push_back(new VideoStopVideoAction);
	actionTypes_.push_back(new ImageAnimationPauseAction);
	actionTypes_.push_back(new ImageAnimationResumeAction);
	actionTypes_.push_back(new StartRecordAudioAction);
	actionTypes_.push_back(new StopRecordAudioAction);
	actionTypes_.push_back(new TAnimationPlayAction);
	actionTypes_.push_back(new TAnimationStopAction);
	actionTypes_.push_back(new TAnimationResetAction);
	actionTypes_.push_back(new PhotoTakePictureAction);
	actionTypes_.push_back(new PhotoCameraOnAction);
	actionTypes_.push_back(new PhotoCameraOffAction);
    actionTypes_.push_back(new PhotoAlbumAction);
	actionTypes_.push_back(new PhotoCameraSwitchAction);
	actionTypes_.push_back(new PhotoSave);
	actionTypes_.push_back(new ShareAction);
	actionTypes_.push_back(new VibrateAction);

	sort(actionTypes_.begin(), actionTypes_.end(),
		actionSort);

	documentTemplateTypes_.push_back(new SingleSceneDocumentTemplate);
	documentTemplateTypes_.push_back(new SceneChangerDocumentTemplate);
	documentTemplateTypes_.push_back(new MagazineDocumentTemplate);
	//documentTemplateTypes_.push_back(new CatalogDocumentTemplate);
	//documentTemplateTypes_.push_back(new BookDocumentTemplate);
	
	downloadCancel_ = false;
	init_ = false;

	networkRequest_ = new NetworkRequest;

	JS_SetCStringsAreUTF8();

	magnification_ = 1;
	curSceneMagnification_ = 1;
	globalCurMagnification_ = 1;

	isCurrentSceneLoaded_ = false;
	isNetworkConnected_ = true;

	curPtrId_ = 0;

	xmlInitParser();
}

Global::~Global()
{
	uninit();

	delete networkRequest_;

	BOOST_FOREACH(AnimationChannel * channel, channelTypes_)
		delete channel;

	BOOST_FOREACH(SceneObject * obj, sceneObjectTypes_)
		delete obj;

	BOOST_FOREACH(AppObject * obj, appObjectTypes_)
		delete obj;

	BOOST_FOREACH(Action * action, actionTypes_)
		delete action;

	BOOST_FOREACH(Event * event, eventTypes_)
		delete event;

	BOOST_FOREACH(DocumentTemplate * docTemplateType, documentTemplateTypes_)
		delete docTemplateType;

	xmlCleanupParser();
	FT_Done_FreeType(ftLibrary_);	

	curl_global_cleanup();

}

void Global::init(
	VideoPlayer * videoPlayer, 
	AudioPlayer * audioPlayer,
	PlayerSupport * playerSupport)
{	
	Global & g = instance();
	if (g.init_)
    {
        g.resetLoadThread();
        g.resetSaveThread();
        return;
    }
	
	g.setVideoPlayer(videoPlayer);
	g.setAudioPlayer(audioPlayer);
	g.playerSupport_ = playerSupport;

	g.currentLoadedMemory_ = 0;
	g.init_ = true;
	g.loadThreadTerminate_ = false;	
	g.saveThreadTerminate_ = false;	
	g.downloadCancel_ = false;

	pthread_mutex_init(&g.loadMutex_, 0);
	pthread_create(&g.loadThread_, 0, Global::loadThreadStatic, &g);
	//pthread_mutex_init(&g.saveMutex_, 0);
	pthread_create(&g.saveThread_, 0, Global::saveThreadStatic, &g);
	
}

void Global::uninit()
{
	Global & g = instance();
	if (!g.init_) return;

	g.loadThreadTerminate_ = true;
	pthread_join(g.loadThread_, 0);	
	g.saveThreadTerminate_ = true;
	pthread_join(g.saveThread_, 0);	
	pthread_mutex_destroy(&g.loadMutex_);
	//pthread_mutex_destroy(&g.saveMutex_);

	BOOST_FOREACH(LoadedImage & img, g.loadedImgs_)
		free(img.bits);

	g.loadedImgs_.clear();
	g.currentLoadedMemory_ = 0;
	
	g.init_ = false;
}

float Global::currentTime() {return playerSupport_->currentTime();}
void Global::msgBox(const char * msg) {playerSupport_->msgBox(msg);}
void Global::logMessage(const char * msg) {playerSupport_->logMessage(msg);}
void Global::showAppLayer(bool showVal) 
{
	playerSupport_->showAppLayer(showVal);
	Global & g = instance();
	if (g.curdoc_) g.curdoc_->textFeatures()->show(showVal);
}
unsigned int Global::totalDiskSpace() {return playerSupport_->totalDiskSpace();}
unsigned int Global::freeDiskSpace() {return playerSupport_->freeDiskSpace();}
void Global::broadcastExternalMessage(const std::string & message) {playerSupport_->receiveMessage(message);}
void Global::setPreserveAspectRatio(bool preserve) {playerSupport_->setPreserveAspectRatio(preserve);}

SceneObject * Global::createSceneObject(const boost::uuids::uuid & type) const
{
	if (type == Root().type()) return new Root;
	if (type == CustomObject().type()) return new CustomObject;

	BOOST_FOREACH(SceneObject * sceneObj, sceneObjectTypes_)
	{
		if (sceneObj->type() == type) return sceneObj->clone(0);
	}	
	return 0;
}

SceneObject * Global::createSceneObject(const std::string & typeStr) const
{
	if (typeStr == Root().typeStr()) return new Root;

	BOOST_FOREACH(SceneObject * sceneObj, sceneObjectTypes_)
	{
		if (sceneObj->typeStr() == typeStr) return sceneObj->clone(0);					
	}		
	return 0;
}


AppObject * Global::createAppObject(const boost::uuids::uuid & type) const
{
	BOOST_FOREACH(AppObject * obj, appObjectTypes_)
	{
		if (obj->type() == type) return obj->clone();
	}	
	return 0;
}


Event * Global::createEvent(const boost::uuids::uuid & type) const
{
	BOOST_FOREACH(Event * obj, eventTypes_)
	{
		if (obj->type() == type) return obj->clone();
	}
	
	return 0;
}

Event * Global::createEvent(SceneObject * srcObject, const std::string & type) const
{
	BOOST_FOREACH(Event * obj, eventTypes_)
	{
		if (obj->typeStr() == type && obj->supportsObject(srcObject)) 
		{
			Event * retEvent = obj->clone();
			//retEvent->target = srcObject;
			return retEvent;
		}
	}
	
	return 0;
}


Event * Global::event(SceneObject * srcObject, const std::string & eventStr) const
{
	BOOST_FOREACH(Event * event, eventTypes_)
	{
		if (event->typeStr() == eventStr && event->supportsObject(srcObject))
		{
			return event;
		}
	}
	return 0;
}

Action * Global::createAction(const boost::uuids::uuid & type) const
{
	BOOST_FOREACH(Action * obj, actionTypes_)
	{
		if (obj->type() == type) return obj->clone();
	}
	
	return 0;
}

AnimationChannel * Global::createAnimationChannel(const boost::uuids::uuid & type) const
{	
	BOOST_FOREACH(AnimationChannel * obj, channelTypes_)
	{
		if (obj->type() == type) return obj->clone();
	}
	
	return 0;
}

AnimationChannel * Global::createAnimationChannel(const std::string & type) const
{	
	BOOST_FOREACH(AnimationChannel * obj, channelTypes_)
	{
		if (obj->name() == type) return obj->clone();
	}
	
	return 0;
}

DocumentTemplate * Global::createDocumentTemplate(const boost::uuids::uuid & type) const
{
	BOOST_FOREACH(DocumentTemplate * obj, documentTemplateTypes_)
	{
		if (obj->type() == type) return obj->newInstance();
	}
	
	return 0;
}

void Global::supportedAnimationChannels(
	std::vector<AnimationChannel *> * channels, SceneObject * sceneObj) const
{
	channels->clear();
	BOOST_FOREACH(AnimationChannel * channel, channelTypes_)
	{
		if (channel->supportsObject(sceneObj)) channels->push_back(channel);

	}
}

void Global::sceneObjects(std::vector<SceneObject *> * sceneObjects) const
{
	sceneObjects->clear();
	BOOST_FOREACH(SceneObject * obj, sceneObjectTypes_)
	{
		vector<boost::uuids::uuid>::const_iterator iter;
		iter = find(blockedSceneObjectTypes_.begin(), blockedSceneObjectTypes_.end(),
			obj->type());

		if (iter == blockedSceneObjectTypes_.end())
			sceneObjects->push_back(obj);
	}
}


void Global::appObjects(std::vector<AppObject *> * appObjects) const
{
	appObjects->clear();
	BOOST_FOREACH(AppObject * obj, appObjectTypes_)
	{
		bool isBlocked = false;
		BOOST_FOREACH(const boost::uuids::uuid & uuid, blockedAppObjectTypes_)
		{
			if (obj->type() == uuid) isBlocked = true;
		}

		if (!isBlocked) appObjects->push_back(obj);
	}
}


void Global::documentTemplates(
	std::vector<DocumentTemplate *> * docTemps) const
{
	docTemps->clear();
	BOOST_FOREACH(DocumentTemplate * docTemp, documentTemplateTypes_)
	{
		docTemps->push_back(docTemp);
	}
}

void Global::supportedEvents(std::vector<Event *> * objs, SceneObject * sceneObj) const
{
	objs->clear();
	BOOST_FOREACH(Event * obj, eventTypes_)
	{
		if (obj->supportsObject(sceneObj)) 
		{
			bool isBlocked = false;
			BOOST_FOREACH(const boost::uuids::uuid & uuid, blockedEventTypes_)
			{
				if (obj->type() == uuid) isBlocked = true;
			}

			if (!isBlocked) objs->push_back(obj);
		}
	}
}

void Global::supportedEvents(std::vector<Event *> * objs, AppObject * appObj) const
{
	objs->clear();
	BOOST_FOREACH(Event * obj, eventTypes_)
	{
		if (obj->type() == ClickEvent().type())
		{
			objs->push_back(obj);
		}
	}
}

void Global::supportedActions(std::vector<Action *> * objs, SceneObject * sceneObj) const
{
	objs->clear();
	BOOST_FOREACH(Action * obj, actionTypes_)
	{
		if (obj->supportsObject(sceneObj)) 
		{
			bool isBlocked = false;
			BOOST_FOREACH(const boost::uuids::uuid & uuid, blockedActionTypes_)
			{
				if (obj->type() == uuid) isBlocked = true;
			}

			if (!isBlocked) objs->push_back(obj);
		}
	}
}

void Global::supportedActions(std::vector<Action *> * objs, AppObject * appObj) const
{
	objs->clear();
	BOOST_FOREACH(Action * obj, actionTypes_)
	{
		if (obj->type() == BroadcastMessageAction().type())
		{
			objs->push_back(obj);
		}
	}
}


void Global::setVideoPlayer(VideoPlayer * videoPlayer)
{
	videoPlayer_ = videoPlayer;
}

VideoPlayer * Global::videoPlayer() const
{
	return videoPlayer_;
}

void Global::setAudioPlayer(AudioPlayer * audioPlayer)
{
	audioPlayer_ = audioPlayer;
}

AudioPlayer * Global::audioPlayer() const
{
	return audioPlayer_;
}

void Global::initAppObject(const ContainerObjects & containerObjects, AppObject * appObject)
{
	appObject->setContainerObjects(containerObjects);
	playerSupport_->initAppObject(appObject);
}


void * Global::loadThreadStatic(void * arg)
{
	((Global *)arg)->loadThread();

	return 0;
}

void * Global::saveThreadStatic(void * arg)
{
	((Global *)arg)->saveThread();

	return 0;
}


bool Global::checkLastestCancel(const LoadRequest & request)
{
	vector<LoadRequest>::iterator itr = latestCancelRequests_.begin();
	for (; itr != latestCancelRequests_.end() ; itr++)
	{
		if (itr->matches(request))
		{
			latestCancelRequests_.erase(itr);
			return true;
		}
	}

	return false;
}

//void gfxLog(const std::string & str);

void Global::loadThread()
{
	while(!loadThreadTerminate_)
	{
        pthread_mutex_lock(&loadMutex_);
		LoadRequest loadRequest;
		//bool newLoadRequest = false;
		newLoadRequest_ = false;
        bool fileDownloading = !downloadingFile_.empty();
        //may need to cancel current download if it was for a cancelled request
		if (fileDownloading && checkLastestCancel(downloadImg_.request)) 
			downloadCancel_ = true;
        
        if (downloadCancel_)
		{
			cancelCurDownload();
			downloadCancel_ = false;
		}
        
        pthread_mutex_unlock(&loadMutex_);
        
        fileDownloading = !downloadingFile_.empty();
		
		if (fileDownloading && !networkRequest_->isRunning())
		{
			//if download finished (successfully or not)
            pthread_mutex_lock(&loadMutex_);
			string readFileName = downloadingFile_;
            
			if (networkRequest_->wasSuccessful())
			{	
				if (downloadImg_.imgType == LoadedImage::Progressive2)
				{
					//this is the second part of the progressive image.
					//generate the full image from the two parts
					readFileName = mergeProgressiveImg(downloadImg_.request.fileName);
				}
			}
			else
			{
				if (networkRequest_->wasFileExist())
					downloadImg_.imgType = LoadedImage::Failed;
				else
					downloadImg_.imgType = LoadedImage::HasNot;

				if (checkFileExists(downloadingFile_))
					unlink(downloadingFile_.c_str());         
			}
            
            pthread_mutex_unlock(&loadMutex_);
            
			if (networkRequest_->wasSuccessful())
				loadImageLocal(downloadImg_.request, downloadImg_.imgType, readFileName);
			//else if (networkRequest_->wasFileExist())
			//{
			//	pthread_mutex_lock(&loadMutex_);
			//	newLoadRequest = true;
			//	loadRequest = downloadImg_.request;
			//	pthread_mutex_unlock(&loadMutex_);
			//}
            
			//signal fileDownloading = false
            pthread_mutex_lock(&loadMutex_);
			downloadingFile_.clear();
            pthread_mutex_unlock(&loadMutex_);
		}
		pthread_mutex_lock(&loadMutex_);
        latestCancelRequests_.clear();
			
		if (!loadRequests_.empty() && currentLoadedMemory_ < gMaxLoadedMemory)
		{
			loadRequest = loadRequests_.front();
			if (downloadingFile_ != loadRequest.fileName)
			{
				//newLoadRequest = true;
				newLoadRequest_ = true;
				loadRequests_.pop_front();			
			}
		}

		pthread_mutex_unlock(&loadMutex_);

		//if (newLoadRequest)
		if (newLoadRequest_)
		{	
			if (loadRequest.objectType == LoadRequest::LoadIRequestmage)
			{
				string localFileName = loadRequest.fileName;				
				bool fileExists = checkFileExists(localFileName);			
				
				//if the file doesn't exist in the read directory, check the write directory
				if (!fileExists)
				{
					localFileName = fullFile(loadRequest.fileName);
					fileExists = checkFileExists(localFileName);
				}

				if (fileExists)
					loadImageLocal(loadRequest, LoadedImage::Full, localFileName);
				else //full image file doesn't exist
				{
					bool isProgressiveFile = boost::iequals(getFileExtension(loadRequest.fileName), "jpg");
					bool prog1Exists;
					if (isProgressiveFile)
					{
						string prog1FileName = progressive1File(loadRequest.fileName);
						localFileName = prog1FileName;
						prog1Exists = checkFileExists(localFileName);

						if (prog1Exists)
							loadImageLocal(loadRequest, LoadedImage::Progressive1, localFileName);
					}

					if (!isProgressiveFile || !prog1Exists)
					{
						pthread_mutex_lock(&loadMutex_);
						prioritySaveRequests_.push_back(loadRequest);	
						pthread_mutex_unlock(&loadMutex_);
					}		
				}			
				
			}
			else if (loadRequest.objectType == LoadRequest::LoadIRequestmage2)
			{
				string prog2FileName = progressive2File(loadRequest.fileName);
				//localFileName = prog2FileName;
				bool prog2Exists = checkFileExists(prog2FileName);

				if (prog2Exists)
				{
					string readfileName = mergeProgressiveImg(loadRequest.fileName);
					loadImageLocal(loadRequest, LoadedImage::Progressive2, readfileName);
				}
				else
				{
					pthread_mutex_lock(&loadMutex_);
					prioritySaveRequests_.push_back(loadRequest);	
					pthread_mutex_unlock(&loadMutex_);
				}		
			}
			else if (loadRequest.objectType == LoadRequest::LoadRequestZoomImage)
            {
				string localFileName = zoomFile(loadRequest.fileName);

				if (checkFileExists(localFileName))
					loadImageLocal(loadRequest, LoadedImage::Full, localFileName);
				else
				{
					pthread_mutex_lock(&loadMutex_);
					prioritySaveRequests_.push_back(loadRequest);	
					pthread_mutex_unlock(&loadMutex_);
				}		
			}
			else if (loadRequest.objectType == LoadRequest::LoadRequestFullImage)
            {
				if (checkFileExists(loadRequest.fileName))
					loadImageLocal(loadRequest, LoadedImage::Full, loadRequest.fileName);
				else
				{
					pthread_mutex_lock(&loadMutex_);
					prioritySaveRequests_.push_back(loadRequest);	
					pthread_mutex_unlock(&loadMutex_);
				}		
			}
			else if (loadRequest.objectType == LoadRequest::LoadRequestModel)
			{
				LoadedModel obj;
				obj.request = loadRequest;
				

				FILE * file = 0;

				BOOST_SCOPE_EXIT( (&file) )
				{
					if (file) fclose(file);
				} BOOST_SCOPE_EXIT_END

				obj.model = NULL;
				obj.size = 0;
				obj.status = LoadedModel::Failed;
				file = fopen(loadRequest.fileName.c_str(), "rb");
				if (file)
				{			
					fseek(file, 0, SEEK_END);
					//note: using the filesize like this is not an accurate 
					//representation of how much memory will be allocated
					obj.size = 1.5f*ftell(file);
					fseek(file, 0, SEEK_SET);

					obj.model = Model::read(getDirectory(loadRequest.fileName).c_str(), file);					
					obj.status = LoadedModel::Loaded;
				}								
					
				currentLoadedMemory_ += obj.size;
				loadedModels_.push_back(obj);
			}
			
		}//end if (newLoadRequest)

				//sched_yield();
#ifndef MSVC
		usleep(1000);
#else
		boost::this_thread::sleep(boost::posix_time::milliseconds(1)); 
#endif
	}	//end while(!loadThreadTerminate_)

	cancelCurDownload();
}
void Global::saveThread()
{
	while(!saveThreadTerminate_)
	{
		
		
		pthread_mutex_lock(&loadMutex_);
        
        bool fileDownloading = !downloadingFile_.empty();

		bool newSaveLocalRequest = false;

		//latestCancelRequests_.clear();
		LoadRequest loadRequest;

		if (!fileDownloading && !prioritySaveRequests_.empty())
		{
			loadRequest = prioritySaveRequests_.front();
			prioritySaveRequests_.pop_front();
			newSaveLocalRequest = true;
		}else if (!fileDownloading && !saveLocalRequests_.empty())
		{
			loadRequest = saveLocalRequests_.front();
			saveLocalRequests_.pop_front();
			newSaveLocalRequest = true;
		}
		pthread_mutex_unlock(&loadMutex_);

		if (newSaveLocalRequest && !fileDownloading /*&& (requestcount_ < 30)*/)
		{	
			
			if (loadRequest.objectType == LoadRequest::LoadIRequestmage)
			{
				string localFileName = loadRequest.fileName;
				bool isProgressiveFile = boost::iequals(getFileExtension(loadRequest.fileName), "jpg");
				bool fileExists = checkFileExists(localFileName);
				bool prog1Exists = false;
				string prog1FileName;
				
				
				//if the file doesn't exist in the read directory, check the write directory
				if (!fileExists)
				{
					localFileName = fullFile(loadRequest.fileName);
					fileExists = checkFileExists(localFileName);
				}

				if (!fileExists)
				{
					if (isProgressiveFile)
					{
						prog1FileName = progressive1File(loadRequest.fileName);
						localFileName = prog1FileName;
						prog1Exists = checkFileExists(localFileName);
						if (prog1Exists)
						{
							if(loadRequest.isNeedToLoad)
							{
								pthread_mutex_lock(&loadMutex_);
								loadRequests_.push_back(loadRequest);	
								pthread_mutex_unlock(&loadMutex_);
							}
						}
						else
							loadImageRemote(loadRequest, LoadedImage::Progressive1, prog1FileName);
					}
					else
						loadImageRemote(loadRequest, LoadedImage::Full, fullFile(loadRequest.fileName));
				}
				else
				{
					if(loadRequest.isNeedToLoad)
					{
						pthread_mutex_lock(&loadMutex_);
						loadRequests_.push_back(loadRequest);	
						pthread_mutex_unlock(&loadMutex_);
					}
				}
			}
			else if (loadRequest.objectType == LoadRequest::LoadIRequestmage2)
			{
				string prog2FileName = progressive2File(loadRequest.fileName);
				loadImageRemote(loadRequest, LoadedImage::Progressive2, prog2FileName);			
			}
			else if (loadRequest.objectType == LoadRequest::LoadRequestZoomImage)
			{
				string localFileName = zoomFile(loadRequest.fileName);

				if (checkFileExists(localFileName))
				{
					if (loadRequest.isNeedToLoad)
					{
						pthread_mutex_lock(&loadMutex_);
						loadRequests_.push_back(loadRequest);	
						pthread_mutex_unlock(&loadMutex_);
					}
				}
				else
					loadImageRemote(loadRequest, LoadedImage::Full, localFileName);			
			}
			else if (loadRequest.objectType == LoadRequest::LoadRequestFullImage)
			{
				if (checkFileExists(loadRequest.fileName))
				{
					if (loadRequest.isNeedToLoad)
					{
						pthread_mutex_lock(&loadMutex_);
						loadRequests_.push_back(loadRequest);	
						pthread_mutex_unlock(&loadMutex_);
					}
				}
				else
					loadImageRemote(loadRequest, LoadedImage::Full, loadRequest.fileName);	
			}
			else
			{
				throw Exception("Unknown save object type");
			}		

		}//end if (newLoadRequest)

				//sched_yield();
#ifndef MSVC
		usleep(1000);
#else
		boost::this_thread::sleep(boost::posix_time::milliseconds(1)); 
#endif
	}	//end while(!saveThreadTerminate_)
}

void Global::resetLoadThread()
{
	pthread_mutex_lock(&loadMutex_);
	BOOST_FOREACH(LoadedImage & img, loadedImgs_)
		free(img.bits);
	loadedImgs_.clear();

	BOOST_FOREACH(LoadedModel & obj, loadedModels_)
		delete obj.model;
	loadedModels_.clear();

	currentLoadedMemory_ = 0;
	loadRequests_.clear();
	downloadCancel_ = true;
	pthread_mutex_unlock(&loadMutex_);
}

void Global::resetSaveThread()
{
	pthread_mutex_lock(&loadMutex_);

    prioritySaveRequests_.clear();
	saveLocalRequests_.clear();
	downloadCancel_ = true;
	pthread_mutex_unlock(&loadMutex_);
}

void Global::requestObjectLoad(void * requester, 
	const std::string & fileName, LoadRequest::ObjectType objectType)
{
	LoadRequest request;
	request.requester = requester;
	request.fileName = getAbsFileName(readDir_, fileName);
	request.objectType = objectType;
	request.isNeedToLoad = true;
	
	pthread_mutex_lock(&loadMutex_);
	loadRequests_.push_back(request);	
	pthread_mutex_unlock(&loadMutex_);
	
}

void Global::requestLocalSaveObject(void * requester, 
	const std::string & fileName, LoadRequest::ObjectType objectType)
{	
	LoadRequest request;
	request.requester = requester;
	request.fileName = getAbsFileName(readDir_, fileName);
	request.objectType = objectType;
	request.isNeedToLoad = false;
	pthread_mutex_lock(&loadMutex_);
    bool filenameExist = false;
    bool isLoadExist = false;
    BOOST_FOREACH(LoadRequest & l, saveLocalRequests_)
    {
        filenameExist |= l.fileName == request.fileName;
        isLoadExist |= l.isNeedToLoad == request.isNeedToLoad;
        if (filenameExist && isLoadExist)
            break;
        filenameExist = false;
        isLoadExist = false;
    }
    if (!filenameExist || !isLoadExist)
        saveLocalRequests_.push_back(request);	
	pthread_mutex_unlock(&loadMutex_);
}

void Global::requestLocalSaveObjectFirst(const std::string & fileName)
{
	LoadRequest request;
	request.requester = 0;
	request.fileName = getAbsFileName(readDir_, fileName);
	request.objectType = LoadRequest::LoadIRequestmage;
	request.isNeedToLoad = false;
	pthread_mutex_lock(&loadMutex_);
	prioritySaveRequests_.push_front(request);	
	pthread_mutex_unlock(&loadMutex_);
}

void Global::cancelObjectLoad(void * requester, const std::string & fileName)
{
	LoadRequest request;
	request.requester = requester;
	request.fileName = getAbsFileName(readDir_, fileName);
		

	pthread_mutex_lock(&loadMutex_);
    latestCancelRequests_.push_back(request);
	deque<LoadRequest>::iterator iterSub = 
		loadRequests_.begin();

	for (;iterSub != loadRequests_.end();)
	{
		LoadRequest & curReqSub = *iterSub;
		if (curReqSub.matches(request))
			iterSub = loadRequests_.erase(iterSub);
		else
			++iterSub;
	}
    iterSub = saveLocalRequests_.begin();
    
	for (;iterSub != saveLocalRequests_.end();)
	{
		LoadRequest & curReqSub = *iterSub;
		if (curReqSub.matches(request))
			iterSub = saveLocalRequests_.erase(iterSub);
		else
			++iterSub;
	}
    iterSub = prioritySaveRequests_.begin();
    
	for (;iterSub != prioritySaveRequests_.end();)
	{
		LoadRequest & curReqSub = *iterSub;
		if (curReqSub.matches(request))
			iterSub = prioritySaveRequests_.erase(iterSub);
		else
			++iterSub;
	}
	
	removeLoadedImages(request);
	removeLoadedModels(request);	

	pthread_mutex_unlock(&loadMutex_);
	
}


//-----------------------------------------------------------------------------

void Global::removeLoadedImages(const LoadRequest & curReq)
{
	vector<LoadedImage>::iterator liter = loadedImgs_.begin();
	for (; liter != loadedImgs_.end();)
	{
		LoadedImage & img = *liter;
		if (img.request.matches(curReq))
		{
			free(img.bits);
			currentLoadedMemory_ -= Texture::getTextureDataSize(img.comprType, img.width, img.height);
			liter = loadedImgs_.erase(liter);
		}
		else 
			++liter;
	}
}

//-----------------------------------------------------------------------------

void Global::removeLoadedModels(const LoadRequest & curReq)
{
	vector<LoadedModel>::iterator liter = loadedModels_.begin();
	for (; liter != loadedModels_.end();)
	{
		LoadedModel & img = *liter;
		if (img.request.matches(curReq))
		{
			delete img.model;
			currentLoadedMemory_ -= img.size;
			liter = loadedModels_.erase(liter);
		}
		else 
			++liter;
	}
}


void Global::cancelCurDownload()
{		
	LOGI("cancelling %s", downloadingFile_.c_str());
	if (checkFileExists(downloadingFile_))
		unlink(downloadingFile_.c_str());
	networkRequest_->abort();
	downloadingFile_ = "";	
}

bool Global::getLoadedImage(void * requester, const std::string & fileName,
	Texture::Format * texCompr, unsigned char ** bits, int * width, int * height, 
	LoadedImage::Type * progType, bool * isLoadedimgempty)
{
    pthread_mutex_lock(&loadMutex_);
	LoadRequest request;
	request.requester = requester;
	request.fileName = getAbsFileName(readDir_, fileName);

	bool loadedImgFound = false;

	vector<LoadedImage>::iterator iter = loadedImgs_.begin();
	for (; iter != loadedImgs_.end(); ++iter)
	{
		LoadedImage & img = *iter;

		if (img.request == request && img.imgType != LoadedImage::Failed)
		{
			loadedImgFound = true;
			*texCompr = img.comprType;
			*progType = img.imgType;
			*bits = img.bits;
			*width = img.width;
			*height = img.height;
			currentLoadedMemory_ -= Texture::getTextureDataSize(img.comprType, img.width, img.height);
			loadedImgs_.erase(iter);
		
			break;
		}
		else if(img.imgType == LoadedImage::Failed)
		{
			currentLoadedMemory_ -= Texture::getTextureDataSize(img.comprType, img.width, img.height);
			loadedImgs_.erase(iter);
		
			break;
		}
	}
	if (loadedImgs_.empty() && loadRequests_.empty() && !loadedImgFound && !newLoadRequest_)
		*isLoadedimgempty = true;

	pthread_mutex_unlock(&loadMutex_);
	return loadedImgFound;	
}


bool Global::getLoadedModel(void * requester, const std::string & fileName,
	Model ** model)
{
	LoadRequest request;
	request.requester = requester;
	request.fileName = getAbsFileName(readDir_, fileName);

	bool loadedObjFound = false;

	pthread_mutex_lock(&loadMutex_);
	vector<LoadedModel>::iterator iter = loadedModels_.begin();
	for (; iter != loadedModels_.end(); ++iter)
	{
		LoadedModel & obj = *iter;

		if (obj.request == request)
		{
			loadedObjFound = true;
			*model = obj.model;
			currentLoadedMemory_ -= obj.size;
			loadedModels_.erase(iter);

			break;
		}
	}

	pthread_mutex_unlock(&loadMutex_);

	return loadedObjFound;
}

void Global::setDirectories(
	const std::string & readDir, 
	const std::string & writeDir, 
	const std::string & remoteReadServer)
{
	
	readDir_ = readDir;
	writeDir_ = writeDir;
	remoteReadServer_ = remoteReadServer;

	boost::trim(readDir_);
	boost::trim(writeDir_);

	char last; 

	if (!readDir_.empty())	
	{
		last = readDir_[readDir_.size() - 1];
		if (last != '/' && last != '\\') readDir_.push_back('/');
	}

	if (!writeDir_.empty())
	{
		last = writeDir_[writeDir_.size() - 1];
		if (last != '/' && last != '\\') writeDir_.push_back('/');
	}

	if (!remoteReadServer_.empty())
	{
		last = remoteReadServer[remoteReadServer_.size() - 1];
		if (last != '/' && last != '\\') remoteReadServer_.push_back('/');
	}
	
}

std::string Global::fullFile(const std::string & fileName) const
{
	return getAbsFileName(writeDir_, getFileNameWithoutDirectory(fileName));
}

std::string Global::progressive1File(const std::string & fileName) const
{	
	return getAbsFileName(writeDir_, getFileNameWithoutDirectory(fileName) + ".base.jpg");
}

std::string Global::progressive2File(const std::string & fileName) const
{
	return getAbsFileName(writeDir_, getFileNameWithoutDirectory(fileName) + ".enh.jpg");
}

std::string Global::progressiveFullFile(const std::string & fileName) const
{
	return getAbsFileName(writeDir_, getFileNameWithoutDirectory(fileName));
}

std::string Global::zoomFile(const std::string & fileName) const
{
	return getAbsFileName(writeDir_, getFileNameWithoutDirectory(fileName) + ".zoom.jpg");
}

std::string Global::mergeProgressiveImg(const std::string & fileName) const
{
	string prog1Img = progressive1File(fileName);
	string prog2Img = progressive2File(fileName);
	string progFullImg = progressiveFullFile(fileName);

	FILE * p1File = NULL, * p2File = NULL, * pFullFile = NULL;
	BOOST_SCOPE_EXIT( (&p1File) (&p2File) (&pFullFile) (&prog1Img) (&prog2Img)) {
		if (p1File) fclose(p1File);
		if (p2File) fclose(p2File);
		if (pFullFile) fclose(pFullFile);

		unlink(prog1Img.c_str());
		unlink(prog2Img.c_str());
    } BOOST_SCOPE_EXIT_END

	p1File = fopen(prog1Img.c_str(), "rb");
	if (!p1File) return "";

	p2File = fopen(prog2Img.c_str(), "rb");
	if (!p2File) return "";

	pFullFile = fopen(progFullImg.c_str(), "wb");
	if (!pFullFile) return "";

	const size_t BUF_SIZE = 4096;
	unsigned char buffer[BUF_SIZE];
	size_t len;

	while((len = fread(buffer, 1, BUF_SIZE, p1File)) > 0)	
		fwrite(buffer, len, 1, pFullFile);

	fseek(pFullFile, -2, SEEK_END);

	while((len = fread(buffer, 1, BUF_SIZE, p2File)) > 0)	
		fwrite(buffer, len, 1, pFullFile);
	
	return progFullImg;
}

void Global::loadImageLocal(const LoadRequest & l, const LoadedImage::Type & type, const std::string & argLocalFileName)
{
	if (!l.isNeedToLoad)
		return;
	LoadedImage img;
	img.request = l;	
	img.imgType = type;
    
	string localFileName = argLocalFileName;
    if (checkFileExists(zoomFile(l.fileName)))
        localFileName = zoomFile(l.fileName);


	if (checkFileExists(localFileName))
	{
		FILE *f;
		if ((f = fopen(localFileName.c_str(), "rb")) == NULL)
			img.bits = NULL;
		else
		{
			fseek(f, 0, SEEK_END);
			if (ftell(f) == 0)
				img.bits = NULL;
			else
			{
				readImage(localFileName, 
					&img.comprType, &img.bits, &img.width, &img.height);
			}
			fseek(f, 0, SEEK_SET);
		}
		if(f) fclose(f);		
	}
	else
	{
		img.bits = NULL;
	}
    
    pthread_mutex_lock(&loadMutex_);
	if (img.bits == NULL)
	{
		img.imgType = LoadedImage::Failed;
		if (checkFileExists(localFileName))
			unlink(localFileName.c_str());
	}

	currentLoadedMemory_ += Texture::getTextureDataSize(img.comprType, img.width, img.height);
	if (checkLastestCancel(l) || argLocalFileName.empty() || img.bits == NULL)
	{
		free(img.bits);
		currentLoadedMemory_ -= Texture::getTextureDataSize(img.comprType, img.width, img.height);
	}
	else
	{
		loadedImgs_.push_back(img);
	}        
	pthread_mutex_unlock(&loadMutex_);
}

void Global::loadImageRemote(const LoadRequest & l, const LoadedImage::Type & type, const std::string & argFileName)
{	
	pthread_mutex_lock(&loadMutex_);

	downloadingFile_ = argFileName;
	downloadImg_.imgType = type;
	downloadUrl_ = remoteReadServer_ + getFileNameWithoutDirectory(downloadingFile_);

	if(!isNetworkConnected_)
	{
		prioritySaveRequests_.push_front(l);
		pthread_mutex_unlock(&loadMutex_);
		return;
	}

	int timeout = 0;
	
	if (type == LoadedImage::Full)
		timeout = 30;
	else if (type == LoadedImage::Progressive1)
		timeout = 5;
	else if (type == LoadedImage::Progressive2)
		timeout = 30;
	networkRequest_->setTimeout(timeout);
	networkRequest_->setFile(downloadingFile_);
	networkRequest_->start(downloadUrl_);
	downloadImg_.request = l;
    pthread_mutex_unlock(&loadMutex_);
}

void Global::requestDeleteFile(std::string filename)
{
	if (checkFileExists(zoomFile(filename)))
		unlink(zoomFile(filename).c_str());
	else if (checkFileExists(progressive2File(filename)))
		unlink(progressive2File(filename).c_str());
	else if (checkFileExists(progressive1File(filename)))
		unlink(progressive1File(filename).c_str());
	else if (checkFileExists(fullFile(filename)))
		unlink(fullFile(filename).c_str());
}

std::vector<BookmarkData> Global::readBookmark(const char * filename)
{
	std::vector<BookmarkData> bookmarks;
	FILE * file = fopen(filename, "rb");

	if (file)
	{
		Reader(file).read(bookmarks);
		fclose(file);
	}

	return bookmarks;
}

void Global::deleteBookmark(int index, const char * filename)
{
	std::vector<BookmarkData> bookmarks;

	bookmarks = Global::readBookmark(filename);

	vector<BookmarkData>::iterator iter;
	for (iter = bookmarks.begin(); iter != bookmarks.end(); ++iter)
	{
		const BookmarkData & bookmark = *iter;
		if (bookmark.pageIndex == index)
		{
			bookmarks.erase(iter);
			break;
		}
	}
	Writer(filename, 0).write(bookmarks);
}

void Global::resetSaveLocalRequest()
{
    pthread_mutex_lock(&loadMutex_);
    saveLocalRequests_.clear();
    pthread_mutex_unlock(&loadMutex_);
}

void BookmarkData::read(Reader & reader, unsigned char version)
{
	reader.read(pageIndex);
	reader.read(filename);
	if (version >= 1)
		reader.read(time);
	else
		time = "00000000000000";
}

void BookmarkData::write(Writer & writer) const
{
	writer.write(pageIndex);
	writer.write(filename);
	writer.write(time);
}


void * Global::idToPtr(int id)
{
	std::map<int, void *>::iterator iter = idToPtrMap_.find(id);
	if (iter == idToPtrMap_.end()) return NULL;
	else return (*iter).second;
}

int Global::ptrToId(void * ptr)
{
	std::map<void *, int>::iterator iter = ptrToIdMap_.find(ptr);
	if (iter == ptrToIdMap_.end())
	{
		int newPtrId = curPtrId_++;
		ptrToIdMap_[ptr] = newPtrId;
		idToPtrMap_[newPtrId] = ptr;
		return newPtrId;
	}
	else return (*iter).second;
}

void Global::unregisterPtr(void * ptr)
{
	std::map<void *, int>::iterator iter = ptrToIdMap_.find(ptr);
	if (iter != ptrToIdMap_.end())
	{
		int id = (*iter).second;
		ptrToIdMap_.erase(ptr);
		idToPtrMap_.erase(id);
	}
}

void doReadImage(const std::string & fileNameStr, Texture::Format * comprType, unsigned char ** bits, int * width, int * height);

void Global::readImage(const std::string & fileName, 
	Texture::Format * comprType, unsigned char ** bits, int * width, int * height)
{
	doReadImage(fileName, comprType, bits, width, height);
}

void Global::getImageDims(const std::string & fileName, int * width, int * height) const
{
	Texture::Format trtype; 
	unsigned char * trbits;
	readImage(getAbsFileName(readDir_, fileName), &trtype, &trbits, width, height);
	free(trbits);
}

void Global::handledTextEditObj(TextEditObject * handledObj)
{
	playerSupport_->handledTextEditObj(handledObj);
}

void Global::selectedTextPart()
{
	playerSupport_->selectedTextPart();
}

void Global::clearTextSelect()
{
	curdoc_->textFeatures()->clearTextSelect();
	playerSupport_->clearTextSelect();
}

void Global::textMemoClicked(Memo * memo)
{
	playerSupport_->textMemoClicked(memo);
}

void Global::photoTakePicture()
{
	playerSupport_->photoTakePicture();
}

void Global::photoCameraSwitch()
{

	playerSupport_->photoCameraSwitch();
}

void Global::photoSave(float x, float y, float width, float height)
{
	playerSupport_->photoSave(x, y, width, height);
}

void Global::photoAlbum()
{
	playerSupport_->photoAlbum();
}

void Global::photoCameraOn(float x, float y, float width, float height)
{
	playerSupport_->photoCameraOn(x, y, width, height);
}

void Global::photoCameraOff()
{
	playerSupport_->photoCameraOff();
}

void Global::cameraOff()
{
	playerSupport_->cameraOff();
	curdoc_->photoDefaultImage();
}

void Global::appObjectisVisible()
{
	playerSupport_->appObjectisVisible();
}

const std::string Global::getOs()
{
	return playerSupport_->getOs();
}

const std::string Global::getModelName()
{
	return playerSupport_->getModelName();
}

const std::string Global::getOsVersion()
{
	return playerSupport_->getOsVersion();
}
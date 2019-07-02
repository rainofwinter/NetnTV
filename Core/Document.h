#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/type_traits.hpp>
#include <string>
#include <map>

#include "Types.h"
#include "MathStuff.h"
#include "TextFont.h"
#include "Orientation.h"
#include "BoundingBox.h"

class GfxRenderer;
class Writer;
class Reader;
class PageFlip;
class TempRenderObject;
class Color;
class TextHandler;
class AppObject;
class DocumentTemplate;
class ScriptProcessor;
class VideoObject;
class Text;
class XmlWriter;
class XmlReader;
class Scene;
class PhotoObject;
//#define NETNTV_WATERMARK

enum ContentType
{
	ContentTypeInternal,
	ContentTypeCEC,
	ContentTypeUDreamMedia,
	ContentTypeLite
};

enum DocumentEvent
{
	DocumentEventClose,
	DocumentEventUpdate,
	DocumentEventExternalMessage
};

class CustomObjectData;


struct JSTracer;

class TextFeatures;

class Document
{
	friend class XmlReader;
public:
	static JSClass scriptObjectClass();
	static JSObject * createScriptObjectProto(ScriptProcessor * s);
	/**
	prevent event handlers from being garbage collected during the lifetime 
	of this object. Not don't root the event handlers. Then they will never 
	be garbage collected.
	*/
	static void gcMark(JSTracer * trc, JSObject * obj);

	Document();
	Document(int width, int height);
	~Document();
	const std::vector<SceneSPtr> & scenes() const {return scenes_;}
	void setScenes(const DocumentTemplateSPtr & docTemp, const std::vector<SceneSPtr> & scenes);	
	void addScene(const SceneSPtr & scene);	
	void deleteScene(const Scene * scene);
	SceneSPtr findScene(const Scene * scene);

	Scene * findScene(const std::string & id) const;

	void remapDirectories(
		const std::string & oldDir, 
		const std::string & newDir);

	void appObjectBoundingBox(const AppObject * appObject, 
		float * x, float * y, float * w, float * h,
		bool * clip, float * clipX, float * clipY, float * clipW, float * clipH) const;

	/**
	refFiles is added onto, and is not cleared.
	*/
	void referencedFiles(std::vector<std::string> * refFiles) const;
	/**
	@return end index (= to next object's start index)
	*/
	void setReferencedFiles(
		const std::string & baseDirectory,
		const std::vector<std::string> & refFiles);

	GfxRenderer * renderer() const {return gl_;}
	void draw();
	
	void start();
	bool update();
	void videoStoppedEvent();
	void audioPlayedEvent(int id);
	void audioStoppedEvent(int id);
	void audioPausedEvent(int id);
	void audioResumedEvent(int id);


	void videoPlayedEvent(int id);
	void videoStoppedEvent(int id);
	void videoPausedEvent(int id);
	void videoSeekedEvent(int id, float time);
	void videoResumedEvent(int id);


	/**
	@param propagate if false this press - move - release sequence will only be
	handled at the upper template level and not be propagated to the scene / 
	scene object level
	*/
	void pressEvent(const Vector2 & startPos, int pressId, bool propagate = true);
	void moveEvent(const Vector2 & curPos, int pressId);
	void releaseEvent(const Vector2 & curPos, int pressId);

	void keyPressEvent(int keyCode);
	void keyReleaseEvent(int keyCode);

	void appObjectTapEvent(AppObject * appObject);

	void broadcastMessage(Scene * scene, const std::string & msg);
	void broadcastExternalMessage(const std::string & message);

	std::string getScriptAccessibleFile(const std::string & scriptSideName) const;

	void readTextFeatures(const std::string & textFeaturesFileName);
	///return root scene
	/*
	Scene * scene() const {return scene_;}
	*/

	void setScriptFile(const std::string & scriptFile) 
	{
		scriptFiles_.clear();
		scriptFiles_.push_back(scriptFile);
	}
	void setScriptFiles(const std::vector<std::string> & scriptFiles)
	{
		scriptFiles_ = scriptFiles;
	}
	const std::vector<std::string> & scriptFiles() const
	{
		return scriptFiles_;
	}

	const std::map<std::string, std::string> & scriptAccessibleFiles() const
	{
		return scriptAccessibleFiles_;
	}

	void setScriptAccessibleFiles(
		const std::map<std::string, std::string> & scriptAccessibleFiles)
	{
		scriptAccessibleFiles_ = scriptAccessibleFiles;
	}


	void init();
	void uninit();	

	const DocumentTemplateSPtr & documentTemplate() const {return template_;}
	void setDocumentTemplate(const DocumentTemplateSPtr & docTemplate);

	void insertBefore(const SceneSPtr & newObj, Scene * refObj);
	void setSize(
		int width, int height, 
		float dpiScale, float minMoveDistScale, 
		bool resizeGl = true);
	
	void setMaskSize(int width, int height);
	
	void addTempRenderObject(TempRenderObject * tempRenderObject);

	int origWidth() const {return origWidth_;}
	int origHeight() const {return origHeight_;}

	bool isCurrentScene(Scene * scene) const;


	const Orientation & orientation() const {return orientation_;}
	void setOrientation(const Orientation & orientation)
	{
		orientation_ = orientation;
	}

	const int & width() const {return width_;}	
	const int & height() const {return height_;}
	
	/*
	const int & glViewPortX() const {return glViewPortX_;}
	const int & glViewPortY() const {return glViewPortY_;}
	*/

	const std::vector<TempRenderObject *> & tempRenderObjects() const 
	{
		return tempRenderObjects_;
	}

	void sceneChangeTo(Scene * scene);
	void sceneSet(Scene * scene);

	void triggerRedraw() {redrawTriggered_ = true;}

	void setCurSceneScale(float sx, float sy)
	{
		curSceneScaleX_ = sx;
		curSceneScaleY_ = sy;
	}

	float curSceneScaleX() const {return curSceneScaleX_;}
	float curSceneScaleY() const {return curSceneScaleY_;}

	ScriptProcessor * scriptProcessor() const {return scriptProcessor_;}

	//-------------------------------------------------------------------------	
	unsigned char version() const {return 17;}
	void write(Writer & writer) const;
	void writeXml(XmlWriter & writer) const;

	void read(Reader & reader, unsigned char);
	void readXml(XmlReader & r, xmlNode * node);

	Vector2 screenToDeviceCoords(const Vector2 & pos) const;
	Vector2 deviceToScreenCoords(const Vector2 & pos) const;

	bool addEventListener(const std::string & event, JSObject * handler);
	bool removeEventListener(const std::string & event, JSObject * handler);
	void fireEvent(DocumentEvent events, JSObject * evtObject);


	void playVideo(
		const VideoObject * vidObj,
		const ContainerObjects & containerObjects, 
		const std::string & sourceName, 
		int x, int y, int width, int height, bool isUrl, 
		bool showControls, float startTime);

	void playVideoFullscreen(
		const VideoObject * vidObj,
		const ContainerObjects & containerObjects, 
		const std::string & fileName, bool isUrl, bool showControls, 
		float startTime);

	/**
	Returns whether the given chain of containerObjects can be considered to
	have originated the currently playing video.

	Note: this is true if containerObjects contains videoPlayingContainers_
	even if they are not completely equal
	*/
	bool isCurrentVideoScene(const ContainerObjects & containerObjects);

	Scene * topScene() const {return topScene_.get();}

	const std::string & remoteReadServer() const {return remoteReadServer_;}
	void setRemoteReadServer(const std::string & remoteReadServer)
	{
		remoteReadServer_ = remoteReadServer;
	}

	CustomObjectData * customObjectData(const boost::uuids::uuid & id) const;
	void registerCustomObject(const boost::uuids::uuid & customType, 
		const std::string & name,
		const std::string & script);

	void setTextData(const std::string & textData);
	const std::string & textData() const {return textData_;}
	void setAllowMultitouch(const bool & allowMultitouch) {allowMultitouch_ = allowMultitouch;}
	const bool & allowMultitouch() const {return allowMultitouch_;}

	// 단말기에서 북마크를 제어할 경우
	bool setCurPageBookmark();
	void getBookmarkPages(std::vector<int> * pages);
	void goToPage(const int & index);
	int pageIndex();

	// Landscape
	void changeLandscape(const bool & var);

	// catalog phone navigation bar
	const bool & isNaviBarDisplayed() const {return isNaviBarDisplayed_;}
	void changeNaviBarDisplay() {isNaviBarDisplayed_ = !isNaviBarDisplayed_;}

	void pause();
	void resume();
	void pauseTimeReset();

	bool isDocumentExit() {return doucumentExit_;}
	void documentExit() {doucumentExit_ = true;}

	TextFeatures * textFeatures() const {return tf_.get();}
	float dpiScale() const {return dpiScale_;}

	void photoSetFileName(const std::string & fileName);
	void photoSaveComplete();
	void photoCaptureComplete();
	void photoCameraOn(
		const PhotoObject * photoObj,
		const ContainerObjects & containerObjects, 
		float x, float y, float width, float height);
	void photoSave(
		const PhotoObject * photoObj,
		const ContainerObjects & containerObjects, 
		float x, float y, float width, float height);
	void photoDefaultImage();
	bool cameraState(){return cameraState_;}
	void setCameraState(bool state){cameraState_ = state;}

	void textEditUpdate();

	void totalScenes();
	const std::vector<SceneSPtr> & getTotalScenes(){return totalScenes_;}

	Scene * curScene() const;
private:	
	void readXmlScenes(XmlReader & r, xmlNode * node);
	void readXmlTemplate(XmlReader & r, xmlNode * node);

	Vector2 toTopSceneCoords(const Vector2 & coords) const;
/*
	void pressEvent(const Vector2 & startPos, int pressId);
	void moveEvent(const Vector2 & curPos, int pressId);
	void releaseEvent(const Vector2 & curPos, int pressId);*/

	struct PressEventData
	{		
		bool startedMoving;
		Vector2 startPos;

		enum EventTarget
		{Template, TopScene, TextFeatures} eventTarget;

		PressEventData()
		{
			startedMoving = false;
			eventTarget = Template;
		}

	};


	bool dispatchPressEvent(bool * isTopScene, const Vector2 & startPos, int pressId, bool propagate);
	bool dispatchMoveEvent(bool isTopScene, const Vector2 & curPos, int pressId);
	bool dispatchReleaseEvent(bool isTopScene, const Vector2 & curPos, int pressId);
	bool dispatchClickEvent(bool isTopScene, const Vector2 & curPos, int pressId);
	bool dispatchDoubleClickEvent(bool isTopScene, const Vector2 & curPos, int pressId);

	std::string readDir_, writeDir_;

	unsigned int globalVersion_;

	DocumentTemplateSPtr template_;

	//Scene * scene_;
	void create();
	std::vector<SceneSPtr> scenes_;
	
	Orientation orientation_;

	float docTime_;

	GfxRenderer * gl_;
	int width_;
	int height_;

	std::vector<TempRenderObject *> tempRenderObjects_;	

	///key - id, value - startPos
	std::map<int, PressEventData> pressEventMap_;

	float prevSingleClickTime_;
	Vector2 prevClickPos_;


	bool redrawTriggered_;

	int origWidth_, origHeight_;
	float curSceneScaleX_, curSceneScaleY_;

	float minMoveDist_;
	/*
	float glViewPortX_, glViewPortY_;
	float templateWidth_, templateHeight_;
	*/

	std::map<std::string, std::string> scriptAccessibleFiles_;
	std::vector<std::string> scriptFiles_;
	ScriptProcessor * scriptProcessor_;
	bool scriptsStarted_;


	JSObject * scriptObject_;

	typedef std::multimap<DocumentEvent, JSObject *> EventListeners;
	EventListeners eventListeners_;

	/**
	To which scene (container object) does the currently playing video belong?	
	*/
	ContainerObjects videoPlayingContainers_;
	int videoX_, videoY_, videoW_, videoH_;
	enum VideoState
	{
		VideoStateStopped,
		VideoStateRegion,
		VideoStateFullscreen,
	}videoPlayState_;	

#ifdef NETNTV_WATERMARK
	//netntv watermark
	Texture * netntvTexture_;
#endif

	SceneSPtr topScene_;
	
	std::string remoteReadServer_;

	bool resizeText_;

	std::map<boost::uuids::uuid, boost::shared_ptr<CustomObjectData> > 
		customObjectData_;

	bool allowMultitouch_;

	std::string textData_;

	double pausedTime_, checkTime_;
	bool isPaused_;

	// catalog phone navigation bar
	bool isNaviBarDisplayed_;
	bool doucumentExit_;


	float dpiScale_;

	boost::scoped_ptr<TextFeatures> tf_;
	std::string textFeaturesFileName_;

	bool cameraState_;

	std::vector<SceneSPtr> totalScenes_;
};


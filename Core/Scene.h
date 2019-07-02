#pragma once

#include "Types.h"
#include "MathStuff.h"
#include "Color.h"
class SceneObject;
class Root;
class GfxRenderer;
class Event;
class Animation;
class Reader;
class Writer;
class XmlReader;
class XmlWriter;
class Subscene;
class ElementMapping;
class SceneContainer;
class ScriptProcessor;
class CameraObject;
class SceneReferencer;
class LightObject;
class ModelFile;
class Text;

struct JSObject;

class Scene
{
	friend class Document;
	friend class Subscene;
	friend class XmlReader;
public:
	static JSObject * createScriptObjectProto(ScriptProcessor * s);
	JSObject * getScriptObject(ScriptProcessor * s);
	static void gcMark(JSTracer * trc, JSObject * obj);
	
	Scene();
	Scene(int width, int height);
	Scene(const Scene & rhs, ElementMapping * mapping);
	void remapReferences(const ElementMapping & mapping);
	void referencedFiles(std::vector<std::string> * refFiles) const;
	int setReferencedFiles(const std::string & baseDirectory, 
		const std::vector<std::string> & refFiles, int index);
	~Scene();

	Vector2 screenToDeviceCoords(const Vector2 & pos) const;

	int numAppObjects() const;
	const std::vector<AppObjectSPtr> & appObjects() const {return appObjects_;}
	AppObjectSPtr deleteAppObject(AppObject * appObject);
	
	void setAppObjects(const std::vector<AppObjectSPtr> & appObjects)
	{
		appObjects_ = appObjects;
	}

	/**
	@param firstTime purpose is similar as in SceneObject
	*/
	void init(GfxRenderer * gl, bool firstTime = false);
	bool isinit() const {return init_;}
	void uninit();

	void updateLightsList();
	void updateModelsList();
	void initSceneLighting(GfxRenderer * gl, const Camera * curViewingCamera) const;

	unsigned char version() const {return 12;}

	void draw(GfxRenderer * gl) const;
	bool waterMark();
	
	void addElement(const SceneObjectSPtr & object);

	void insertBefore(const AppObjectSPtr & newObj, AppObject * refObj);

	const std::string & name() const {return name_;}
	void setName(const std::string & name) {name_ = name;}
	
	void start(float docTime, bool triggerEvent = true);
	void preStart(float docTime);
	bool update(float docTime);
	bool asyncLoadUpdate();
	
	bool isLoaded() const;

	bool isPlaying(Animation * animation) const;

	void stopEventListeners();
	void stop(bool triggerEvent = true);

	bool isInit() const {return init_;}

	bool handleEvent(Event * event);

	Root * root() const {return (Root *)root_.get();}

	const std::vector<AnimationSPtr> & animations() const 
	{return animations_;}

	SceneObjectSPtr findObject(SceneObject * obj) const;
	AppObjectSPtr findAppObject(const std::string & id)const;

	AppObjectSPtr findAppObject(AppObject * appObj) const;
	SceneObjectSPtr findObject(const std::string & id) const;
	SceneObjectSPtr findObject(
		const SceneObject * parent, const std::string & id) const;

	/**
	Is the scene currently showing? 
	Doesn't have to be the actual current template scene. For example, it could
	be contained in a subscene contained in the current template scene.
	*/
	bool isCurrentScene() const;

	SceneObjectSPtr findObject(
		const SceneObject * parent, SceneObject * obj) const;
	

	void addAnimation(const AnimationSPtr & animation);
	void deleteAnimation(Animation * animation);
	void setAnimations(const std::vector<AnimationSPtr> & animations);
	int animationIndex(Animation * animation) const;
	AnimationSPtr findAnimation(Animation * animation) const;
	AnimationSPtr findAnimation(const char * name) const;
	void insertBefore(const AnimationSPtr & newObj, Animation * refObj);

	int numTopLevelObjects() const;

	const Camera * camera() const;
	CameraObject * userCamera() const {return cameraObject_;}

	void setUserCamera(CameraObject * userCamera);	

	void cameraObjects(std::vector<CameraObject *> * cameraObjs) const;
	
	
	/**
	@param mousePos mouse cursor position in device coordinates (-1:1, -1:1)
	*/
	bool clickEvent(const Vector2 & mousePos, int pressId);
	bool doubleClickEvent(const Vector2 & mousePos, int pressId);
	bool pressEvent(const Vector2 & startPos, int pressId);
	bool moveEvent(const Vector2 & curPos, int pressId);
	bool releaseEvent(const Vector2 & curPos, int pressId);
	bool wasPressIntersected(int pressId) const;

	bool keyPressEvent(int keyCode);
	bool keyReleaseEvent(int keyCode);

	SceneObject * mouseOverObject() const;

	void play(Animation * animation, float time = 0.0f);
	void resume(Animation * animation);
	void stop(Animation * animation);

	const float & screenWidth() const {return screenWidth_;}
	const float & screenHeight() const {return screenHeight_;}	

	void setScreenWidth(const float & screenWidth);
	void setScreenHeight(const float & screenHeight);

	const bool & zBuffer() const {return zBuffer_;}
	
	void setZBuffer(const bool & val);

	const Color & bgColor() const {return bgColor_;}
	void setBgColor(const Color & bgColor) {bgColor_ = bgColor;}

	void allListeners(std::vector<EventListenerSPtr> * allListeners) const;

	void write(Writer & writer) const;
	void writeXml(XmlWriter & w) const;
	void read(Reader & reader, unsigned char);

	void readXml(XmlReader & r, xmlNode * node);

	Document * parentDocument() const {return parentDocument_;}
	void setParentDocument(Document * document) {parentDocument_ = document;}

	SceneObject * intersect(const Ray & ray, Vector3 * intPt) const;
	AppObject * intersectAppObject(const Ray & ray) const;

	bool broadcastMessage(const std::string & message);

	bool dependsOn(Scene * scene) const;
	void dependsOnScenes(std::vector<Scene *> * dependsOnScenes, bool recursive = true) const;
	bool isShowing(Scene * scene) const;
	void showingScenes(std::set<Scene *> * showingScenes) const;

	void initAppObjects();
	void uninitAppObjects();

	/**
	Normally, a Scene is uninited automatically when the number of inited 
	subscenes referencing it goes down to zero.
	if keepInit is true, the Scene will not be automatically uninited in such
	cases.
	*/
	void setKeepInit(bool keepInit);
	
	void getContainerObjects(ContainerObjects * containerObjects);

	SceneObject * curContainerObject() const
	{
		return curContainerObject_;
	}

	void setCurContainerObject(SceneObject * containerObject)
	{
		curContainerObject_ = containerObject;
	}

	void addObjRef(SceneReferencer * obj);
	void removeObjRef(SceneReferencer * obj);
	/**

	*/
	void handleInitBasedOnRefs(bool firstTimeInit = false);

	void getSceneObjectsByID(const boost::uuids::uuid & type, std::vector<SceneObject *> * objs) const;

	const bool & zoom() const {return zoom_;}
	void setZoom(const bool & val);

	const int & zoomMag() const {return zoomMagState_;}
	void setZoomMag(const int & val) {zoomMagState_ = val;}

	void playSyncedAnimations(SceneObject * obj);
	void stopSyncedAnimations(SceneObject * obj);
	void seekSyncedAnimations(SceneObject * obj, float time);
	void resumeSyncedAnimations(SceneObject * obj);	

	SceneObject * syncObject(Animation * animation) const;
	void setSyncObject(Animation * animation, SceneObject * syncObj);

	void removeSyncedAnimation(Animation * animation);
	void removeSyncObject(SceneObject * object);

	std::vector<Animation *> syncedAnimations(SceneObject * syncObj) const;

	/**
	Signify that the list of objects has changed	
	*/
	void flagObjectsListChanged();	
	
	/**
	Check if objectsListChanged flag is true, if so take appropriate measures,
	and reset the flag to false
	*/
	void handleObjectsListChanged();

	void resizeText(GfxRenderer * gl);

	const std::string & trackingUrl() const {return trackingUrl_;}
	void setTrackingUrl(const std::string & trackingUrl) {trackingUrl_ = trackingUrl;}

	void requestLocalSaveObject();
    
    const Vector3 & currentScale() const {return currentScale_;}
    void setCurrentScale(const Vector3 & var) {currentScale_ = var;}

	void setPauseTime(const float & time);

	const std::vector<ModelFile *> & models() const {return models_;}

	const float & lightIntensity() const {return lightIntensity_;}
	void setLightIntensity(const float & val) {lightIntensity_ = val;}

	const float & lightSpecularIntensity() const {return lightSpecularIntensity_;}
	void setLightSpecularIntensity(const float & val) {lightSpecularIntensity_ = val;}

	const float & lightAmbient() const {return lightAmbient_;}
	void setLightAmbient(const float & val) {lightAmbient_ = val;}

	Scene * rootScene();

	void textObjects(std::vector<Text *> * textObjs) const;
private:
	void readXmlAnimations(XmlReader & r, xmlNode * parentNode);
	void readXmlSyncedAnimations(XmlReader & r, xmlNode * parentNode);
	void readXmlRoot(XmlReader & r, xmlNode * parentNode);


	Scene(const Scene &);
	Scene & operator = (const Scene &);
	void create();

	bool handleEvent(SceneObject * obj, Event * event);

	void setSceneObjData(SceneObject * obj);

	void allListeners(SceneObject * obj, 
		std::vector<EventListenerSPtr> * allListeners) const;

	void setupNewObject(SceneObject * obj);

	/**
	private helper function for cameraObjects function
	*/
	void cameraObjects(
		SceneObject * root, std::vector<CameraObject *> * cameraObjs) const;
	
	void resetCamera(float aspectRatio);
	
	void updateObjsWithSyncedAnimations();
	void updateModelsList(SceneObject * sceneObject);

	void setSceneNum(const int & sceneNum);
	
	void textObjects(
		SceneObject * root, std::vector<Text *> * textObjs) const;
private:

	std::string name_;
	std::vector<AnimationSPtr> animations_;

	std::vector<AppObjectSPtr> appObjects_;
	SceneObjectSPtr root_;	

	/**
	Default "head-on" scene camera
	*/
	CameraSPtr camera_;
	
	/**
	user specified camera
	*/
	CameraObject * cameraObject_;
	
	//int nScenecnt_;
	
	float screenWidth_;
	float screenHeight_;
	bool zBuffer_;
	Color bgColor_;
	Document * parentDocument_;
	
	//-------------------------------------------------------------------------

	struct AnimationPlayData
	{
		AnimationPlayData(float a, Animation * b) 
		{startTime = a; animation = b;}

		float startTime;
		Animation * animation;
	};

	std::vector<AnimationPlayData> playingAnimations_;


	bool init_;
	std::set<SceneReferencer *> objRefs_;
	bool keepInit_;

	struct PressEventData
	{
		PressEventData() {}
		PressEventData(
			const Vector2 & pos, const std::vector<SceneObject *> & objects, bool intersected)
		{
			this->pos = pos;
			this->objects = objects;
			this->intersected = intersected;
		}
		Vector2 pos;
		std::vector<SceneObject *> objects;
		bool intersected;
	};

	typedef std::map<int, PressEventData> PressEventMap;
	PressEventMap pressEventMap_;

	/**
	If this scene is presented as a contained scene of some parent object, 
	a reference to that parent object should be stored here.
	*/
	SceneContainer * container_;
	
	JSObject * scriptObject_;

	SceneObject * curContainerObject_;

	bool zoom_;
	int zoomMagState_;	// 0: 2.0	1: 3.0	2: 4.0
	
	////////
	std::map<Animation *, SceneObject *> syncedAnimations_;
	std::multimap<SceneObject *, Animation *> objsWithSyncedAnimations_;

	/**
	List of lights in the scene
	*/
	std::vector<LightObject *> lights_;
	/**
	List of models in the scene
	*/
	std::vector<ModelFile *> models_;
	
	mutable bool objectsListChanged_;

	/**
	url to access at start of scene (eg for tracking purposes)
	*/
	std::string trackingUrl_;	

	/**
	To ensure that SceneStart and SceneStop events are fired in a 1-to-1 manner.
	*/
	int startCount_;
    
	Vector3 currentScale_;

	//default lighting
	float lightIntensity_;
	float lightSpecularIntensity_;
	float lightAmbient_;

	
	
	
#ifdef WATERMARK
	Texture * netntvTexture_;
	bool waterMark_;
#endif
};
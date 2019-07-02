#pragma once
#include "Types.h"
#include "MathStuff.h"
#include "BoundingBox.h"
#include "ScriptEventListener.h"
#include "AppObject.h"
#include "Attrib.h"

class GfxRenderer;

///////////////////////////////////////////////////////////////////////////////
class VisualAttrib;
class EventListener;
class Event;
class Group;
class MaskObject;
class Text;
class Text_2;
class Writer;
class XmlWriter;
class Reader;
class TempRenderObject;
class ElementMapping;
class PressEventData;
class ScriptProcessor;
struct JSObject;
class XmlReader;
class XmlWriter;

bool isSceneObject(ScriptProcessor * s, JSObject * jsObj);

class SceneObject
{
	friend class VisualAttrib;
	friend class Scene;
public:

	static JSObject * createBaseScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);
	static void gcMark(JSTracer * tracer, JSObject * obj);

	
	virtual const char * typeStr() const = 0;
	virtual boost::uuids::uuid type() const = 0;
	virtual SceneObject * clone(ElementMapping * elementMapping) const = 0;
	virtual void remapReferences(const ElementMapping & elementMapping);

	/**
	refFiles is added onto, and is not cleared.
	*/
	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	/**
	Caution: refFiles should all actually exist. Some objects might need to modify
	the new referenced files.
	@return end index (= to next object's start index)
	*/
	virtual int setReferencedFiles(
		const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int startIndex);

	SceneObject();
	virtual ~SceneObject();

	virtual Vector2 convertToChildSceneCoords(const Vector2 & coords) const {return coords;}
	virtual Vector2 convertFromChildSceneCoords(const Vector2 & coords) const {return coords;}

	SceneObject(const SceneObject & rhs, ElementMapping * elementMapping);
	
	/**
	Initialize GL objects/state. Note: the object state should remain the same
	across different calls to Init (only the gl state should be affected).
	In child class init functions SceneObject::init should be called first.

	@param firstTime set to true if this particular init call should be counted
	as a load for purposes of event handling (this prevents spurious loaded 
	events)
	*/
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	
	/**
	In child class uninit functions SceneObject::uninit should be called last.
	(Because it will nullify gl_).
	This function should be written in away that calling it multiple times in a
	row will be harmless / allowable. The subsequent calls after the first call
	should have no effect.

	Also this should be called first (as a virtual function) in child class 
	destructors.
	*/
	virtual void uninit();

	bool isInit() const {return gl_ != NULL;}

	Scene * parentScene() const {return parentScene_;}
	
	void setId(const std::string & stringId) {stringId_ = stringId;}
	const std::string & id() const {return stringId_;}

	SceneObjectSPtr removeChild(SceneObject * child);
	void addChild(const SceneObjectSPtr & child);
	void setAsSubObject(SceneObject * obj);
	void insertBefore(const SceneObjectSPtr & newObj, SceneObject * refObj);
	void insertAfter(const SceneObjectSPtr & newObj, SceneObject * refObj);

	SceneObjectSPtr deleteChild(SceneObject * child);
	void deleteChildren();
	const Transform & transform() const;
	void setTransform(const Transform & transform);
	
	virtual void draw(GfxRenderer * gl) const;

	///draw just this object (non recursive) don't worry about transforms etc.
	virtual void drawObject(GfxRenderer * gl) const {}
	
	/**
	@return whether the event was handled or not. If false, then any objects
	containing this object (not parent-child but for example: ArticleManager 
	containing SceneChanger) can handle the given event. Return true to block
	this event from reaching such containing objects.
	*/
	virtual bool clickEvent(const Vector2 & mousePos, int pressId);
	virtual bool doubleClickEvent(const Vector2 & mousePos, int pressId);
	virtual bool pressEvent(const Vector2 & startPos, int pressId);
	virtual bool releaseEvent(const Vector2 & pos, int pressId);
	virtual bool moveEvent(const Vector2 & pos, int pressId);	
	virtual bool receiveMessageEvent(const std::string & message);

	virtual bool handleEvent(Event * event, float sceneTime);

	virtual void start(float docTime);

	void stopEventListeners();

	/**
	implementation should be based on start
	*/
	virtual void preStart(float docTime);
	
	virtual void initAppObjects();
	virtual void uninitAppObjects();

	virtual bool update(float sceneTime);
	virtual bool asyncLoadUpdate();
	/**
	The return value of this is subtly different from the return value of 
	asyncLoadUpate().

	For example, in the case of SceneChanger/ImageChanger, asyncLoadUpdate()
	will return true even if the child scenes/images have not been loaded.
	This is because the SceneChanger/ImageChanger can display its own loading 
	progress indicator, so the top-level progress indicator shouldn't be shown.

	However isLoaded() only returns true if everything is indeed loaded.
	*/
	virtual bool isLoaded() const;
	
	virtual void stop();
	virtual void appObjectStop();

	virtual VisualAttrib * visualAttrib() {return 0;}
	virtual Group * group() {return 0;}
	virtual MaskObject * maskObject() {return 0;}
	virtual Text * text() {return 0;}
	SceneObject * parent() const {return parent_;}

	void addListener(const EventListenerSPtr & handler) 
	{eventListeners_.push_back(handler);}

	bool addScriptEventListener(const ScriptEventListener & handler);
	bool removeScriptEventListener(const ScriptEventListener & handler);

	EventListenerSPtr deleteListener(EventListener * listener);
	EventListenerSPtr findListener(EventListener * listener);

	const std::vector<EventListenerSPtr> & eventListeners() const
	{return eventListeners_;}

	void setEventListeners(const std::vector<EventListenerSPtr> & listeners);
	void insertBefore(const EventListenerSPtr & newObj, EventListener * refObj);

	const std::vector<SceneObjectSPtr> & children() const {return children_;}
	void setChildren(const std::vector<SceneObjectSPtr> & children);
	
	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	int numChildren() const {return children_.size();}

	Matrix parentTransform() const;
	float totalOpacity() const;
	///return the untransformed / local / original bounding box	
	virtual BoundingBox extents() const = 0;

	/**
	set the parent object and recursively set the appropriate parent scene as 
	well
	*/
	void setParent(SceneObject * parent);	

	bool isThisAndAncestorsVisible() const;

	BoundingBox worldExtents() const;

	void addTempRenderObject(GfxRenderer * gl, TempRenderObject * tempRenderObject);
	int numTempRenderObjects() const {return (int)tempRenderObjects_.size();}

	virtual bool dependsOn(Scene * scene) const;
	virtual void dependsOnScenes(std::vector<Scene *> * dependsOnScenes, bool recursive = true) const;

	virtual AppObjectOffset appObjectOffset() const {return AppObjectOffset();}

	virtual bool isShowing(Scene * scene) const;
	/**
	Doesn't clear showingScenes beforehand.
	*/
	virtual void showingScenes(std::set<Scene *> * showingScenes) const;

	virtual bool showPoint(Vector2 & outCoords, Scene * childScene, const Vector2 & childScenePoint) 
	{
		return false;
	}

	/**
	Cheat function to get the GfxRenderer for this object. 
	Don't use this if at all possible.
	For instance it will crash if there is no parent scene or not parent document
	*/
	GfxRenderer * renderer() const;


	virtual unsigned char version() const = 0;
	virtual void write(Writer & writer) const = 0;
	virtual const char * xmlTag() const;// {return "";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void read(Reader & reader, unsigned char) = 0;
	virtual void readXml(XmlReader & r, xmlNode * node);

	virtual void getSceneObjectsByID(
		const boost::uuids::uuid & type, std::vector<SceneObject *> * objs) const;

	virtual void setParentScene(Scene * parentScene);

	void handleLoadedEventDispatch(bool everythingLoaded);

	virtual void requestLocalSaveObject() {}

	virtual AppObjectOffset textFeaturesOffset() const {return AppObjectOffset();}

	virtual void setTextString(const std::wstring & text) {};
	virtual const std::wstring & textString() const {return L"";}
private:

	SceneObject(const SceneObject & rhs);
	SceneObject & operator = (const SceneObject & rhs);
	
	void create();	

	///total transform of object may have changed due to unparenting/parenting/etc
	void triggerOnSetTransform();

protected:
	std::string stringId_;

	std::vector<SceneObjectSPtr> children_;
	mutable std::vector<TempRenderObject *> tempRenderObjects_;
	std::vector<EventListenerSPtr> eventListeners_;
	
	std::vector<ScriptEventListener> scriptEventListeners_;
	
	SceneObject * parent_;	
	Scene * parentScene_;

	JSObject * scriptObject_;

	///The GfxRenderer that this object has been initialized with
	GfxRenderer * gl_;

	bool isLoadedDispatched_;
	static boost::uuids::string_generator sUuidGen_;
};





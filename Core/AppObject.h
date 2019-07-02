#pragma once
#include "Types.h"
#include "MathStuff.h"
#include "BoundingBox.h"
#include "ScriptEventListener.h"
class GfxRenderer;

///////////////////////////////////////////////////////////////////////////////
class VisualAttrib;
class Writer;
class XmlWriter;
class XmlReader;
class Reader;
class EventListener;
class ScriptProcessor;
struct JSObject;

class AppObjectOffset
{
public:
	//translation
	float dx, dy;
	//scale
	float sx, sy;

	//translation of clipping region
	//assume clipping region scale is the same as sx, sy
	//float cx, cy;

	//clip rect
	float clipX, clipY, clipWidth, clipHeight;
	bool clip;
	AppObjectOffset()
	{
		//cx = 0.0f;
		//cy = 0.0f;
		dx = 0.0f;
		dy = 0.0f;
		sx = 1.0f;
		sy = 1.0f;
		clip = false;
		clipX = 0.0f;
		clipY = 0.0f;
		clipWidth = 0.0f;
		clipHeight = 0.0f;
	}

	void addOffset(const AppObjectOffset & rhs);
};

struct AppObjectTransform
{
	int x;
	int y;
	int width;
	int height;
};

bool isAppObject(ScriptProcessor * s, JSObject * jsObj);

class AppObject
{
	friend class Scene;
public:	
	static JSObject * createBaseScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);
	static void gcMark(JSTracer * tracer, JSObject * obj);
	virtual const char * typeStr() const = 0;


	virtual boost::uuids::uuid type() const = 0;
	virtual AppObject * clone() const = 0;
	
	/**
	refFiles is added onto, and is not cleared.
	*/
	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	/**
	@return end index (= to next object's start index)
	*/
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int startIndex);

	AppObject();
	virtual ~AppObject();

	AppObject(const AppObject & rhs);	

	virtual void init();
	virtual void uninit();

	const int & x() const {return x_;}
	const int & y() const {return y_;}

	void setX(const int & x) {x_ = x;}
	void setY(const int & y) {y_ = y;}

	const int & width() const {return width_;}
	const int & height() const {return height_;}

	void setWidth(const int & width) {width_ = width;}
	void setHeight(const int & height) {height_ = height;}

	const bool & visible() const {return visible_;}
	void setVisible(const bool & visible) {visible_ = visible;}

	const float & opacity() const {return opacity_;}
	void setOpacity(const float & opacity) {opacity_ = opacity;}

	Scene * parentScene() const;
	const ContainerObjects & containerObjects() const {return containerObjects_;}
	
	void setId(const std::string & stringId) {stringId_ = stringId;}
	const std::string & id() const {return stringId_;}

	void setTransform(const AppObjectTransform & t)
	{
		x_ = t.x;
		y_ = t.y;
		width_ = t.width;
		height_ = t.height;
	}

	AppObjectTransform transform() const
	{
		AppObjectTransform ret;
		ret.x = x_;
		ret.y = y_;
		ret.width = width_;
		ret.height = height_;
		return ret;
	}

	virtual VisualAttrib * visualAttrib() {return 0;}

	virtual bool intersect(Vector3 * intPt, const Ray & ray) 
	{
		return false;
	}

	bool isAffectedByOffset() const {return affectedByOffset_;}
	void setAffectedByOffset(bool val) {affectedByOffset_ = val;}

	//void setParentScene(Scene * parentScene);
	void setContainerObjects(const ContainerObjects & containerObjects);

	bool handleEvents() const;
	void setHandleEvents(bool val) {handleEvents_ = val;}
	
	///return the untransformed / local / original bounding box	
	virtual BoundingBox extents() const = 0;
	BoundingBox worldExtents() const;

	virtual unsigned char version() const = 0;
	virtual void write(Writer & writer) const = 0;
	virtual const char * xmlTag() const {return "";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void read(Reader & reader, unsigned char) = 0;
	virtual void readXml(XmlReader & r, xmlNode * parent);

	virtual bool clickEvent(const Vector2 & mousePos, int pressId);

	virtual bool handleEvent(Event * event, float sceneTime);

	void addListener(const EventListenerSPtr & handler) 
	{eventListeners_.push_back(handler);}
	EventListenerSPtr deleteListener(EventListener * listener);
	EventListenerSPtr findListener(EventListener * listener);

	const std::vector<EventListenerSPtr> & eventListeners() const
	{return eventListeners_;}

	void setEventListeners(const std::vector<EventListenerSPtr> & listeners);


private:
	AppObject & operator = (const AppObject & rhs);

	
	void create();	

protected:
	std::string stringId_;	
	
	//Scene * parentScene_;
	/**
	objects (SceneChanger, SubScene) that contain this app object. NULL
	signifies that the app object belongs to the top level doc template

	parentObjects_[i] contains parentObjects_[i+1]
	parentObjects_[i_last] contains this appobject

	Only has meaning after the app object is initialized
	*/
	//AppObject * parent_;
	JSObject * scriptObject_;
	std::vector<ScriptEventListener> scriptEventListeners_;
	std::vector<AppObjectSPtr> children_;
	static boost::uuids::string_generator sUuidGen_;

	ContainerObjects containerObjects_;
	
	bool init_;
	bool affectedByOffset_;
	bool handleEvents_;

	float opacity_;
	bool visible_;

	int x_, y_, width_, height_;

	std::vector<EventListenerSPtr> eventListeners_;
};

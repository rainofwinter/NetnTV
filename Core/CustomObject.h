#pragma once
#include "SceneObject.h"
#include "Color.h"
#include "Attrib.h"
///////////////////////////////////////////////////////////////////////////////
class Mesh;
class Material;
class ScriptProcessor;
struct JSObject;

class CustomObjectData
{
	friend class CustomObject;
public:
	void setScript(const std::string & script) {script_ = script;}
	void setName(const std::string & name) {name_ = name;}
	unsigned char version() const {return 0;}
	void write(Writer & writer) const;
	void read(Reader & reader, unsigned char);
	void writeXml(XmlWriter & w) const;
	void readXml(XmlReader & r, xmlNode * node);
private:
	///utf8
	std::string script_; 
	std::string name_;
};

///////////////////////////////////////////////////////////////////////////////

class CustomObject : public SceneObject
{
public:

	virtual const char * typeStr() const 
	{
		return "CustomObject";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = 
			sUuidGen_("14ED2222-3AB7-2799-1717-82D5364987AF");
		return uuid;
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{
		return new CustomObject(*this, elementMapping);
	}


	CustomObject();	
	CustomObject(const CustomObject & rhs, ElementMapping * elementMapping);
	
	~CustomObject();

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}


	virtual BoundingBox extents() const;

	virtual void remapReferences(const ElementMapping & elementMapping);
	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(
		const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int startIndex);

	virtual bool clickEvent(const Vector2 & mousePos, int pressId);
	virtual bool doubleClickEvent(const Vector2 & mousePos, int pressId);
	virtual bool pressEvent(const Vector2 & startPos, int pressId);
	virtual bool releaseEvent(const Vector2 & pos, int pressId);
	virtual bool moveEvent(const Vector2 & pos, int pressId);	
	virtual bool receiveMessageEvent(const std::string & message);

	virtual void setParentScene(Scene * parentScene);
	
	virtual bool handleEvent(Event * event, float sceneTime);

	virtual void start(float docTime);
	virtual void preStart(float docTime);
	
	virtual void initAppObjects();
	virtual void uninitAppObjects();

	virtual bool update(float sceneTime);
	virtual bool asyncLoadUpdate();

	virtual bool dependsOn(Scene * scene) const;
	virtual void dependsOnScenes(std::vector<Scene *> * dependsOnScenes, bool recursive = true) const;

	virtual AppObjectOffset appObjectOffset() const;
	virtual bool isShowing(Scene * scene) const;

	virtual void showingScenes(std::set<Scene *> * showingScenes) const;


	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "CustomObject";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	const std::string & data() const {return data_;}
	void setData(const std::string & data);

	const boost::uuids::uuid & customType() const {return customObjectId_;}
	void setCustomType(const boost::uuids::uuid & id) {customObjectId_ = id;}
private:
	void create();
	CustomObject(const CustomObject & rhs);
	CustomObject & operator = (const CustomObject & rhs);

	ScriptProcessor * getScriptProcessor() const;

private:
	VisualAttrib visualAttrib_;
	boost::uuids::uuid customObjectId_;
	std::string data_;
	bool isLoadedSinceInit_;
	/**
	current js global object.
	Query whether script init has occured by testing if this is NULL or not
	*/
	JSObject * global_;

	boost::scoped_ptr<SceneObject> rootObject_;
};

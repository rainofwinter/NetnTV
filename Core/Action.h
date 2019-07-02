#pragma once
class Document;
class SceneObject;
class Writer;
class Reader;
class Animation;
class Scene;
class ElementMapping;
class AppObject;
class XmlReader;
class XmlWriter;

class Action
{

public:
	Action() {targetObject_ = 0;}
	virtual boost::uuids::uuid type() const = 0;
	virtual const char * typeStr() const = 0;	
	virtual ~Action() {}
	virtual Action * clone() const = 0;

	/**
	This is mostly to maintain backwards compatibility with older content.
	@return true if this action needs to be reset before SceneStart so that it
	will appear correctly during the period of time between when a page first
	becomes visible and it becomes fully visible (thus triggering a SceneStart 
	event)
	*/
	virtual bool needsScenePreStart() const {return false;}

	/**
	@return false if remapping failed
	*/
	virtual bool remapReferences(const ElementMapping & mapping);

	virtual void referencedFiles(std::vector<std::string> * refFiles) const {}
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index) {return index;}

	virtual Action & equals(const Action & rhs) = 0;

	/**
	make sure to return scene state to an acceptable state in the event that 
	this action gets interrupted
	*/
	virtual void stop() {}

	/**
	Note: obj can be null in case of Scene Root
	*/
	virtual bool supportsObject(SceneObject * obj) const
	{
		return true;
	}
	
	/**
	There are two cases to think about: 
	The Action's scene is the current scene
	The Action's scene is not the current scene
	
	@param object the object that originated the event for this action. Will
	never be null. In case of Scene Root will be the actual Root object

	@return need to redraw
	*/
	virtual bool start(SceneObject * object, float time) = 0;

	virtual bool startAppObj(AppObject * object, float time) {return true;}
	virtual bool updateAppObj(AppObject * object, float time) {return true;}
	bool scenePreStartAppObj(AppObject * object, float time);
	
	/**
	Reset this action before SceneStart if necessary
	@return need to redraw
	*/
	bool scenePreStart(SceneObject * object, float time);

	virtual bool hasDuration() const = 0;
	virtual bool isExecuting() const = 0;
	virtual bool update(SceneObject * object, float time) = 0;	

	virtual void setTargetObject(SceneObject * obj) {targetObject_ = obj;}
	virtual void setDocument(Document * document) {}
	SceneObject * targetObject() const {return targetObject_;}

	virtual bool dependsOn(Animation * animation) const = 0;
	virtual bool dependsOn(Scene * scene) const = 0;

	/**
	return true if this action depends on the existence of the specified object
	*/
	virtual bool dependsOn(SceneObject * obj) const;

	/**
	return true if this action depends on there being a parent-child type relationship 
	between the two specified objects
	*/
	virtual bool dependsOn(
		SceneObject * parent, SceneObject * child) const 
	{
		return false;
	}


	virtual unsigned char version() const = 0;
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);
	
protected:
	SceneObject * targetObject_;

};


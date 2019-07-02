#pragma once
#include "MathStuff.h"
#include "Action.h"
#include "Transform.h"

///////////////////////////////////////////////////////////////////////////////
class ImageAnimationPlayAction : public Action
{
public:
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("bc8c1600-443c-11e2-a25f-0800200c9a66");
		return uuid;
	}

	virtual const char * typeStr() const
	{
		return "Play ImageAnimation";
	}

	virtual bool needsScenePreStart() const {return true;}

	ImageAnimationPlayAction();
	~ImageAnimationPlayAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new ImageAnimationPlayAction(*this);}	
	virtual Action & equals(const Action & rhs);

	/**
	obj may be null, which signifies "no target object."
	*/
	virtual bool supportsObject(SceneObject * obj) const;

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const {return false;}
	virtual bool isExecuting() const {return false;}
	virtual bool update(SceneObject * object, float time) {return false;}

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const;

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "ImageAnimationPlay";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
	
private:
};
///////////////////////////////////////////////////////////////////////////////

class ImageAnimationStopAction : public Action
{
public:
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("88c6f410-443d-11e2-a25f-0800200c9a66");
		return uuid;
	}

	virtual const char * typeStr() const
	{
		return "Stop ImageAnimation";
	}

	virtual bool needsScenePreStart() const {return true;}

	ImageAnimationStopAction();
	~ImageAnimationStopAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new ImageAnimationStopAction(*this);}	
	virtual Action & equals(const Action & rhs);

	/**
	obj may be null, which signifies "no target object."
	*/
	virtual bool supportsObject(SceneObject * obj) const;

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const {return false;}
	virtual bool isExecuting() const {return false;}
	virtual bool update(SceneObject * object, float time) {return false;}

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const;

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "ImageAnimationStop";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
	
private:
};




///////////////////////////////////////////////////////////////////////////////

class ImageAnimationResetAction : public Action
{
public:
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("bbb6f410-443d-11e2-b2bf-08b0200b9bbb");
		return uuid;
	}

	virtual const char * typeStr() const
	{
		return "Reset ImageAnimation";
	}

	virtual bool needsScenePreStart() const {return true;}

	ImageAnimationResetAction();
	~ImageAnimationResetAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new ImageAnimationResetAction(*this);}	
	virtual Action & equals(const Action & rhs);

	/**
	obj may be null, which signifies "no target object."
	*/
	virtual bool supportsObject(SceneObject * obj) const;

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const {return false;}
	virtual bool isExecuting() const {return false;}
	virtual bool update(SceneObject * object, float time) {return false;}

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const;

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "ImageAnimationReset";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
	
private:
};
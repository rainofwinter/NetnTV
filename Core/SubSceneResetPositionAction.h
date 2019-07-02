#pragma once
#include "MathStuff.h"
#include "Action.h"
#include "Transform.h"

class Animation;
class Document;
class SubsceneResetPositionAction : public Action
{
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("21022131-2337-5222-9021-1945E6ABE107");
	}

	virtual const char * typeStr() const
	{
		return "Reset Position";
	}

	virtual bool needsScenePreStart() const {return true;}

	SubsceneResetPositionAction();
	~SubsceneResetPositionAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new SubsceneResetPositionAction(*this);}	
	virtual Action & equals(const Action & rhs);
	/**
	obj may be null, which signifies "no target object."
	*/
	virtual bool supportsObject(SceneObject * obj) const;

	virtual bool remapReferences(const ElementMapping & mapping)
	{return Action::remapReferences(mapping);}

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);	

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const;

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "SubSceneResetPosition";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
private:

};


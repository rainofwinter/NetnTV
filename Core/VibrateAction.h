#pragma once
#include "Action.h"

class Animation;

class VibrateAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("ac9c5c80-600c-11e4-9803-0800200c9a66");
	}

	virtual const char * typeStr() const
	{
		return "Vibrate Action";
	}

	VibrateAction();
	~VibrateAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new VibrateAction(*this);}
	virtual bool remapReferences(const ElementMapping & mapping)
	{return Action::remapReferences(mapping);}

	virtual Action & equals(const Action & rhs);

	virtual bool supportsObject(SceneObject * obj) const
	{
		return obj == 0;
	}

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);	

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * object) const {return false;}

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "VibrateAction";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);

	void setVibrateTime(int time) { time_ = time; }
	int vibrateTime() { return time_; }
private:
	int time_;
};

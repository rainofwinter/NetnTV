#pragma once
#include "Action.h"

class Animation;

class LaunchAppAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("812308DD-114C-500B-0AD5-B17AC68BE307");
	}

	virtual const char * typeStr() const
	{
		return "Launch App";
	}

	LaunchAppAction();
	~LaunchAppAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new LaunchAppAction(*this);}
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


	void setIosParams(const std::string & params) {iosParams_ = params;}
	const std::string & iosParams() const {return iosParams_;}

	void setAndroidParams(const std::string & params) {androidParams_ = params;}
	const std::string & androidParams() const {return androidParams_;}

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * object) const {return false;}

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "LaunchApp";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
private:
	std::string iosParams_;  
	std::string androidParams_;
};

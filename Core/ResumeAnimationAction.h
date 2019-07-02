#pragma once
#include "Action.h"

class Animation;

class ResumeAnimationAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("60ACDCF3-B028-8C5F-EE76-CDAB80ABC440");
	}

	virtual const char * typeStr() const
	{
		return "Resume animation";
	}

	ResumeAnimationAction();
	~ResumeAnimationAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new ResumeAnimationAction(*this);}
	virtual bool remapReferences(const ElementMapping & mapping);
	virtual Action & equals(const Action & rhs);

	virtual bool supportsObject(SceneObject * obj) const
	{
		return obj == 0;
	}

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);	

	void setAnimation(Animation * animation) {animation_ = animation;}
	Animation * animation() const {return animation_;}

	virtual bool dependsOn(Animation * animation) const 
	{
		return animation == animation_;
	}

	virtual bool dependsOn(Scene * scene) const {return false;}

	virtual bool dependsOn(SceneObject * obj) const {return false;}

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "ResumeAnimation";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
private:
	Animation * animation_;
};

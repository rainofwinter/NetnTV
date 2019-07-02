#ifndef StopAnimationAction_h__
#define StopAnimationAction_h__

#include "Action.h"

class Animation;

class StopAnimationAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("E37EEBCD-D14C-562B-9775-A7BAE68BE3A7");
	}

	virtual const char * typeStr() const
	{
		return "Stop animation";
	}

	StopAnimationAction();
	~StopAnimationAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new StopAnimationAction(*this);}
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

	virtual const char * xmlTag() const {return "StopAnimation";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
private:
	Animation * animation_;
};



#endif // StopAnimationAction_h__
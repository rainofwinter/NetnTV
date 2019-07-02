#ifndef WaitUntilAction_h__
#define WaitUntilAction_h__

#include "Action.h"

class Animation;

class WaitUntilAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("8210D0FD-D14C-5D22-9775-214AE68BE107");
	}

	virtual const char * typeStr() const
	{
		return "Wait until";
	}

	WaitUntilAction();
	~WaitUntilAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new WaitUntilAction(*this);}
	virtual bool remapReferences(const ElementMapping & mapping)
	{return Action::remapReferences(mapping);}

	virtual Action & equals(const Action & rhs);

	virtual bool supportsObject(SceneObject * obj) const;

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);	

	float waitUntilTime() const {return waitUntilTime_;}
	void setWaitUntilTime(float time) {waitUntilTime_ = time;}
	float delayedTime() const {return delayedTime_;}
	void setDelayedTime(float time) {/*if (isExecuting_)*/ delayedTime_ = time;}

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const 
	{return false;}

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "WaitUntil";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
	bool isExecuting(){return isExecuting_;}
private:
	float waitUntilTime_;
	float delayedTime_;

	bool isExecuting_;
};



#endif // WaitUntilAction_h__
#ifndef ResetAnimationAction_h__
#define ResetAnimationAction_h__

#include "Action.h"

class Animation;

class ResetAnimationAction : public Action
{	
public:
	enum ResetType
	{
		Any,
		All
	};
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("837EABCD-D14C-500B-9775-A17AE68BE307");
	}

	virtual const char * typeStr() const
	{
		return "Reset animation";
	}

	virtual bool needsScenePreStart() const 
	{
		return true;
	}

	ResetAnimationAction();
	~ResetAnimationAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new ResetAnimationAction(*this);}
	virtual bool remapReferences(const ElementMapping & mapping);

	virtual Action & equals(const Action & rhs);

	virtual bool supportsObject(SceneObject * obj) const
	{
		return obj == 0;
	}

	const ResetType & resetType() const 
	{return resetType_;}

	void setResetType(const ResetType & resetType) 
	{resetType_ = resetType;}

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

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "ResetAnimation";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
private:
	Animation * animation_;
	ResetType resetType_;
};



#endif // ResetAnimationAction_h__
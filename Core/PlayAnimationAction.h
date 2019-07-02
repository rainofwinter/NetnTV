#ifndef PlayAnimationAction_h__
#define PlayAnimationAction_h__

#include "Action.h"

class Animation;

class PlayAnimationAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("8370D0FD-D14C-500B-9775-A17AE68BE307");
	}

	virtual const char * typeStr() const
	{
		return "Play animation";
	}

	virtual bool needsScenePreStart() const {return true;}

	PlayAnimationAction();
	~PlayAnimationAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new PlayAnimationAction(*this);}
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

	virtual const char * xmlTag() const {return "PlayAnimation";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
private:
	Animation * animation_;
};



#endif // PlayAnimationAction_h__
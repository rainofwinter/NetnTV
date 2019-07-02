#pragma once
#include "Action.h"
#include "ModelAnimation.h"

class ModelFile;

class ModelPlayAnimationAction : public Action
{
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("601B2A33-215C-52D2-80E1-1945E1ABE147");
	}

	virtual const char * typeStr() const
	{
		return "Play Animation";
	}

	ModelPlayAnimationAction();
	~ModelPlayAnimationAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new ModelPlayAnimationAction(*this);}	
	virtual Action & equals(const Action & rhs);

	/**
	obj may be null, which signifies "no target object."
	*/
	virtual bool supportsObject(SceneObject * obj) const;

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);	

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const;

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "ModelPlayAnimation";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);

	void setModelAnimation(const ModelAnimation & anim) {modelAnimation_ = anim;}
	const ModelAnimation & modelAnimation() const {return modelAnimation_;}
private:
	ModelFile * modelFile_;
	ModelAnimation modelAnimation_;
};


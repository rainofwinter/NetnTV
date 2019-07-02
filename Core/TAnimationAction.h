#pragma once
#include "MathStuff.h"
#include "Action.h"
#include "Transform.h"

///////////////////////////////////////////////////////////////////////////////
class TAnimationPlayAction : public Action
{
public:
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("bc9c2666-443c-11e2-a25f-0812299c9add");
		return uuid;
	}

	virtual const char * typeStr() const
	{
		return "Play TAnimation";
	}

	virtual bool needsScenePreStart() const {return true;}

	TAnimationPlayAction();
	~TAnimationPlayAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new TAnimationPlayAction(*this);}	
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
	
private:
};
///////////////////////////////////////////////////////////////////////////////

class TAnimationStopAction : public Action
{
public:
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("9906f421-443d-11e2-a25f-08112aac9a66");
		return uuid;
	}

	virtual const char * typeStr() const
	{
		return "Stop TAnimation";
	}

	virtual bool needsScenePreStart() const {return true;}

	TAnimationStopAction();
	~TAnimationStopAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new TAnimationStopAction(*this);}	
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
	
private:
};




///////////////////////////////////////////////////////////////////////////////

class TAnimationResetAction : public Action
{
public:
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("ccc6f410-443d-13e2-b2bf-08b0200b9ddd");
		return uuid;
	}

	virtual const char * typeStr() const
	{
		return "Reset TAnimation";
	}

	virtual bool needsScenePreStart() const {return true;}

	TAnimationResetAction();
	~TAnimationResetAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new TAnimationResetAction(*this);}	
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
	
private:
};
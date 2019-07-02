#ifndef ShowOnlyAction_h__
#define ShowOnlyAction_h__

#include "Action.h"
#include "SceneObject.h"

class Animation;

class ShowOnlyAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("83FAD0FD-D14C-5DAG-9775-A17AE68BE107");
	}

	virtual const char * typeStr() const
	{
		return "Show only";
	}

	virtual bool needsScenePreStart() const {return true;}

	ShowOnlyAction();
	~ShowOnlyAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new ShowOnlyAction(*this);}
	virtual bool remapReferences(const ElementMapping & mapping);
	virtual Action & equals(const Action & rhs);

	virtual bool supportsObject(SceneObject * obj) const;

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);	

	void setShowObject(SceneObject * object) {showObject_ = object;}
	SceneObject * showObject() const {return showObject_;}

	virtual bool dependsOn(Animation * animation) const 
	{
		return false;
	}

	virtual bool dependsOn(Scene * scene) const 
	{
		return false;
	}
	virtual bool dependsOn(SceneObject * obj) const;
	virtual bool dependsOn(SceneObject * parent, SceneObject * child) const;

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "ShowOnly";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
private:
	SceneObject * showObject_;
};



#endif // ShowOnlyAction_h__
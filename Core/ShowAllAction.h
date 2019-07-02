#ifndef ShowAllAction_h__
#define ShowAllAction_h__

#include "Action.h"
#include "SceneObject.h"

class Animation;

class ShowAllAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("83ABC0FD-D14C-5DAG-9775-A17AE68BE321");
	}

	virtual const char * typeStr() const
	{
		return "Show/Hide All";
	}

	virtual bool needsScenePreStart() const {return true;}

	const bool & visible() const {return visible_;}
	void setVisible(const bool & visible) {visible_ = visible;}

	ShowAllAction();
	~ShowAllAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new ShowAllAction(*this);}	
	virtual bool remapReferences(const ElementMapping & mapping)
	{return Action::remapReferences(mapping);}

	virtual Action & equals(const Action & rhs);

	virtual bool supportsObject(SceneObject * obj) const;

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);
	

	virtual bool dependsOn(Animation * animation) const 
	{
		return false;
	}

	virtual bool dependsOn(Scene * scene) const 
	{
		return false;
	}
	virtual bool dependsOn(SceneObject * obj) const 
	{
		return Action::dependsOn(obj);
	}

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "PlayAnimation";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
private:
	bool visible_;
};



#endif // ShowAllAction_h__
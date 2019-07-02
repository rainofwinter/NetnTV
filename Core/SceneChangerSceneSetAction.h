#ifndef SceneChangerSceneSetAction_h__
#define SceneChangerSceneSetAction_h__
#include "MathStuff.h"
#include "Action.h"
#include "Transform.h"

class Animation;
class Document;
class SceneChangerSceneSetAction : public Action
{
public:
	enum ChangeMode
	{
		Specific,
		Adjacent
	};

	enum Direction
	{
		Previous,
		Next
	};
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("20121231-2157-5222-9021-1945E6ABE107");
	}

	virtual const char * typeStr() const
	{
		return "Scene Set";
	}

	virtual bool needsScenePreStart() const {return true;}

	SceneChangerSceneSetAction();
	~SceneChangerSceneSetAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new SceneChangerSceneSetAction(*this);}	
	virtual Action & equals(const Action & rhs);	
	virtual bool remapReferences(const ElementMapping & mapping);

	/**
	obj may be null, which signifies "no target object."
	*/
	virtual bool supportsObject(SceneObject * obj) const;

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);	

	Scene * scene() const {return scene_;}
	void setScene(Scene * scene) {scene_ = scene;}

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const;
	virtual bool dependsOn(SceneObject * obj) const;

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "SceneChangerSceneSet";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);

	const ChangeMode & mode() const {return mode_;}
	void setMode(const ChangeMode & mode) {mode_ = mode;}
	const Direction & direction() const {return direction_;}
	void setDirection(const Direction & direction) {direction_ = direction;}
private:
	ChangeMode mode_;
	Direction direction_;

	Scene * scene_;
};



#endif // SceneChangerSceneSetAction_h__
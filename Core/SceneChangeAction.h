#ifndef SceneChangeAction_h__
#define SceneChangeAction_h__
#include "MathStuff.h"
#include "Action.h"
#include "Transform.h"

class Animation;
class Document;
class SceneChangeAction : public Action
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
		return gen("1410D0FD-FFFF-5222-9021-014AE6ABE107");
	}

	virtual const char * typeStr() const
	{
		return "Scene Change";
	}

	SceneChangeAction();
	~SceneChangeAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new SceneChangeAction(*this);}	
	virtual Action & equals(const Action & rhs);	
	virtual bool remapReferences(const ElementMapping & mapping);

	virtual bool supportsObject(SceneObject * obj) const;

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);	

	Scene * scene() const {return scene_;}
	void setScene(Scene * scene) {scene_ = scene;}

	Document * document() const {return document_;}
	virtual void setDocument(Document * document) 
	{
		document_ = document;
	}

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const;
	virtual bool dependsOn(SceneObject * obj) const;

	virtual unsigned char version() const {return 3;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "SceneChange";}
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
	Document * document_;
};



#endif // SceneChangeAction_h__
#ifndef SetOpacityAction_h__
#define SetOpacityAction_h__

#include "Action.h"

class Animation;

class SetOpacityAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("8228874D-D14C-500B-9775-777AE737E307");
	}

	virtual const char * typeStr() const
	{
		return "Set opacity";
	}

	virtual bool needsScenePreStart() const {return true;}

	SetOpacityAction();
	~SetOpacityAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new SetOpacityAction(*this);}
	virtual bool remapReferences(const ElementMapping & mapping)
	{return Action::remapReferences(mapping);}
	
	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	virtual Action & equals(const Action & rhs);

	virtual bool supportsObject(SceneObject * obj) const;

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);	

	void setOpacity(const float & opacity) {opacity_ = opacity;}
	const float & opacity() const {return opacity_;}

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const 
	{
		return Action::dependsOn(obj);
	}

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "SetOpacity";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);
private:
	float opacity_;
};



#endif // SetOpacityAction_h__
#ifndef SetVisibilityAction_h__
#define SetVisibilityAction_h__

#include "Action.h"

class XmlReader;
class XmlWriter;

class SetVisibilityAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("8228874D-D14C-500B-9775-747747747747");
	}

	virtual const char * typeStr() const
	{
		return "Set visibility";
	}

	virtual bool needsScenePreStart() const {return true;}

	SetVisibilityAction();
	~SetVisibilityAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new SetVisibilityAction(*this);}
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

	void setVisibility(const bool & visibility) {visibility_ = visibility;}
	const bool & visibility() const {return visibility_;}

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const 
	{
		return Action::dependsOn(obj);
	}

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "SetVisibility";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);
private:
	bool visibility_;
};



#endif // SetVisibilityAction_h__
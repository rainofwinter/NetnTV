#ifndef ImageAnimationPauseAction_h__
#define ImageAnimationPauseAction_h__
#include "Action.h"

class ImageAnimationPauseAction : public Action
{
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("7e414d7e-c29c-44ef-a7c9-7b51f40780f3");
	}

	virtual const char * typeStr() const
	{
		return "Pause ImageAnimation";
	}
	
	ImageAnimationPauseAction();
	~ImageAnimationPauseAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new ImageAnimationPauseAction(*this);}	
	virtual Action & equals(const Action & rhs);	
	virtual bool remapReferences(const ElementMapping & mapping)
	{return Action::remapReferences(mapping);}

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

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
	//virtual bool dependsOn(SceneObject * obj) const;

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "ImageAnimationPause";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
};



#endif // ImageAnimationPauseAction_h__
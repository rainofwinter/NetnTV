#ifndef ImageSetFileAction_h__
#define ImageSetFileAction_h__

#include "Action.h"

class Animation;

class ImageSetFileAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("8992874D-D14C-500B-9775-A17AE68BE307");
	}

	virtual const char * typeStr() const
	{
		return "Set file";
	}

	virtual bool needsScenePreStart() const {return true;}

	ImageSetFileAction();
	~ImageSetFileAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new ImageSetFileAction(*this);}
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

	void setFileName(const std::string & fileName) {fileName_ = fileName;}
	const std::string & fileName() const {return fileName_;}

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const {return false;}

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "ImageSetFile";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
private:
	std::string fileName_;
};



#endif // ImageSetFileAction_h__
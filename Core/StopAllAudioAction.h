#ifndef StopAllAudioAction_h__
#define StopAllAudioAction_h__

#include "Action.h"

class Animation;

class StopAllAudioAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("8223104D-D14C-500B-9AAA-CCCCE68BEAAA");
	}

	virtual const char * typeStr() const
	{
		return "Stop all audio";
	}

	StopAllAudioAction();
	~StopAllAudioAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new StopAllAudioAction(*this);}
	virtual bool remapReferences(const ElementMapping & mapping)
	{return Action::remapReferences(mapping);}
	
	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	virtual Action & equals(const Action & rhs);

	virtual bool supportsObject(SceneObject * obj) const
	{
		return obj == 0;
	}

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);		
	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	//virtual bool dependsOn(SceneObject * obj) const {return Action::dependsOn(obj);}


	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);	
	
	virtual const char * xmlTag() const {return "StopAllAudio";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
private:

};



#endif // StopAllAudioAction_h__
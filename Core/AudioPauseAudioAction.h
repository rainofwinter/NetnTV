#pragma once
#include "Action.h"

class Animation;

class AudioPauseAudioAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("b583c688-0c70-4542-8f3c-6280c842bbb1");
		return uuid;		 
	}

	virtual const char * typeStr() const
	{
		return "Pause audio";
	}

	AudioPauseAudioAction();
	~AudioPauseAudioAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new AudioPauseAudioAction(*this);}
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
	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	//virtual bool dependsOn(SceneObject * obj) const {return Action::dependsOn(obj);}

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "AudioPauseAudio";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);

private:

};


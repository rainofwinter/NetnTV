#pragma once
#include "Action.h"
#include "SourceType.h"


class Animation;

class AudioPlayAudioAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = 
			boost::uuids::string_generator()("89182341-ABDC-CA0B-F7A5-B17EED8BE3F7");
		return uuid;		
	}

	virtual const char * typeStr() const
	{
		return "Play audio";
	}

	AudioPlayAudioAction();
	~AudioPlayAudioAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new AudioPlayAudioAction(*this);}
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


	virtual unsigned char version() const {return 2;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "AudioPlayAudio";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);

	const float & startTime() const {return startTime_;}
	void setStartTime(const float & startTime) {startTime_ = startTime;}

private:
	//bool repeat_;
	float startTime_;
};

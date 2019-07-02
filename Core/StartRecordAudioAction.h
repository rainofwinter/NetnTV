#pragma once

#include "Action.h"

class StartRecordAudioAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("4194fce2-2a2d-49c1-8d30-ca27a132a1c2");
	}

	virtual const char * typeStr() const
	{
		return "Start record audio";
	}

	StartRecordAudioAction();
	~StartRecordAudioAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new StartRecordAudioAction(*this);}
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
	//virtual bool dependsOn(SceneObject * obj) const	{return Action::dependsOn(obj);}


	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "StartRecordAudio";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
private:
};



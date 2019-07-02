#pragma once
#include "Action.h"


class VideoPlayVideoAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = 
			boost::uuids::string_generator()("20c9c396-9679-4122-8135-3d0616d3c054");
		return uuid;		
	}

	virtual const char * typeStr() const
	{
		return "Play";
	}

	VideoPlayVideoAction();
	~VideoPlayVideoAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new VideoPlayVideoAction(*this);}
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

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "VideoPlayVideo";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);

	const float & startTime() const {return startTime_;}
	void setStartTime(const float & startTime) {startTime_ = startTime;}

private:

	float startTime_;
};

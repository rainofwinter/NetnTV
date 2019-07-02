#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "Texture.h"
///////////////////////////////////////////////////////////////////////////////
class Camera;


class AudioObject : public SceneObject
{
public:
	enum SourceType
	{
		File,
		Recorded
	};
public:
	
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);
	

	virtual const char * typeStr() const 
	{
		return "Audio";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = 
			boost::uuids::string_generator()("99DAAF12-BB32-C199-CC12-26A53DD384AA");
		return uuid;		
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new AudioObject(*this, elementMapping);}

	bool update(float sceneTime);

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	virtual bool asyncLoadUpdate();
	virtual bool isLoaded() const;

	AudioObject();
	AudioObject(const AudioObject & rhs, ElementMapping * elementMapping);

	~AudioObject();

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 3;}
	
	virtual void write(Writer & writer) const;

	virtual const char * xmlTag() const {return "Audio";}
	virtual void writeXml(XmlWriter & w) const;

	virtual void read(Reader & reader, unsigned char);
	virtual void readXml(XmlReader & r, xmlNode * node);

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	virtual VisualAttrib * visualAttrib() {return NULL;}

	const bool & repeat() const {return repeat_;}
	void setRepeat(const bool & repeat) {repeat_ = repeat;}

	const std::string & fileName() const {return fileName_;}
	void setFileName(const std::string & fileName);

	const SourceType & sourceType() const 
	{
		return sourceType_;
	}

	void setSourceType(const SourceType & sourceType) 
	{
		sourceType_ = sourceType;
	}
	
	float volume() const {return volume_;}
	void setVolume(float volume);

	void play(float startTime);
	void stop();
	void pause();
	void resume();

	void notifyStopped();
	void notifyPlayed();
	void notifyPaused();
	void notifySeeked(float time);
	void notifyResumed();
	
	float currentTime();
private:
	AudioObject(const AudioObject & rhs);
	AudioObject & operator = (const AudioObject & rhs);
	void create();	

private:
	std::string fileName_;
	bool isPlaying_;
	std::set<float> firedPlayEventTimes_;
	bool repeat_;

	SourceType sourceType_;
	float volume_;
	float currnetTime_;
	float playTime_;
};
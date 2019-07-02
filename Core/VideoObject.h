#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "Texture.h"
///////////////////////////////////////////////////////////////////////////////
class Camera;


class VideoObject : public SceneObject
{
	friend class VideoPlayVideoAction;
public:
	enum SourceType
	{
		File = 0,
		Url
	};

public:
	
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);	

	virtual const char * typeStr() const 
	{
		return "Video";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = 
			boost::uuids::string_generator()("f048340e-318b-4f2c-b74f-dbf625effb56");
		return uuid;		
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new VideoObject(*this, elementMapping);}

	bool update(float sceneTime);

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);


	virtual bool asyncLoadUpdate();
	virtual bool isLoaded() const;

	VideoObject();
	VideoObject(const VideoObject & rhs, ElementMapping * elementMapping);

	~VideoObject();

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "Video";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	virtual VisualAttrib * visualAttrib() {return NULL;}

	const std::string & source() const {return source_;}
	const std::vector<std::string> & sourceNames() const {return sourceName_;}
	const std::vector<int> & sourceMode() const {return sourceMode_;}
	void setSource(const std::string & source);
	void setSourceNames(const std::vector<std::string> & sourceNames);
	void setSourceMode(const std::vector<int> & sourceMode);

	const SourceType & sourceType() const {return sourceType_;}
	void setSourceType(const SourceType & sourceType) {sourceType_ = sourceType;}

	const bool & playInRegion() const {return playInRegion_;}
	void setPlayInRegion(const bool & val) {playInRegion_ = val;}

	const int & x() const {return x_;}
	void setX(const int & x) {x_ = x;}

	const int & y() const {return y_;}
	void setY(const int & y) {y_ = y;}
	
	const int & width() const {return width_;}
	void setWidth(const int & width) {width_ = width;}
	
	const int & height() const {return height_;}
	void setHeight(const int & height) {height_ = height;}

	const bool & showPlaybackControls() const {return showPlaybackControls_;}
	void setShowPlaybackControls(const bool & val) {showPlaybackControls_ = val;}

	void play(float startTime);
	void stop();

	void notifyStopped();
	void notifyPlayed();
	void notifyPaused();
	void notifyResumed();
	void notifySeeked(float time);

	float currentTime();

	bool isPlaying() {return isPlaying_;}

private:
	VideoObject(const VideoObject & rhs);
	VideoObject & operator = (const VideoObject & rhs);
	void create();	

private:
	std::string source_;
	bool isPlaying_;

	SourceType sourceType_;
	bool playInRegion_;
	bool showPlaybackControls_;
	int x_, y_, width_, height_;


	std::set<float> firedPlayEventTimes_;

	float startTime_;
	float currentTime_;

	std::vector<std::string> sourceName_;
	std::vector<int> sourceMode_;

};
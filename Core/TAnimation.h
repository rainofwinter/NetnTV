#pragma once
#include "SceneObject.h"

class TAnimationObject;

class TAnimFrame;
typedef boost::shared_ptr<TAnimFrame> TAnimFrameSPtr;

class TAnimDepth;
typedef boost::shared_ptr<TAnimDepth> TAnimDepthSPtr;

class TPlaceObject;
class TRemoveObject;
class TPlaySound;

typedef struct _xmlNode xmlNode;

///////////////////////////////////////////////////////////////////////////////
class TFrames
{
	friend class TPlaceObject;
	friend class TRemoveObject;
public:
	TFrames();
	TFrames(const TFrames & rhs);
	virtual ~TFrames() {}
	void drawDepths(GfxRenderer * gl, TAnimationObject * animObj) const;
	/**
	@return did repeat?
	*/
	bool updateFrame(TAnimationObject * animObj, float time);

	void doFirstFrame(TAnimationObject * animObj, float time);

	void addFrame(const TAnimFrameSPtr & frame);

	void referencedFiles(std::vector<std::string> * refFiles) const;

	int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	void play(TAnimationObject * animObj, float startTime);
	void setDuration(float duration);
	
	void setRepeat(const bool & repeat) {repeat_ = repeat;}
	const bool & repeat() const {return repeat_;}

	void setParentDepth(TAnimDepth * parentDepth);
	TAnimDepth * parentDepth() const {return parentDepth_;}

	void stopSounds();

protected:
	void drawMasks(GfxRenderer * gl, TAnimationObject * animObj, std::list<TAnimDepth *> & masks) const;
	void drawDepth(GfxRenderer * gl, TAnimationObject * animObj, TAnimDepth * depth) const;
	void unloadDepths();
protected:
	std::vector<TAnimFrameSPtr> frames_;
	std::vector<TAnimDepthSPtr> depths_;

	float duration_;
	bool isPlaying_;
	int curFrameIndex_;
	bool repeat_;
	int curRep_;
	float animTime_;
	float startTime_;	

	TAnimDepth * parentDepth_;
};

///////////////////////////////////////////////////////////////////////////////
class TAnimSprite;

class TAnimObject 
{
public:
	TAnimObject();
	virtual ~TAnimObject();

	virtual TAnimObject * clone() = 0;
	
	virtual TAnimSprite * sprite() {return NULL;}

	virtual bool isLoaded() const = 0;
	virtual void load(GfxRenderer * gl, TAnimationObject * animObj) = 0;
	virtual void unload() = 0;
	virtual void asyncLoadUpdate() = 0;

	unsigned short id() const {return id_;}

	virtual unsigned int loadSize() const = 0;
			
	virtual void update(TAnimationObject * animObj, float time) = 0;
	virtual void draw(GfxRenderer * gl, TAnimationObject * animObj, TAnimDepth * depth) const = 0;
	
	virtual void referencedFiles(std::vector<std::string> * refFiles) const = 0;

	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index) = 0;
	
	void setId(unsigned short id) 
	{
		id_ = id;
	}
protected:
	unsigned short id_;

};
///////////////////////////////////////////////////////////////////////////////


class TAnimSprite : public TAnimObject, public TFrames
{
	friend class TAnimSpriteLoaded;
public:
	virtual TAnimSprite * sprite() {return this;}
	TAnimSprite();
	~TAnimSprite();

	TAnimSprite(const TAnimSprite & rhs);
	virtual TAnimObject * clone() {return new TAnimSprite(*this);}
		
	virtual bool isLoaded() const;
	virtual void load(GfxRenderer * gl, TAnimationObject * animObj);
	virtual void unload();
	virtual void asyncLoadUpdate();


	virtual unsigned int loadSize() const;
	virtual void update(TAnimationObject * animObj, float time);
	virtual void draw(GfxRenderer * gl, TAnimationObject * animObj, TAnimDepth * depth) const;

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;

	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);
private:
	void handleFrame(TAnimFrame * frame);

private:
	///loaded object instances
	std::vector<TPlaceObject *> loadObjs_;
};
///////////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr<TAnimObject> TAnimObjectSPtr;

class TAnimImage: public TAnimObject
{
public:
	TAnimImage();
	~TAnimImage();

	TAnimImage(const TAnimImage & rhs);
	virtual TAnimObject * clone() {return new TAnimImage(*this);}

	virtual bool isLoaded() const;
	virtual void load(GfxRenderer * gl, TAnimationObject * animObj);
	virtual void unload();
	virtual void asyncLoadUpdate();

	virtual unsigned int loadSize() const {return imageSize_;}	
	virtual void update(TAnimationObject * animObj, float time);
	virtual void draw(GfxRenderer * gl, TAnimationObject * animObj, TAnimDepth * depth) const;

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;

	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	//void setFileName(const std::string & fileName);
	void setFileNames(
		const std::vector< std::string> & fileNames,
		const std::vector< std::pair<int, int> > & coords);

	const std::vector<std::string> & fileNames() const
	{
		return fileNames_;
	}

	void setFileSize(unsigned int size)
	{
		imageSize_ = size;
	}
	
	void setMatrix(const Matrix & matrix)
	{
		matrix_ = matrix;
	}
private:

	void drawRegion(
		Texture * texture, GfxRenderer * gl, 
		TAnimDepth * depth, int x, int y, float opacity) const;

	std::vector< std::string> fileNames_;
	std::vector< std::pair<int, int> > coords_;

	unsigned int imageSize_;	
	Matrix matrix_;

	//boost::shared_ptr<Text> text_;
	
	std::vector<TextureSPtr> textures_;
};



///////////////////////////////////////////////////////////////////////////////

class TAnimTag
{
public:
	virtual TAnimTag * clone() const = 0;
	virtual ~TAnimTag() {}
	virtual TRemoveObject * removeObject() {return NULL;}
	virtual TPlaceObject * placeObject() {return NULL;}
	virtual TPlaySound * playSound() {return NULL;}
	virtual void doTag(TFrames * obj, TAnimationObject * animObj, float time) = 0;

};

class AudioObject;

class TPlaySound : public TAnimTag
{
public:
	TPlaySound();
	~TPlaySound();
	TPlaySound(const TPlaySound & rhs);
	virtual TAnimTag * clone() const
	{
		return new TPlaySound(*this);
	}

	virtual TPlaySound * playSound() {return this;}

	AudioObject * audio() {return audio_.get();}
		
	virtual void doTag(TFrames * obj, TAnimationObject * animObj, float time);

	void setFileName(const std::string & fileName);
	const std::string fileName() const {return fileName_;}

	void setTime(const float & time) {time_ = time;}
private:
	std::string fileName_;
	boost::scoped_ptr<AudioObject> audio_;
	float time_;
};

class TPlaceObject : public TAnimTag
{
public:
	TPlaceObject();
	TPlaceObject(const TPlaceObject & rhs);
	virtual TAnimTag * clone() const
	{
		return new TPlaceObject(*this);
	}

	void load(GfxRenderer * gl, TAnimationObject * animObj);	
	void unload();
	void asyncLoadUpdate();
	void draw(GfxRenderer * gl, TAnimationObject * animObj, TAnimDepth * depth);
	bool isLoaded() const;
	
	virtual TPlaceObject * placeObject() {return this;}
	
	unsigned short depth() const {return depth_;}
	TAnimObject * object() const {return object_.get();}

	virtual void doTag(TFrames * obj, TAnimationObject * animObj, float time);

	void setObject(TAnimObject & obj);
	void setDepth(unsigned short depth);
	void setMatrix(const Matrix & matrix);
	void setOpacity(float opacity);
	void setVisibility(bool visibility);

	
	bool keepLoaded() const {return keepLoaded_;}
	void setKeepLoaded(bool keepLoaded) {keepLoaded_ = keepLoaded;}

	unsigned short clipDepth() const {return clipDepth_;}
	void setClipDepth(unsigned short clipDepth) {clipDepth_ = clipDepth;}
private:
	unsigned short depth_;
	TAnimObjectSPtr object_;
	
	bool hasMatrix_;
	Matrix matrix_;
	
	bool hasVisibility_;
	bool visibility_;
	
	bool hasOpacity_;
	float opacity_;

	bool keepLoaded_;

	unsigned short clipDepth_;
};


class TRemoveObject : public TAnimTag
{
public:
	virtual TAnimTag * clone() const
	{
		return new TRemoveObject(*this);
	}

	virtual TRemoveObject * removeObject() {return this;}
	virtual void doTag(TFrames * obj, TAnimationObject * animObj, float time);

	void setDepth(unsigned short depth);
	unsigned short depth() const {return depth_;}
private:
	unsigned short depth_;
};

typedef boost::shared_ptr<TAnimTag> TAnimTagSPtr;

///////////////////////////////////////////////////////////////////////////////

class TAnimFrame
{
public:
	TAnimFrame();
	TAnimFrame(const TAnimFrame & rhs);
	void setTime(float time)
	{
		time_ = time;
	}
	float time() const {return time_;}
	const std::vector<TAnimTagSPtr> & tags() const {return tags_;}

	void addTag(const TAnimTagSPtr & tag);
private:
	
	float time_;
	std::vector<TAnimTagSPtr> tags_;
};



///////////////////////////////////////////////////////////////////////////////

class TLoadCue
{
public:
	TLoadCue()
	{
		time_ = 0.0f;
		endTime_ = 0.0f;
		placeObject_ = NULL;
	}

	float time() const {return time_;}
	void setTime(float time) {time_ = time;}

	float endTime() const {return endTime_;}
	void setEndTime(float time) {endTime_ = time;}
	
	TPlaceObject * placeObject() const {return placeObject_;}
	void setPlaceObject(TPlaceObject * object) {placeObject_ = object;}
	
	float loadTime(float loadSpeed) const;
	
	bool operator < (const TLoadCue & rhs) const
	{
		if (time_ != rhs.time_) return time_ < rhs.time_;
		else return endTime_ < rhs.endTime_;
	}

private:
	float time_;
	float endTime_;
	TPlaceObject * placeObject_;
};

///////////////////////////////////////////////////////////////////////////////

class TAnimDepth
{
	friend class TPlaceObject;
	friend class TRemoveObject;
public:
	TAnimDepth(TFrames * parent, unsigned short index);

	void setPlaceObject(TPlaceObject * object);

	TPlaceObject * placeObject() const {return placeObject_;}

	VisualAttrib * visualAttrib() {return &visualAttrib_;}

	TFrames * parentSprite() const;
	void setParentSprite(TFrames *);
private:
public:
	///visual attributes for the object at this depth
	VisualAttrib visualAttrib_;

	TPlaceObject * placeObject_;

	unsigned short index_;
	TFrames * parentSprite_;
};

///////////////////////////////////////////////////////////////////////////////
class TAnimationObject : public SceneObject, public TFrames
{
public:
	TAnimationObject();
	~TAnimationObject();
	TAnimationObject(
		const TAnimationObject & rhs, ElementMapping * elementMapping);
	
	virtual const char * typeStr() const 
	{
		return "TAnimationObject";
	}
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("DCDCDAD1-ABBD-DCCC-BBCB-01DCD0D991AF");
		return uuid;
	}

	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new TAnimationObject(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & elementMapping);

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(
		const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int startIndex);

	virtual void drawObject(GfxRenderer * gl) const;

	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;	
	
	virtual void start(float docTime);
	virtual void preStart(float docTime);
	virtual bool update(float sceneTime);
	virtual bool asyncLoadUpdate();
	virtual bool isLoaded() const;
	
	virtual bool dependsOn(Scene * scene) const;

	virtual unsigned char version() const {return 3;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "TAnimationObject";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	void setAnimDataFileName(const std::string & animDataFileName);
	const std::string & animDataFileName() const {return animDataFileName_;}

	bool loadXml(const std::string & fileName);
	void unloadXml();
	void play(float time);
	void stop();
	void reset();
	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	unsigned int width() const {return width_;}
	unsigned int height() const {return height_;}

	const float & loadSpeed() const {return loadSpeed_;}
	void setLoadSpeed(const float & loadSpeed) {loadSpeed_ = loadSpeed;}

	const bool & repeat() const {return repeat_;}
	void setRepeat(const bool & repeat) {repeat_ = repeat;}

	const std::string & actualFileName(const std::string & fileName) const;

	const std::vector<std::string> allDataFileName() const { return fileNames_;}
	
	bool exchangeXmltext(const std::string & fileName, const std::string & origfile, std::vector<std::string> &targettexts, const std::string & newtext);	
	
private:
	void create();

	void computeLoadCues();

	void addNewLoadCue(float time, TPlaceObject * placeObject);
	int getLoadCueIndex(float time);

	void handleFrame(TAnimFrame * frame);

	
	void loadXmlDefineSprite(xmlNode * node);
	void loadXmlDefineImage(xmlNode * node);

	TPlaySound * loadXmlPlaySound(xmlNode * node);
	TPlaceObject * loadXmlPlaceObject(xmlNode * node);
	TRemoveObject *  loadXmlRemoveObject(xmlNode * node);

	TAnimFrame * loadXmlFrame(xmlNode * node);

	bool intersectRect(Vector3 * out, const Ray & ray);
private:
	VisualAttrib visualAttrib_;
	///In kbytes / sec
	float loadSpeed_;
	std::string animDataFileName_;
	std::string directory_;
	bool hasActualFileNames_;
	std::map<std::string, std::string> actualFileNames_;


	
	typedef std::map<unsigned short, TAnimObjectSPtr> DefineObjects;
	DefineObjects defineObjects_;
	std::deque<TLoadCue> loadCues_;
	int curLoadCueIndex_;

	std::vector<TPlaceObject *> preloadObjs_;
	///objects to load right now
	std::list<TPlaceObject *> loadObjs_;
	unsigned int width_, height_;


	bool needsMask_;

	std::vector< std::string> fileNames_;
};
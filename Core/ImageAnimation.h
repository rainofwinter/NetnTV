#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "Texture.h"

///////////////////////////////////////////////////////////////////////////////

class LoadingScreen;
class ScriptProcessor;

class ImageAnimation : public SceneObject
{

public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "ImageAnimation";
	}
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("CDCDAD0D-ABCD-DDCC-EDAB-51D5DAD987AF");
		return uuid;
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new ImageAnimation(*this, elementMapping);}

	
	virtual void remapReferences(const ElementMapping & elementMapping);
	
	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(
		const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int startIndex);

	ImageAnimation();
	ImageAnimation(const ImageAnimation & rhs, ElementMapping * elementMapping);
	
	~ImageAnimation();

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;	

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);


	virtual void start(float docTime);
	virtual void preStart(float docTime);
	virtual bool update(float sceneTime);
	virtual bool asyncLoadUpdate();
	virtual bool isLoaded() const;

	const float & width() const {return width_;}
	void setWidth(const float & width) {width_ = width;}

	const float & height() const {return height_;}
	void setHeight(const float & height) {height_ = height;}

	const float & fps() const {return fps_;}
	void setFps(const float & fps) {fps_ = fps;}

	const std::vector<std::string> & fileNames() const 
	{
		return fileNames_;
	}
	
	void setFileNames(const std::vector<std::string> & fileNames);

	virtual bool dependsOn(Scene * scene) const;

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "ImageAnimation";}
	virtual void writeXml(XmlWriter & x) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	void play();
	void reset();
	void stop();

	void pause();
	void resume();

	const std::vector<TextureSPtr> & buffers() const {return textures_;}

	const bool & repeat() const {return repeat_;}
	void setRepeat(const bool & repeat) {repeat_ = repeat;}

	const Texture::Format & format() const {return format_;}
	void setFormat(const Texture::Format & format) {format_ = format;}

	const int & maxMemFrames() const {return maxMemFrames_;}
	void setMaxMemFrames(const int & maxMemFrames) {maxMemFrames_ = maxMemFrames;}
private:	
	void create();
	ImageAnimation(const ImageAnimation & rhs);
	ImageAnimation & operator = (const ImageAnimation & rhs);
	bool intersectRect(Vector3 * out, const Ray & ray);

	bool setFrame(int frameIndex);
	void initBuffers();
private:	
	VisualAttrib visualAttrib_;

	float width_, height_;	
	
	std::vector<std::string> fileNames_;
	std::vector<unsigned char> refCount_;
	std::vector<TextureSPtr> textures_;


	
	


	float fps_;
	bool repeat_;
	float startTime_;

	Texture::Format format_;
	int maxMemFrames_;

	int curFrameIndex_, curDisplayIndex_;

	bool pause_;
	float puaseGapTime_;
};


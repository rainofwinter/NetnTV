#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "Texture.h"
///////////////////////////////////////////////////////////////////////////////
class Texture;
class Mesh;
class Material;


class ImageGrid : public SceneObject
{
public:
	virtual const char * typeStr() const 
	{
		return "ImageGrid";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("BBBB0DF2-4D37-2799-AAAA-AAAA3733872F");
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new ImageGrid(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);
	
	virtual void start(float docTime);
	virtual void preStart(float docTime);
	bool update(float sceneTime);

	virtual bool asyncLoadUpdate();

	ImageGrid();
	ImageGrid(const ImageGrid & rhs, ElementMapping * elementMapping);

	~ImageGrid();

	void setFormat(Texture::Format format) {format_ = format;}
	Texture::Format format() const {return format_;}

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 3;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "ImageGrid";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	const int & width() const {return width_;}
	void setWidth(const int & width) {width_ = width;} 
	const int & height() const {return height_;}
	void setHeight(const int & height) {height_ = height;}

	const std::vector<std::string> & fileNames() const {return fileNames_;}
	void setFileNames(const std::vector<std::string> & fileNames)
	{
		fileNames_ = fileNames;
	}

	const int & thumbWidth() const {return thumbWidth_;}
	void setThumbWidth(const int & width) {thumbWidth_ = width;} 
	const int & thumbHeight() const {return thumbHeight_;}
	void setThumbHeight(const int & height) {thumbHeight_ = height;}

	const bool & doAnimation() const {return doAnimation_;}
	void setDoAnimation(const bool & doAnimation) 
	{
		doAnimation_ = doAnimation;
		t_ = 1;
	}

	const int & gap() const  {return gap_;}
	void setGap(const int & gap) {gap_ = gap;}

	const float & startTime() const {return startTime_;}
	void setStartTime(const float & startTime) {startTime_ = startTime;}

	const float & duration() const {return duration_;}
	void setDuration(const float & duration) {duration_ = duration;}

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	virtual bool clickEvent(const Vector2 & mousePos, int pressId);

private:
	ImageGrid(const ImageGrid & rhs);
	ImageGrid & operator = (const ImageGrid & rhs);
	void create();
	void processCoords(const Vector2 & pos, 
		Vector2 * localCoords, Ray * mouseRay, Ray * invMouseRay);
private:

	std::vector<std::string> fileNames_;
	int thumbWidth_;
	int thumbHeight_;	
	int gap_;	
	int width_;
	int height_;		
	float duration_;
	float startTime_;
	float docBaseStartTime_;//
	
	struct ImageData
	{
		Image * image;
		Vector3 dir;
		Vector3 up;
		float tRandMult;

		float startRotX, startRotY, startRotZ;

		ImageData();
		~ImageData();
		
	};
	std::vector<ImageData *> images_;	


	float trajRandomness_;
	float trajDepth_;
	float t_;//

	VisualAttrib visualAttrib_;

	bool doAnimation_;
	Texture::Format format_;
};
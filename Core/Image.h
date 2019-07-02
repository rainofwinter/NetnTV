#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "Texture.h"
///////////////////////////////////////////////////////////////////////////////
class Mesh;
class Material;
class ScriptProcessor;
class Color;
struct JSObject;
class XmlReader;
class XmlWriter;

class Image : public SceneObject
{

public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "Image";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = 
			sUuidGen_("24ED0DF2-4D37-2799-1A12-56D53733872F");
		return uuid;
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new Image(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	bool update(float sceneTime);

	void setFormat(Texture::Format format) {format_ = format;}
	Texture::Format format() const {return format_;}

	virtual bool asyncLoadUpdate();	
	virtual bool isLoaded() const;

	bool getBitsFromPixelBuffer(PixelBuffer * pixelBuffer);
	PixelBuffer * createPixelBufferFromImage();

	const Texture * texture() const {return texture_;}

	Image();
	Image(const Image & rhs, ElementMapping * elementMapping);

	~Image();

	/**
	Default behavior is to set width and height to the size of the image file
	*/
	void setFileName(const std::string & fileName, bool resetDims = true);
	const std::string & fileName() const {return fileName_;}

	void swap(Image * other);

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 5;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "Image";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	const int & width() const {return width_;}
	void setWidth(const int & width) {width_ = width;}

	const int & height() const {return height_;}
	void setHeight(const int & height) {height_ = height;}

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	bool getPixel(int x, int y, Color * color) const;

	virtual void requestLocalSaveObject();

	void requestZoom();

	const bool & pixelDraw() const {return pixelDraw_;}
	void setPixelDraw(const bool & pixelDraw) {pixelDraw_ = pixelDraw;}


private:
	Image(const Image & rhs);
	Image & operator = (const Image & rhs);
	void create();
private:
	//Mesh * mesh_;
	Texture * texture_;
	//Material * material_;
	std::string fileName_;
	VisualAttrib visualAttrib_;

	int width_;	
	int height_;

	Texture::Format format_;

	bool pixelDraw_;
};
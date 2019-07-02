#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "Texture.h"
///////////////////////////////////////////////////////////////////////////////
class Mesh;
class Material;


class TestBed : public SceneObject
{

public:
	virtual const char * typeStr() const 
	{
		return "TestBed";
	}
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("24E11DF2-4D37-1199-1A12-56D53722872F");
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new TestBed(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	bool update(float sceneTime);

	void setFormat(Texture::Format format) {format_ = format;}
	Texture::Format format() const {return format_;}

	virtual bool asyncLoadUpdate();
	bool isLoaded() const;

	const Texture * texture() const {return texture_;}

	TestBed();
	TestBed(const TestBed & rhs, ElementMapping * elementMapping);

	~TestBed();

	/**
	Default behavior is to set width and height to the size of the image file
	*/
	void setFileName(const std::string & fileName, bool resetDims = true);
	const std::string & fileName() const {return fileName_;}

	void swap(TestBed * other);

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 4;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	const int & width() const {return width_;}
	void setWidth(const int & width) {width_ = width;}

	const int & height() const {return height_;}
	void setHeight(const int & height) {height_ = height;}

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

private:
	TestBed(const TestBed & rhs);
	TestBed & operator = (const TestBed & rhs);
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
};
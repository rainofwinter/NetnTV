#include "stdafx.h"
#include "TestBed.h"
#include "Mesh.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Reader.h"
#include "FileUtils.h"
#include "Global.h"

using namespace std;

void TestBed::create()
{
	texture_ = new Texture;
	width_ = 0;
	height_ = 0;
	format_ = Texture::UncompressedRgba32;
}

TestBed::TestBed()
{
	create();
	setId("TestBed");
}

TestBed::TestBed(const TestBed & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();
	fileName_ = rhs.fileName_;
	visualAttrib_ = rhs.visualAttrib_;
	width_ = rhs.width_;
	height_ = rhs.height_;
}




TestBed::~TestBed()
{	
	delete texture_;	
}

void TestBed::swap(TestBed * other)
{
	Texture * tempTexture = texture_;
	string tempFileName = fileName_;

	fileName_ = other->fileName_;
	texture_ = other->texture_;

	other->fileName_ = tempFileName;
	other->texture_ = tempTexture;

}

void TestBed::referencedFiles(std::vector<std::string> * refFiles) const
{
	SceneObject::referencedFiles(refFiles);
	refFiles->push_back(fileName_);
}

int TestBed::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);
	fileName_ = refFiles[index++];
	return index;
}


void TestBed::setFileName(const std::string & fileName, bool resetDims)
{
	fileName_ = fileName;
	if (resetDims)
	{
		width_ = -1;
		height_ = -1;
	}
}

void TestBed::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
	texture_->init(gl, fileName_, format_);	
	//TODO remove this when older versions are phased out
	if (texture_->isLoaded())
	{
		if (width_ < 0) width_ = texture_->width();
		if (height_ < 0) height_ = texture_->height();
	}
}

void TestBed::uninit()
{
	texture_->uninit();	
	SceneObject::uninit();
}


bool TestBed::update(float sceneTime)
{	
	bool needRedraw = SceneObject::update(sceneTime);
	//TODO right now this always returns true while image is loading
	//it should return true right at the point image loading completes
	needRedraw |= !texture_->isLoaded();
	
	return needRedraw;
}

bool TestBed::asyncLoadUpdate()
{
	bool everythingLoaded = SceneObject::asyncLoadUpdate();
	if (!texture_->isLoaded())
	{
		texture_->asyncLoadUpdate();				
	}
	
	if (texture_->isLoaded())
	{		
		//TODO remove this when older versions are phased out
		if (width_ < 0) width_ = texture_->width();
		if (height_ < 0) height_ = texture_->height();
	}

	return everythingLoaded & texture_->isLoaded();	
}

bool TestBed::isLoaded() const
{
	return texture_->isLoaded();
}

void TestBed::drawObject(GfxRenderer * gl) const
{
	float fOpacity = totalOpacity();
	if (fOpacity == 0) return;

	if (!texture_->isLoaded()) return;
	gl->useTextureProgram();
	gl->setTextureProgramOpacity(fOpacity);		
	gl->use(texture_);
	gl->drawRect(0, 0, width(), height());
}


BoundingBox TestBed::extents() const
{
	return BoundingBox(
		Vector3(0, 0, 0), 
		Vector3((float)width(), (float)height(), 0));
}

void TestBed::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");	
	
	writer.write(fileName_, "fileName");

	writer.write(format_, "format");

	writer.write(width_, "width");
	writer.write(height_, "height");
	
}

void TestBed::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);	
	reader.read(fileName_);
	
	if (version >= 4)
	{
		reader.read(format_);
	}
	if (version >= 3)
	{
		reader.read(width_);
		reader.read(height_);
	}
	else
	{
		width_ = -1;
		height_ = -1;
		if (version == 2)
		{
			float overrideWidth, overrideHeight;
			reader.read(overrideWidth);
			reader.read(overrideHeight);
			
			if (overrideWidth > 0) width_ = overrideWidth;
			if (overrideHeight > 0) height_ = overrideHeight;		
		}		
	}
}


SceneObject * TestBed::intersect(Vector3 * out, const Ray & ray)
{
	if (!visualAttrib_.isVisible()) return 0;
	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();


	Vector3 a = trans * Vector3(0, 0, 0);
	Vector3 b = trans * Vector3(0, height(), 0);
	Vector3 c = trans * Vector3(width(), height(), 0);
	Vector3 d = trans * Vector3(width(), 0, 0);

	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return this;
	}
	else return 0;
}
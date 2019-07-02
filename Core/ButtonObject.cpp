#include "StdAfx.h"
#include "ButtonObject.h"
#include "Writer.h"
#include "Reader.h"
#include "Xml.h"
#include "GfxRenderer.h"
#include "Texture.h"
#include "Image.h"
#include "SceneObject.h"
#include "EventListener.h"
#include "Event.h"

#include "Global.h"
#include "PressEvent.h"

using namespace std;

void ButtonObject::create()
{
	width_ = 300;
	height_ = 100;
	format_ = Texture::UncompressedRgba32;
	pressed_ = false;
}

ButtonObject::ButtonObject()
{
	create();
	setId("ButtonObject");
}

ButtonObject::ButtonObject(const ButtonObject & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();
	visualAttrib_ = rhs.visualAttrib_;
	height_ = rhs.height_;
	width_ = rhs.width_;
	format_ = Texture::UncompressedRgba32;

	BOOST_FOREACH(string fileName, rhs.fileNames_)
		fileNames_.push_back(fileName);	
}

ButtonObject::~ButtonObject()
{
	uninit();
}

bool ButtonObject::asyncLoadUpdate()
{
	bool everythingLoaded = SceneObject::asyncLoadUpdate();
	for (int i = 0; i < (int)textures_.size(); ++i)
	{
		textures_[i]->asyncLoadUpdate();
		everythingLoaded &= textures_[i]->isLoaded();
	}
	return everythingLoaded;
}

bool ButtonObject::isLoaded() const
{
	if (!gl_) return false;
	bool ret = true;
	for (int i = 0; i < (int)textures_.size(); ++i)
	{
		ret &= textures_[i]->isLoaded();			
	}
	
	return ret;
}

void ButtonObject::drawObject(GfxRenderer * gl) const
{
	if (textures_.empty()) return;
	if (textures_[0]->needSeparateAlpha())
	{
		gl->useTextureAlphaProgram();
		gl->setTextureAlphaProgramOpacity(visualAttrib_.opacity());		
	}
	else
	{
		gl->useTextureProgram();
		gl->setTextureProgramOpacity(visualAttrib_.opacity());				
	}	
	
	if(pressed_)
	{
		gl->use(textures_[1].get());
		gl->drawRect(0, 0, width_, height_);
	}else{
		gl->use(textures_[0].get());
		gl->drawRect(0, 0, width_, height_);
	}
}

bool ButtonObject::update(float sceneTime)
{
	bool needRedraw = false;
	needRedraw |= SceneObject::update(sceneTime);
	
	return needRedraw;
}

void ButtonObject::referencedFiles(std::vector<std::string> * refFiles) const
{
	SceneObject::referencedFiles(refFiles);

	BOOST_FOREACH(std::string fileName, fileNames_)
		refFiles->push_back(fileName);
}

int ButtonObject::setReferencedFiles(
	const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);

	BOOST_FOREACH(std::string & fileName, fileNames_)
		fileName = refFiles[index++];

	return index;
}

void ButtonObject::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);

	writer.write(visualAttrib_, "visualAttrib");	
	
	writer.write(fileNames_, "fileNames");

	writer.write(format_, "format");

	writer.write(width_, "width");
	writer.write(height_, "height");
}

void ButtonObject::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);	
	reader.read(fileNames_);
	reader.read(format_);
	reader.read(width_);
	reader.read(height_);
}


void ButtonObject::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();
	
	w.startTag("FileNames");
	BOOST_FOREACH(string str, fileNames_)	
	{
		w.writeTag("FileName", str);
	}
	w.endTag();
	w.writeTag("Format", (unsigned int)format_);
	w.writeTag("Width", width_);
	w.writeTag("Height", height_);
}

void ButtonObject::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);

	unsigned int val;

	for (xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
		}
		else if (r.isNodeName(curNode, "VisualAttrib"))
		{
			string fileName;
			for(xmlNode * fileNode = curNode->children; fileNode; fileNode = fileNode->next)
			{		
				if (fileNode->type != XML_ELEMENT_NODE) continue;				
				if (r.getNodeContentIfName(fileName, fileNode, "FileName"))
				{
					fileNames_.push_back(fileName);
				}
			}
		}
		else if (r.getNodeContentIfName(val, curNode, "Format"))
		{			
			format_ = (Texture::Format)val;
		}
		else if (r.getNodeContentIfName(width_, curNode, "Width"));
		else if (r.getNodeContentIfName(height_, curNode, "Height"));
	}
}


void ButtonObject::setFileNames(const std::vector<std::string> & fileNames)
{	
	fileNames_ = fileNames;	
	init(gl_);
}

BoundingBox ButtonObject::extents() const
{
	return BoundingBox(
		Vector3(0, 0, 0), 
		Vector3(width(), height(), 0));
}

void ButtonObject::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
	int numFiles = (int)fileNames_.size();
	textures_.resize(numFiles);

	for (int i = 0; i < numFiles; ++i)
	{
		textures_[i].reset(new Texture);
		textures_[i]->setFileName(fileNames_[i]);
	}

	for (int i = 0; i < (int)textures_.size(); ++i)
		if (!textures_[i]->isInit()) textures_[i]->init(gl, format_);

}

bool ButtonObject::intersectRect(Vector3 * out, const Ray & ray)
{
	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();

	Vector3 a = trans * Vector3(0, 0, 0);
	Vector3 b = trans * Vector3(0, height(), 0);
	Vector3 c = trans * Vector3(width(), height(), 0);
	Vector3 d = trans * Vector3(width(), 0, 0);

	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return true;
	}
	else return false;
}

SceneObject * ButtonObject::intersect(Vector3 * intPt, const Ray & ray)
{
	if (!visualAttrib_.isVisible()) return 0;
	if (intersectRect(intPt, ray)) return this;
	else return 0;
}

void ButtonObject::uninit()
{
	if (!gl_) return;
	BOOST_FOREACH(TextureSPtr & texture, textures_) texture->uninit();
	SceneObject::uninit();
}

bool ButtonObject::pressEvent(const Vector2 & startPos, int pressId)
{
	pressed_ = true;
	bool handled = SceneObject::pressEvent(startPos, pressId);
	if (handled) {return true;}

	return true;
}

bool ButtonObject::releaseEvent(const Vector2 & pos, int pressId)
{
	pressed_ = false;
	bool handled = SceneObject::releaseEvent(pos, pressId);	
	if (handled){return true;}

	return true;
}

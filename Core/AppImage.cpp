#include "stdafx.h"
#include "AppImage.h"
#include "Reader.h"
#include "Writer.h"
#include "FileUtils.h"
#include "Xml.h"
using namespace std;

void AppImage::create()
{
	width_ = 0;
	height_ = 0;
	visible_ = true;
	opacity_ = 1.0f;	
}

AppImage::AppImage()
{
	create();
}

void AppImage::referencedFiles(std::vector<std::string> * refFiles) const
{
	AppObject::referencedFiles(refFiles);
	refFiles->push_back(fileName_);
}

int AppImage::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = AppObject::setReferencedFiles(baseDirectory, refFiles, index);
	fileName_ = refFiles[index++];
	return index;
}

BoundingBox AppImage::extents() const
{
	return BoundingBox(
		Vector3(0, 0, 0), 
		Vector3((float)width(), (float)height(), 0));
}

bool AppImage::intersect(Vector3 * out, const Ray & ray)
{
	if (!visible()) return 0;


	Vector3 a = Vector3((float)x_, (float)y_, 0.0f);
	Vector3 b = Vector3(float(x_ + width_), (float)y_, 0.0f);
	Vector3 c = Vector3(float(x_ + width_), float(y_ + height_), 0.0f);
	Vector3 d = Vector3((float)x_, float(y_ + height_), 0.0f);

	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return true;
	}
	else return false;
}

void AppImage::write(Writer & writer) const
{
	writer.writeParent<AppObject>(this);
	
	writer.write(fileName_, "fileName");
}


void AppImage::writeXml(XmlWriter & w) const
{
	AppObject::writeXml(w);
	w.writeTag("FileName", fileName_);
}

void AppImage::readXml(XmlReader & r, xmlNode * parent)
{
	AppObject::readXml(r, parent);

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(fileName_, curNode, "FileName"));
	}
	
}

void AppImage::read(Reader & reader, unsigned char version)
{
	reader.readParent<AppObject>(this);


	if (version == 0)
	{
		VisualAttrib visualAttrib;
		int width, height;
		string fileName;

		reader.read(visualAttrib);	
		reader.read(fileName);	

		reader.read(width);
		reader.read(height);

		Transform trans = visualAttrib.transform();

		width_ = int(trans.scaling().x * width);
		height_ = int(trans.scaling().y * height);

		x_ = (int)trans.translation().x;
		y_ = (int)trans.translation().y;
		fileName_ = fileName;
	}
	else
	{
	reader.read(fileName_);	
	}

}

void AppImage::setFileName(const std::string & fileName, bool resetDims)
{
	fileName_ = fileName;
}

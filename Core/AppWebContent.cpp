#include "stdafx.h"
#include "AppWebContent.h"
#include "Reader.h"
#include "Writer.h"
#include "Xml.h"
#include "FileUtils.h"

using namespace std;

void AppWebContent::create()
{
	width_ = 320;
	height_ = 200;
	handleEvents_ = true;
	stringId_ = "AppWebContent";
	mode_ = ModeUrl;
	setHandleEvents(true);
}

AppWebContent::AppWebContent()
{
	create();
}

void AppWebContent::referencedFiles(std::vector<std::string> * refFiles) const
{
	AppObject::referencedFiles(refFiles);
}

int AppWebContent::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = AppObject::setReferencedFiles(baseDirectory, refFiles, index);
	return index;
}

BoundingBox AppWebContent::extents() const
{
	return BoundingBox(
		Vector3(0, 0, 0), 
		Vector3((float)width(), (float)height(), 0));
}

bool AppWebContent::intersect(Vector3 * out, const Ray & ray)
{
	if (!visualAttrib_.isVisible()) return 0;
	Matrix trans = visualAttrib_.transformMatrix();


	Vector3 a = trans * Vector3(0.0f, 0.0f, 0.0f);
	Vector3 b = trans * Vector3(0.0f, (float)height(), 0.0f);
	Vector3 c = trans * Vector3((float)width(), (float)height(), 0.0f);
	Vector3 d = trans * Vector3((float)width(), 0.0f, 0.0f);

	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return true;
	}
	else return false;
}

void AppWebContent::write(Writer & writer) const
{
	writer.writeParent<AppObject>(this);
	writer.write(visualAttrib_, "visualAttrib");
	writer.write(mode_, "mode");
	writer.write(url_, "url");
	writer.write(html_, "html");
	
	writer.write(width_, "width");
	writer.write(height_, "height");
	
}

void AppWebContent::read(Reader & reader, unsigned char version)
{
	reader.readParent<AppObject>(this);
	reader.read(visualAttrib_);	
	if (version >= 1)
	{
		reader.read(mode_);
		reader.read(url_);
		reader.read(html_);
	}
	else
	{
		mode_ = ModeUrl;
		reader.read(url_);	
		html_ = "";
	}
	reader.read(width_);
	reader.read(height_);
}

void AppWebContent::writeXml(XmlWriter & w) const
{
	AppObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();

	w.writeTag("Mode", (unsigned int)mode_);
	w.writeTag("Url", url_);
	w.writeTag("Html", html_);
	w.writeTag("Width", width_);
	w.writeTag("Height", height_);
}

void AppWebContent::readXml(XmlReader & r, xmlNode * parent)
{
	AppObject::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(val, curNode, "Mode")) mode_ = (Mode)val;
		else if (r.getNodeContentIfName(url_, curNode, "Url"));
		else if (r.getNodeContentIfName(html_, curNode, "Html"));
		else if (r.getNodeContentIfName(width_, curNode, "Width"));
		else if (r.getNodeContentIfName(height_, curNode, "Height"));
	}
	
}

void AppWebContent::setUrl(const std::string & url)
{
	url_ = url;
}

void AppWebContent::setHtml(const std::string & html)
{
	html_ = html;
}


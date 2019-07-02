#include "stdafx.h"
#include "ImageSetFileAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "Image.h"
#include "ElementMapping.h"
#include "Global.h"
#include "Document.h"
#include "FileUtils.h"

using namespace std;

ImageSetFileAction::ImageSetFileAction()
{	
}

ImageSetFileAction::~ImageSetFileAction()
{	
}

bool ImageSetFileAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == Image().type();
}

void ImageSetFileAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{
	refFiles->push_back(fileName_);
}

int ImageSetFileAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	fileName_ = refFiles[index++];
	return index;
}

Action & ImageSetFileAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (ImageSetFileAction &)rhs;
	return *this;
}


bool ImageSetFileAction::start(SceneObject * object, float time)
{
	Image * obj = (Image *)targetObject_;
	GfxRenderer * gl = obj->parentScene()->parentDocument()->renderer();
	obj->setFileName(fileName_);
	obj->init(gl);
	
	return true;
}

bool ImageSetFileAction::hasDuration() const
{
	return false;
}

bool ImageSetFileAction::isExecuting() const
{
	return false;
}

bool ImageSetFileAction::update(SceneObject * object, float time)
{
	return false;
}

void ImageSetFileAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(fileName_, "video filename");
}

void ImageSetFileAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
	reader.read(fileName_);
}

void ImageSetFileAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Filename", fileName_);
}

void ImageSetFileAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(fileName_, curNode, "Filename"));
	}
}
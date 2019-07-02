#include "stdafx.h"
#include "ImageChangerSetImageAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ImageChanger.h"
#include "ElementMapping.h"

ImageChangerSetImageAction::ImageChangerSetImageAction()
{
	imageIndex_ = 0;
	mode_ = Specific;
	direction_ = Next;
}

ImageChangerSetImageAction::~ImageChangerSetImageAction()
{	
}

bool ImageChangerSetImageAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

Action & ImageChangerSetImageAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (ImageChangerSetImageAction &)rhs;
	return *this;
}


bool ImageChangerSetImageAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == ImageChanger().type();
}

bool ImageChangerSetImageAction::start(SceneObject * object, float time)
{	
	ImageChanger * obj = (ImageChanger *)targetObject_;
	int imageIndex = -1;

	if (mode_ == Specific)
		imageIndex = imageIndex_;
	else
	{
		if (direction_ == Previous)
			imageIndex = obj->prevImageIndex();
		else
			imageIndex = obj->nextImageIndex();
	}
	
	if (imageIndex >= 0) obj->setImage(imageIndex);
	return true;
}

bool ImageChangerSetImageAction::hasDuration() const
{
	return true;
}

bool ImageChangerSetImageAction::isExecuting() const
{
	return false;
}

bool ImageChangerSetImageAction::update(SceneObject * object, float time)
{
	return false;
}

void ImageChangerSetImageAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(mode_, "mode");
	writer.write(direction_, "direction");
	writer.write(imageIndex_, "imageIndex");	
}

void ImageChangerSetImageAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	if (version < 1)
		reader.read(imageIndex_);
	else
	{
		reader.read(mode_);
		reader.read(direction_);
		reader.read(imageIndex_);
	}
}

void ImageChangerSetImageAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Mode", (unsigned int)mode_);
	w.writeTag("Direction", (unsigned int)direction_);
	w.writeTag("ImageIndex", imageIndex_);
}

void ImageChangerSetImageAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(val, curNode, "Mode")) mode_ = (ChangeMode)val;
		else if (r.getNodeContentIfName(val, curNode, "Mode")) direction_ = (Direction)val;
		else if (r.getNodeContentIfName(imageIndex_, curNode, "ImageIndex"));
	}
}
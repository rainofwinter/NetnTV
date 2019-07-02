#include "stdafx.h"
#include "ImageChangerShowImageAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ImageChanger.h"
#include "ElementMapping.h"

ImageChangerShowImageAction::ImageChangerShowImageAction()
{
	imageIndex_ = 0;
	mode_ = Specific;
	direction_ = Next;
}

ImageChangerShowImageAction::~ImageChangerShowImageAction()
{	
}

bool ImageChangerShowImageAction::dependsOn(SceneObject * obj) const 
{
	SceneObject * curObj = targetObject_;
	while (curObj)
	{
		if (curObj == obj) return true;
		curObj = curObj->parent();
	}

	return false;	
}

Action & ImageChangerShowImageAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (ImageChangerShowImageAction &)rhs;
	return *this;
}


bool ImageChangerShowImageAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == ImageChanger().type();
}

bool ImageChangerShowImageAction::start(SceneObject * object, float time)
{	
	Scene * scene = object->parentScene();
	//if (!scene->isCurrentScene()) return false;

	ImageChanger * obj = (ImageChanger *)targetObject_;

	int imageIndex = -1;
	if (mode_ == Specific)
		imageIndex = imageIndex_;
	else
		if (direction_ == Previous)
			imageIndex = obj->prevImageIndex();
		else
			imageIndex = obj->nextImageIndex();

	if (imageIndex >= 0) obj->imageChangeTo(imageIndex);
	return true;
}

bool ImageChangerShowImageAction::hasDuration() const
{
	return true;
}

bool ImageChangerShowImageAction::isExecuting() const
{
	return false;
}

bool ImageChangerShowImageAction::update(SceneObject * object, float time)
{
	return false;
}

void ImageChangerShowImageAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(mode_);
	writer.write(direction_, "direction");
	writer.write(imageIndex_, "imageIndex");		
}

void ImageChangerShowImageAction::read(Reader & reader, unsigned char version)
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

void ImageChangerShowImageAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Mode", (unsigned int)mode_);
	w.writeTag("Direction", (unsigned int)direction_);
	w.writeTag("ImageIndex", imageIndex_);
}

void ImageChangerShowImageAction::readXml(XmlReader & r, xmlNode * parent)
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
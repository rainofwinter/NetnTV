#include "stdafx.h"
#include "ImageAnimationAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Reader.h"
#include "ImageAnimation.h"
#include "ElementMapping.h"
///////////////////////////////////////////////////////////////////////////////
ImageAnimationPlayAction::ImageAnimationPlayAction()
{
}

ImageAnimationPlayAction::~ImageAnimationPlayAction()
{	
}

bool ImageAnimationPlayAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

Action & ImageAnimationPlayAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	*this = (ImageAnimationPlayAction &)rhs;
	return *this;
}


bool ImageAnimationPlayAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == ImageAnimation().type();
}

bool ImageAnimationPlayAction::start(SceneObject * object, float time)
{	
	ImageAnimation * obj = (ImageAnimation *)targetObject_;
	obj->play();
	return true;
}

void ImageAnimationPlayAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);	
}

void ImageAnimationPlayAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}

void ImageAnimationPlayAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void ImageAnimationPlayAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);
}
///////////////////////////////////////////////////////////////////////////////

ImageAnimationStopAction::ImageAnimationStopAction()
{
}

ImageAnimationStopAction::~ImageAnimationStopAction()
{	
}

bool ImageAnimationStopAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

Action & ImageAnimationStopAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	*this = (ImageAnimationStopAction &)rhs;
	return *this;
}


bool ImageAnimationStopAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == ImageAnimation().type();
}

bool ImageAnimationStopAction::start(SceneObject * object, float time)
{	
	ImageAnimation * obj = (ImageAnimation *)targetObject_;
	obj->stop();
	return true;
}

void ImageAnimationStopAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);	
}

void ImageAnimationStopAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}


void ImageAnimationStopAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void ImageAnimationStopAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);
}

///////////////////////////////////////////////////////////////////////////////

ImageAnimationResetAction::ImageAnimationResetAction()
{
}

ImageAnimationResetAction::~ImageAnimationResetAction()
{	
}

bool ImageAnimationResetAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

Action & ImageAnimationResetAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	*this = (ImageAnimationResetAction &)rhs;
	return *this;
}


bool ImageAnimationResetAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == ImageAnimation().type();
}

bool ImageAnimationResetAction::start(SceneObject * object, float time)
{	
	ImageAnimation * obj = (ImageAnimation *)targetObject_;
	obj->reset();
	return true;
}

void ImageAnimationResetAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);	
}

void ImageAnimationResetAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}

void ImageAnimationResetAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void ImageAnimationResetAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);
}

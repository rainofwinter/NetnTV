#include "stdafx.h"
#include "ImageAnimation.h"
#include "ImageAnimationPauseAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Global.h"
#include "FileUtils.h"
#include "Document.h"
#include "DocumentTemplate.h"

using namespace std;


ImageAnimationPauseAction::ImageAnimationPauseAction()
{

}

ImageAnimationPauseAction::~ImageAnimationPauseAction()
{
}

bool ImageAnimationPauseAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == ImageAnimation().type();
}


void ImageAnimationPauseAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{
}

int ImageAnimationPauseAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	return index;
}

Action & ImageAnimationPauseAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;

	*this = (ImageAnimationPauseAction &)rhs;
	return *this;
}


bool ImageAnimationPauseAction::start(SceneObject * object, float time)
{	
	ImageAnimation * iaObj = (ImageAnimation *)targetObject_;
	iaObj->pause();
	return true;
}

bool ImageAnimationPauseAction::hasDuration() const
{
	return false;
}

bool ImageAnimationPauseAction::isExecuting() const
{
	return false;
}

bool ImageAnimationPauseAction::update(SceneObject * object, float time)
{
	return false;
}

void ImageAnimationPauseAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
}

void ImageAnimationPauseAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}

void ImageAnimationPauseAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void ImageAnimationPauseAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);
}
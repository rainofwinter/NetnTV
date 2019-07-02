#include "stdafx.h"
#include "ImageAnimation.h"
#include "ImageAnimationResumeAction.h"
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


ImageAnimationResumeAction::ImageAnimationResumeAction()
{

}

ImageAnimationResumeAction::~ImageAnimationResumeAction()
{
}

bool ImageAnimationResumeAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == ImageAnimation().type();
}

void ImageAnimationResumeAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{
}

int ImageAnimationResumeAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	return index;
}

Action & ImageAnimationResumeAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;

	*this = (ImageAnimationResumeAction &)rhs;
	return *this;
}


bool ImageAnimationResumeAction::start(SceneObject * object, float time)
{	
	ImageAnimation * iaObj = (ImageAnimation *)targetObject_;
	iaObj->resume();
	return true;
}

bool ImageAnimationResumeAction::hasDuration() const
{
	return false;
}

bool ImageAnimationResumeAction::isExecuting() const
{
	return false;
}

bool ImageAnimationResumeAction::update(SceneObject * object, float time)
{
	return false;
}

void ImageAnimationResumeAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
}

void ImageAnimationResumeAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}

void ImageAnimationResumeAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void ImageAnimationResumeAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);
}
#include "stdafx.h"
#include "VideoStopVideoAction.h"
#include "VideoObject.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Global.h"
#include "VideoPlayer.h"
#include "FileUtils.h"

using namespace std;

VideoStopVideoAction::VideoStopVideoAction()
{	
}

VideoStopVideoAction::~VideoStopVideoAction()
{	
}

bool VideoStopVideoAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == VideoObject().type();
}

void VideoStopVideoAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{	
}

int VideoStopVideoAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{	
	return index;
}

Action & VideoStopVideoAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (VideoStopVideoAction &)rhs;
	return *this;
}


bool VideoStopVideoAction::start(SceneObject * object, float time)
{
	Global::instance().videoPlayer()->deactivate();
	return true;
}

bool VideoStopVideoAction::hasDuration() const
{
	return false;
}

bool VideoStopVideoAction::isExecuting() const
{
	return false;
}

bool VideoStopVideoAction::update(SceneObject * object, float time)
{
	return false;
}

void VideoStopVideoAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
}

void VideoStopVideoAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
}

void VideoStopVideoAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void VideoStopVideoAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
	}
}
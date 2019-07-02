#include "stdafx.h"
#include "VideoObject.h"
#include "VideoPlayVideoAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Global.h"
#include "VideoPlayer.h"
#include "FileUtils.h"
#include "Document.h"
#include "DocumentTemplate.h"

using namespace std;

VideoPlayVideoAction::VideoPlayVideoAction()
{
	startTime_ = 0.0f;
}

VideoPlayVideoAction::~VideoPlayVideoAction()
{	
}

bool VideoPlayVideoAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == VideoObject().type();
}

void VideoPlayVideoAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{
}

int VideoPlayVideoAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	return index;
}

Action & VideoPlayVideoAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (VideoPlayVideoAction &)rhs;
	return *this;
}


bool VideoPlayVideoAction::start(SceneObject * object, float time)
{	
	VideoObject * videoObj = (VideoObject *)targetObject_;
	videoObj->play(startTime_);
	return true;
}

bool VideoPlayVideoAction::hasDuration() const
{
	return false;
}

bool VideoPlayVideoAction::isExecuting() const
{
	return false;
}

bool VideoPlayVideoAction::update(SceneObject * object, float time)
{
	return false;
}

void VideoPlayVideoAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(startTime_);
}

void VideoPlayVideoAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	if (version >= 1)
	{
		reader.read(startTime_);
	}
}

void VideoPlayVideoAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("StartTime", startTime_);
}

void VideoPlayVideoAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(startTime_, curNode, "StartTime"));
	}
}
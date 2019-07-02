#include "stdafx.h"
#include "PlayAudioAction.h"
#include "Animation.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Global.h"
#include "AudioPlayer.h"
#include "FileUtils.h"
#include "Document.h"
#include "DocumentTemplate.h"

using namespace std;

PlayAudioAction::PlayAudioAction()
{	
	startTime_ = 0.0f;
}

PlayAudioAction::~PlayAudioAction()
{	
}


void PlayAudioAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{
	refFiles->push_back(source_);
}

int PlayAudioAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	source_ = refFiles[index++];
	return index;
}

Action & PlayAudioAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (PlayAudioAction &)rhs;
	return *this;
}


bool PlayAudioAction::start(SceneObject * object, float time)
{	
	Scene * scene = object->parentScene();
	Document * document = scene->parentDocument();
	if (!scene->isCurrentScene())
		return true;

	Global::instance().audioPlayer()->play(
		getAbsFileName(Global::instance().readDirectory(), source_), startTime_);
	return true;
}

bool PlayAudioAction::hasDuration() const
{
	return false;
}

bool PlayAudioAction::isExecuting() const
{
	return false;
}

bool PlayAudioAction::update(SceneObject * object, float time)
{
	return false;
}

void PlayAudioAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(source_, "video filename");
	writer.write(startTime_, "startTime");
}

void PlayAudioAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
	reader.read(source_);
	if (version >= 1)
	{
		reader.read(startTime_);
	}

}

void PlayAudioAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Source", source_);
	w.writeTag("startTime", startTime_);
}

void PlayAudioAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(source_, curNode, "Source"));
		else if (r.getNodeContentIfName(startTime_, curNode, "startTime"));
	}
}
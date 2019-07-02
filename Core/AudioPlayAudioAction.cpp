#include "stdafx.h"
#include "AudioObject.h"
#include "AudioPlayAudioAction.h"
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

AudioPlayAudioAction::AudioPlayAudioAction()
{
	startTime_ = 0.0f;
}

AudioPlayAudioAction::~AudioPlayAudioAction()
{	
}

bool AudioPlayAudioAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == AudioObject().type();
}

void AudioPlayAudioAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{
}

int AudioPlayAudioAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	return index;
}

Action & AudioPlayAudioAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (AudioPlayAudioAction &)rhs;
	return *this;
}


bool AudioPlayAudioAction::start(SceneObject * object, float time)
{	
	AudioObject * audioObj = (AudioObject *)targetObject_;
	Scene * scene = object->parentScene();
	Document * document = scene->parentDocument();
	if (!scene->isCurrentScene())
		return true;
	audioObj->play(startTime_);

	return true;
}

bool AudioPlayAudioAction::hasDuration() const
{
	return false;
}

bool AudioPlayAudioAction::isExecuting() const
{
	return false;
}

bool AudioPlayAudioAction::update(SceneObject * object, float time)
{
	return false;
}

void AudioPlayAudioAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(startTime_);
}

void AudioPlayAudioAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	if (version < 1)
	{
		bool unused;
		reader.read(unused);
	}	

	if (version >= 2)
	{
		reader.read(startTime_);
	}
}

void AudioPlayAudioAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("StartTime", startTime_);
}

void AudioPlayAudioAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(startTime_, curNode, "StartTime"));
	}
}
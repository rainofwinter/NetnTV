#include "stdafx.h"
#include "AudioStopAudioAction.h"
#include "AudioObject.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Global.h"
#include "AudioPlayer.h"
#include "FileUtils.h"

using namespace std;

AudioStopAudioAction::AudioStopAudioAction()
{	
}

AudioStopAudioAction::~AudioStopAudioAction()
{	
}

bool AudioStopAudioAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == AudioObject().type();
}

void AudioStopAudioAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{	
}

int AudioStopAudioAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{	
	return index;
}

Action & AudioStopAudioAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (AudioStopAudioAction &)rhs;
	return *this;
}


bool AudioStopAudioAction::start(SceneObject * object, float time)
{
	((AudioObject *)targetObject_)->stop();
	return true;
}

bool AudioStopAudioAction::hasDuration() const
{
	return false;
}

bool AudioStopAudioAction::isExecuting() const
{
	return false;
}

bool AudioStopAudioAction::update(SceneObject * object, float time)
{
	return false;
}

void AudioStopAudioAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
}

void AudioStopAudioAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
}

void AudioStopAudioAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void AudioStopAudioAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
	}
}
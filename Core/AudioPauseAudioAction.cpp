#include "stdafx.h"
#include "AudioPauseAudioAction.h"
#include "AudioObject.h"
#include "Writer.h"
#include "Reader.h"
#include "Global.h"
#include "AudioPlayer.h"

using namespace std;

AudioPauseAudioAction::AudioPauseAudioAction()
{	
}

AudioPauseAudioAction::~AudioPauseAudioAction()
{	
}

bool AudioPauseAudioAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == AudioObject().type();
}

void AudioPauseAudioAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{	
}

int AudioPauseAudioAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{	
	return index;
}

Action & AudioPauseAudioAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (AudioPauseAudioAction &)rhs;
	return *this;
}


bool AudioPauseAudioAction::start(SceneObject * object, float time)
{
	((AudioObject *)targetObject_)->pause();	
	return true;
}

bool AudioPauseAudioAction::hasDuration() const
{
	return false;
}

bool AudioPauseAudioAction::isExecuting() const
{
	return false;
}

bool AudioPauseAudioAction::update(SceneObject * object, float time)
{
	return false;
}

void AudioPauseAudioAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
}

void AudioPauseAudioAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
}

void AudioPauseAudioAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void AudioPauseAudioAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
	}
}
#include "stdafx.h"
#include "AudioResumeAudioAction.h"
#include "AudioObject.h"
#include "Writer.h"
#include "Reader.h"
#include "Global.h"
#include "AudioPlayer.h"

using namespace std;

AudioResumeAudioAction::AudioResumeAudioAction()
{	
}

AudioResumeAudioAction::~AudioResumeAudioAction()
{	
}

bool AudioResumeAudioAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == AudioObject().type();
}

void AudioResumeAudioAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{	
}

int AudioResumeAudioAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{	
	return index;
}

Action & AudioResumeAudioAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (AudioResumeAudioAction &)rhs;
	return *this;
}


bool AudioResumeAudioAction::start(SceneObject * object, float time)
{
	((AudioObject *)targetObject_)->resume();
	return true;
}

bool AudioResumeAudioAction::hasDuration() const
{
	return false;
}

bool AudioResumeAudioAction::isExecuting() const
{
	return false;
}

bool AudioResumeAudioAction::update(SceneObject * object, float time)
{
	return false;
}

void AudioResumeAudioAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
}

void AudioResumeAudioAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
}

void AudioResumeAudioAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void AudioResumeAudioAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
	}
}
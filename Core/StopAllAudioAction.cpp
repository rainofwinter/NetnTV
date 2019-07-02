#include "stdafx.h"
#include "StopAllAudioAction.h"
#include "Animation.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Global.h"
#include "AudioPlayer.h"
#include "FileUtils.h"

using namespace std;

StopAllAudioAction::StopAllAudioAction()
{	
}

StopAllAudioAction::~StopAllAudioAction()
{	
}


void StopAllAudioAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{	
}

int StopAllAudioAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{	
	return index;
}

Action & StopAllAudioAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (StopAllAudioAction &)rhs;
	return *this;
}


bool StopAllAudioAction::start(SceneObject * object, float time)
{
	Global::instance().audioPlayer()->stopAll();
	return true;
}

bool StopAllAudioAction::hasDuration() const
{
	return false;
}

bool StopAllAudioAction::isExecuting() const
{
	return false;
}

bool StopAllAudioAction::update(SceneObject * object, float time)
{
	return false;
}

void StopAllAudioAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
}

void StopAllAudioAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
}

void StopAllAudioAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void StopAllAudioAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
	}
}
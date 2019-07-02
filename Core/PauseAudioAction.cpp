#include "stdafx.h"
#include "PauseAudioAction.h"
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

PauseAudioAction::PauseAudioAction()
{	
}

PauseAudioAction::~PauseAudioAction()
{	
}


void PauseAudioAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{	
}

int PauseAudioAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{	
	return index;
}

Action & PauseAudioAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (PauseAudioAction &)rhs;
	return *this;
}


bool PauseAudioAction::start(SceneObject * object, float time)
{
	Global::instance().audioPlayer()->pauseAll();
	return true;
}

bool PauseAudioAction::hasDuration() const
{
	return false;
}

bool PauseAudioAction::isExecuting() const
{
	return false;
}

bool PauseAudioAction::update(SceneObject * object, float time)
{
	return false;
}

void PauseAudioAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
}

void PauseAudioAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
}


void PauseAudioAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void PauseAudioAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
	}
}
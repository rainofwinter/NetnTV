#include "stdafx.h"
#include "ResumeAudioAction.h"
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

ResumeAudioAction::ResumeAudioAction()
{	
}

ResumeAudioAction::~ResumeAudioAction()
{	
}


void ResumeAudioAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{	
}

int ResumeAudioAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{	
	return index;
}

Action & ResumeAudioAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (ResumeAudioAction &)rhs;
	return *this;
}


bool ResumeAudioAction::start(SceneObject * object, float time)
{
	Global::instance().audioPlayer()->resumeAll();
	return true;
}

bool ResumeAudioAction::hasDuration() const
{
	return false;
}

bool ResumeAudioAction::isExecuting() const
{
	return false;
}

bool ResumeAudioAction::update(SceneObject * object, float time)
{
	return false;
}

void ResumeAudioAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
}

void ResumeAudioAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
}

void ResumeAudioAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void ResumeAudioAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
	}
}
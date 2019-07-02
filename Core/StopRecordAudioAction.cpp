#include "stdafx.h"
#include "StopRecordAudioAction.h"
#include "Writer.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Global.h"
#include "AudioPlayer.h"

using namespace std;

StopRecordAudioAction::StopRecordAudioAction()
{	
}

StopRecordAudioAction::~StopRecordAudioAction()
{	
}


void StopRecordAudioAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{
	Action::referencedFiles(refFiles);
}

int StopRecordAudioAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	return Action::setReferencedFiles(baseDirectory, refFiles, index);
}

Action & StopRecordAudioAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (StopRecordAudioAction &)rhs;
	return *this;
}


bool StopRecordAudioAction::start(SceneObject * object, float time)
{
	Global::instance().audioPlayer()->endRecord();
	return true;
}

bool StopRecordAudioAction::hasDuration() const
{
	return false;
}

bool StopRecordAudioAction::isExecuting() const
{
	return false;
}

bool StopRecordAudioAction::update(SceneObject * object, float time)
{
	return false;
}

void StopRecordAudioAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
}

void StopRecordAudioAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}

void StopRecordAudioAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void StopRecordAudioAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
	}
}
#include "stdafx.h"
#include "StartRecordAudioAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Global.h"
#include "AudioPlayer.h"
#include "FileUtils.h"

using namespace std;

StartRecordAudioAction::StartRecordAudioAction()
{	
}

StartRecordAudioAction::~StartRecordAudioAction()
{	
}


void StartRecordAudioAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{
	Action::referencedFiles(refFiles);
}

int StartRecordAudioAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	return Action::setReferencedFiles(baseDirectory, refFiles, index);
}

Action & StartRecordAudioAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (StartRecordAudioAction &)rhs;
	return *this;
}


bool StartRecordAudioAction::start(SceneObject * object, float time)
{	
	Scene * scene = object->parentScene();
	Document * document = scene->parentDocument();
	if (!scene->isCurrentScene())
		return true;

	Global::instance().audioPlayer()->startRecord();
	return true;
}

bool StartRecordAudioAction::hasDuration() const
{
	return false;
}

bool StartRecordAudioAction::isExecuting() const
{
	return false;
}

bool StartRecordAudioAction::update(SceneObject * object, float time)
{
	return false;
}

void StartRecordAudioAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);

}

void StartRecordAudioAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}

void StartRecordAudioAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void StartRecordAudioAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
	}
}
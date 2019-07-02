#include "stdafx.h"
#include "StopVideoAction.h"
#include "Animation.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Global.h"
#include "VideoPlayer.h"
#include "FileUtils.h"
#include "Document.h"
#include "DocumentTemplate.h"

using namespace std;

StopVideoAction::StopVideoAction()
{	
}

StopVideoAction::~StopVideoAction()
{	
}


void StopVideoAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{	
}

int StopVideoAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{	
	return index;
}

Action & StopVideoAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (StopVideoAction &)rhs;
	return *this;
}


bool StopVideoAction::start(SceneObject * object, float time)
{
	Scene * scene = object->parentScene();
	Document * document = scene->parentDocument();
	if (!scene->isCurrentScene())
		return true;

	Global::instance().videoPlayer()->deactivate();
	return true;
}

bool StopVideoAction::hasDuration() const
{
	return false;
}

bool StopVideoAction::isExecuting() const
{
	return false;
}

bool StopVideoAction::update(SceneObject * object, float time)
{
	return false;
}

void StopVideoAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
}

void StopVideoAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
}

void StopVideoAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void StopVideoAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
	}
}
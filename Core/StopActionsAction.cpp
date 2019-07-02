#include "stdafx.h"
#include "StopActionsAction.h"
#include "Scene.h"
#include "Reader.h"
#include "Xml.h"
#include "Writer.h"
#include "SceneObject.h"
#include "EventListener.h"

using namespace std;

StopActionsAction::StopActionsAction()
{	
}

StopActionsAction::~StopActionsAction()
{	
}


void StopActionsAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{	
}

int StopActionsAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{	
	return index;
}

Action & StopActionsAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;

	*this = (StopActionsAction &)rhs;
	return *this;
}


bool StopActionsAction::start(SceneObject * object, float time)
{
	Scene * scene = object->parentScene();
	Document * document = scene->parentDocument();
	
	std::vector<EventListenerSPtr> listeners;
	scene->allListeners(&listeners);

	BOOST_FOREACH(EventListenerSPtr listener, listeners)
	{
		listener->stop();
	}
	
	
	return true;
}

bool StopActionsAction::hasDuration() const
{
	return false;
}

bool StopActionsAction::isExecuting() const
{
	return false;
}

bool StopActionsAction::update(SceneObject * object, float time)
{
	return false;
}

void StopActionsAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
}

void StopActionsAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
}

void StopActionsAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void StopActionsAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
	}
}
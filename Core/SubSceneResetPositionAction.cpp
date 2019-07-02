#include "stdafx.h"
#include "SubSceneResetPositionAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "SubScene.h"
#include "ElementMapping.h"

SubsceneResetPositionAction::SubsceneResetPositionAction()
{
}

SubsceneResetPositionAction::~SubsceneResetPositionAction()
{	
}

bool SubsceneResetPositionAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

Action & SubsceneResetPositionAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (SubsceneResetPositionAction &)rhs;
	return *this;
}


bool SubsceneResetPositionAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == Subscene().type();
}

bool SubsceneResetPositionAction::start(SceneObject * object, float time)
{	
	Subscene * obj = (Subscene *)targetObject_;
	obj->setScrollPosition(Vector2(0, 0));
	obj->setDragPosition(Vector2(0, 0));
	return true;
}

bool SubsceneResetPositionAction::hasDuration() const
{
	return false;
}

bool SubsceneResetPositionAction::isExecuting() const
{
	return false;
}

bool SubsceneResetPositionAction::update(SceneObject * object, float time)
{
	return false;
}

void SubsceneResetPositionAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	
}

void SubsceneResetPositionAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}

void SubsceneResetPositionAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
}

void SubsceneResetPositionAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
	}
}
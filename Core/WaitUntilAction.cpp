#include "stdafx.h"
#include "WaitUntilAction.h"
#include "Animation.h"
#include "Scene.h"
#include "Group.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"

WaitUntilAction::WaitUntilAction()
{
	waitUntilTime_ = 0;
	delayedTime_ = 0;
	isExecuting_ = false;
}

WaitUntilAction::~WaitUntilAction()
{	
}

Action & WaitUntilAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (WaitUntilAction &)rhs;
	return *this;
}

bool WaitUntilAction::supportsObject(SceneObject * obj) const
{
	return !obj;
}

bool WaitUntilAction::start(SceneObject * object, float time)
{
	//delayedTime_ = 0;
	isExecuting_ = true;
	return false;
}

bool WaitUntilAction::hasDuration() const
{
	return true;
}

bool WaitUntilAction::isExecuting() const
{
	return isExecuting_;
}

bool WaitUntilAction::update(SceneObject * object, float time)
{
	if (time >= waitUntilTime_ + delayedTime_)
	{
		delayedTime_ = 0;
		isExecuting_ = false;
	}
	return false;
}

void WaitUntilAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(waitUntilTime_, "waitUntilTime");	
}

void WaitUntilAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
	reader.read(waitUntilTime_);
}

void WaitUntilAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("WaitUntilTime", waitUntilTime_);
}

void WaitUntilAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(waitUntilTime_, curNode, "WaitUntilTime"));
	}
}
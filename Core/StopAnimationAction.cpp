#include "stdafx.h"
#include "StopAnimationAction.h"
#include "Animation.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"

#include "ElementMapping.h"

StopAnimationAction::StopAnimationAction()
{
	animation_ = 0;
}

StopAnimationAction::~StopAnimationAction()
{	
}

bool StopAnimationAction::remapReferences(const ElementMapping & mapping)
{
	animation_ = mapping.mapAnimation(animation_);
	return animation_ != 0 && Action::remapReferences(mapping);
}

Action & StopAnimationAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (StopAnimationAction &)rhs;
	return *this;
}


bool StopAnimationAction::start(SceneObject * object, float time)
{
	object->parentScene()->stop(animation_);
	//animation_->update(0);	
	return true;
}

bool StopAnimationAction::hasDuration() const
{
	return false;
}

bool StopAnimationAction::isExecuting() const
{
	return false;
}

bool StopAnimationAction::update(SceneObject * object, float time)
{
	return false;
}

void StopAnimationAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(animation_, "animation");	
}

void StopAnimationAction::read(Reader & reader, unsigned char version)
{	
	reader.readParent<Action>(this);
	reader.read(animation_);
}

void StopAnimationAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Animation", animation_);
}

void StopAnimationAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(val, curNode, "Animation"))
		{
			animation_ = (Animation *)val;
		}
	}
}
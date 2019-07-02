#include "stdafx.h"
#include "ResumeAnimationAction.h"
#include "Animation.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ElementMapping.h"

ResumeAnimationAction::ResumeAnimationAction()
{
	animation_ = 0;
}

ResumeAnimationAction::~ResumeAnimationAction()
{	
}

bool ResumeAnimationAction::remapReferences(const ElementMapping & mapping)
{
	animation_ = mapping.mapAnimation(animation_);
	return animation_ != 0 && Action::remapReferences(mapping);
}

Action & ResumeAnimationAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (ResumeAnimationAction &)rhs;
	return *this;
}


bool ResumeAnimationAction::start(SceneObject * object, float time)
{
	object->parentScene()->resume(animation_);
	return true;
}

bool ResumeAnimationAction::hasDuration() const
{
	return false;
}

bool ResumeAnimationAction::isExecuting() const
{
	return false;
}

bool ResumeAnimationAction::update(SceneObject * object, float time)
{
	return true;
}

void ResumeAnimationAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(animation_, "animation");	
}

void ResumeAnimationAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	reader.read(animation_);
}

void ResumeAnimationAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Animation", animation_);
}

void ResumeAnimationAction::readXml(XmlReader & r, xmlNode * parent)
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
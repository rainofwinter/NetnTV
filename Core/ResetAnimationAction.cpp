#include "stdafx.h"
#include "ResetAnimationAction.h"
#include "Animation.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"

#include "ElementMapping.h"

ResetAnimationAction::ResetAnimationAction()
{
	animation_ = 0;
	resetType_ = Any;
}

ResetAnimationAction::~ResetAnimationAction()
{	
}

bool ResetAnimationAction::remapReferences(const ElementMapping & mapping)
{
	if (resetType_ == Any)
	{
		animation_ = mapping.mapAnimation(animation_);
		return animation_ != 0 && Action::remapReferences(mapping);
	}
	else
		return true;
}

Action & ResetAnimationAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (ResetAnimationAction &)rhs;
	return *this;
}


bool ResetAnimationAction::start(SceneObject * object, float time)
{
	if (resetType_ == Any)
	{
		object->parentScene()->stop(animation_);
		animation_->update(0);	
	}
	else
	{
		std::vector<AnimationSPtr> anims = object->parentScene()->animations();
		BOOST_FOREACH(const AnimationSPtr & anim, anims)
		{
			object->parentScene()->stop(anim.get());
			anim->update(0);	
		}
	}
	return true;
}

bool ResetAnimationAction::hasDuration() const
{
	return false;
}

bool ResetAnimationAction::isExecuting() const
{
	return false;
}

bool ResetAnimationAction::update(SceneObject * object, float time)
{
	return false;
}

void ResetAnimationAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(animation_, "animation");
	writer.write(resetType_, "resetType");
}

void ResetAnimationAction::read(Reader & reader, unsigned char version)
{	
	reader.readParent<Action>(this);
	reader.read(animation_);
	if (version >= 1)
	{
		reader.read(resetType_);
	}
}

void ResetAnimationAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Animation", animation_);
	w.writeTag("ResetType", (unsigned int)resetType_);
}

void ResetAnimationAction::readXml(XmlReader & r, xmlNode * parent)
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
		else if (r.getNodeContentIfName(val, curNode, "ResetType")) resetType_ = (ResetType)val;
	}
}
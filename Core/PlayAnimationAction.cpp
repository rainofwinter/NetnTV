#include "stdafx.h"
#include "PlayAnimationAction.h"
#include "Animation.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Xml.h"

PlayAnimationAction::PlayAnimationAction()
{
	animation_ = 0;
}

PlayAnimationAction::~PlayAnimationAction()
{	
}

bool PlayAnimationAction::remapReferences(const ElementMapping & mapping)
{
	animation_ = mapping.mapAnimation(animation_);
	return animation_ != 0 && Action::remapReferences(mapping);
}

Action & PlayAnimationAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (PlayAnimationAction &)rhs;
	return *this;
}


bool PlayAnimationAction::start(SceneObject * object, float time)
{
	object->parentScene()->play(animation_);
	return true;
}

bool PlayAnimationAction::hasDuration() const
{
	return false;
}

bool PlayAnimationAction::isExecuting() const
{
	return false;
}

bool PlayAnimationAction::update(SceneObject * object, float time)
{
	return true;
}

void PlayAnimationAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(animation_, "animation");	
}

void PlayAnimationAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	reader.read(animation_);
}

void PlayAnimationAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Animation", animation_);
}

void PlayAnimationAction::readXml(XmlReader & r, xmlNode * parent)
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
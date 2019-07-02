#include "stdafx.h"
#include "ShowOnlyAction.h"
#include "Animation.h"
#include "Scene.h"
#include "Group.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Reader.h"
#include "Xml.h"

#include "ElementMapping.h"

ShowOnlyAction::ShowOnlyAction()
{
	showObject_ = 0;
}

ShowOnlyAction::~ShowOnlyAction()
{	
}

bool ShowOnlyAction::dependsOn(SceneObject * obj) const 
{
	if (Action::dependsOn(obj)) return true;	
	
	SceneObject * curObj = showObject_;
	while (curObj)
	{
		if (curObj == obj) return true;
		curObj = curObj->parent();
	}

	return false;
}

bool ShowOnlyAction::dependsOn(
	SceneObject * parent, SceneObject * child) const
{
	if (parent == targetObject_ && child == showObject_)
		return true;

	return false;
}

bool ShowOnlyAction::remapReferences(const ElementMapping & mapping)
{
	showObject_ = mapping.mapObject(showObject_);
	return showObject_ != 0 && Action::remapReferences(mapping);
}

Action & ShowOnlyAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (ShowOnlyAction &)rhs;
	return *this;
}

bool ShowOnlyAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->group();
}

bool ShowOnlyAction::start(SceneObject * object, float time)
{
	Group * grp = targetObject_->group();
	BOOST_FOREACH(const SceneObjectSPtr & child, grp->children())
	{
		VisualAttrib * attr = child->visualAttrib();
		if (!attr) continue;
		if (child.get() == showObject_) attr->setVisible(true);
		else attr->setVisible(false);
	}

	return true;
}

bool ShowOnlyAction::hasDuration() const
{
	return false;
}

bool ShowOnlyAction::isExecuting() const
{
	return false;
}

bool ShowOnlyAction::update(SceneObject * object, float time)
{
	return true;
}

void ShowOnlyAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(showObject_, "showObject");	
}

void ShowOnlyAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	reader.read(showObject_);
}

void ShowOnlyAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("ShowObject", w.idMapping().getId(showObject_));
}

void ShowOnlyAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(val, curNode, "ShowObject")) showObject_ = r.idMapping().getSceneObject(val);
	}
}
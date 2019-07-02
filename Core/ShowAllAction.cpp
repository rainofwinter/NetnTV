#include "stdafx.h"
#include "ShowAllAction.h"
#include "Animation.h"
#include "Scene.h"
#include "Group.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Reader.h"
#include "Xml.h"

ShowAllAction::ShowAllAction()
{
	visible_ = true;
}

ShowAllAction::~ShowAllAction()
{	
}

Action & ShowAllAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (ShowAllAction &)rhs;
	return *this;
}

bool ShowAllAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->group();
}

bool ShowAllAction::start(SceneObject * object, float time)
{
	Group * grp = targetObject_->group();
	BOOST_FOREACH(const SceneObjectSPtr & child, grp->children())
	{
		VisualAttrib * attr = child->visualAttrib();
		if (!attr) continue;
		attr->setVisible(visible_);
	}
	return true;
}

bool ShowAllAction::hasDuration() const
{
	return false;
}

bool ShowAllAction::isExecuting() const
{
	return false;
}

bool ShowAllAction::update(SceneObject * object, float time)
{
	return false;
}

void ShowAllAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(visible_);
}

void ShowAllAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);

	if (version >= 1)
		reader.read(visible_);
}

void ShowAllAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Visible", visible_);
}

void ShowAllAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(visible_, curNode, "Visible"));
	}
}
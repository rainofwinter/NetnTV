#include "stdafx.h"
#include "SetOpacityAction.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Reader.h"
#include "Global.h"
#include "Attrib.h"
#include "Xml.h"

using namespace std;

SetOpacityAction::SetOpacityAction()
{	
	opacity_ = 0.0f;
}

SetOpacityAction::~SetOpacityAction()
{	
}

bool SetOpacityAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->visualAttrib();
}

void SetOpacityAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{
	Action::referencedFiles(refFiles);
}

int SetOpacityAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	return Action::setReferencedFiles(baseDirectory, refFiles, index);
}

Action & SetOpacityAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (SetOpacityAction &)rhs;
	return *this;
}


bool SetOpacityAction::start(SceneObject * object, float time)
{
	targetObject_->visualAttrib()->setOpacity(opacity_);
	return true;
}

bool SetOpacityAction::hasDuration() const
{
	return false;
}

bool SetOpacityAction::isExecuting() const
{
	return false;
}

bool SetOpacityAction::update(SceneObject * object, float time)
{
	return false;
}

void SetOpacityAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(opacity_, "opacity");
}

void SetOpacityAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
	reader.read(opacity_);
}

void SetOpacityAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Opacity", opacity_);
}

void SetOpacityAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(opacity_, curNode, "Opacity"));
	}
}
#include "stdafx.h"
#include "SetVisibilityAction.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Reader.h"
#include "Global.h"
#include "Attrib.h"
#include "Xml.h"
using namespace std;

SetVisibilityAction::SetVisibilityAction()
{	
	visibility_ = true;
}

SetVisibilityAction::~SetVisibilityAction()
{	
}


bool SetVisibilityAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->visualAttrib();
}

void SetVisibilityAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{
	Action::referencedFiles(refFiles);
}

int SetVisibilityAction::setReferencedFiles(
	const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	return Action::setReferencedFiles(baseDirectory, refFiles, index);
}

Action & SetVisibilityAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (SetVisibilityAction &)rhs;
	return *this;
}


bool SetVisibilityAction::start(SceneObject * object, float time)
{
	targetObject_->visualAttrib()->setVisible(visibility_);
	return true;
}

bool SetVisibilityAction::hasDuration() const
{
	return false;
}

bool SetVisibilityAction::isExecuting() const
{
	return false;
}

bool SetVisibilityAction::update(SceneObject * object, float time)
{
	return false;
}

void SetVisibilityAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(visibility_, "visibility");
}

void SetVisibilityAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
	reader.read(visibility_);
}

void SetVisibilityAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Visibility", visibility_);
}

void SetVisibilityAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(visibility_, curNode, "Visibility"));
	}
}
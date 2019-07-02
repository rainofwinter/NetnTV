#include "stdafx.h"
#include "VibrateAction.h"
#include "Animation.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Global.h"
#include "VideoPlayer.h"
#include "FileUtils.h"

using namespace std;

VibrateAction::VibrateAction()
{
	time_ = 1000;
}

VibrateAction::~VibrateAction()
{	
}

Action & VibrateAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (VibrateAction &)rhs;
	return *this;
}


bool VibrateAction::start(SceneObject * object, float time)
{
	Scene * scene = object->parentScene();
	if (!scene->isCurrentScene()) return true;

	Global::vibrateAction(time_);

	return true;
}

bool VibrateAction::hasDuration() const
{
	return false;
}

bool VibrateAction::isExecuting() const
{
	return false;
}

bool VibrateAction::update(SceneObject * object, float time)
{
	return false;
}

void VibrateAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(time_, "vibrateTime");	
	
}

void VibrateAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	reader.read(time_);
}

void VibrateAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("VibrateTime", time_);
}

void VibrateAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(time_, curNode, "VibrateTime"));
	}
}
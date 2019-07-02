#include "stdafx.h"
#include "LaunchAppAction.h"
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

LaunchAppAction::LaunchAppAction()
{
}

LaunchAppAction::~LaunchAppAction()
{	
}

Action & LaunchAppAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (LaunchAppAction &)rhs;
	return *this;
}


bool LaunchAppAction::start(SceneObject * object, float time)
{
	Scene * scene = object->parentScene();
	if (!scene->isCurrentScene()) return true;

#ifdef ANDROID
	Global::launchApp(androidParams_);
#elif defined IOS
	Global::launchApp(iosParams_);
#else
	Global::launchApp("");
#endif

	return true;
}

bool LaunchAppAction::hasDuration() const
{
	return false;
}

bool LaunchAppAction::isExecuting() const
{
	return false;
}

bool LaunchAppAction::update(SceneObject * object, float time)
{
	return false;
}

void LaunchAppAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(androidParams_, "androidParams");
	writer.write(iosParams_, "iosParams");	
	
}

void LaunchAppAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	reader.read(androidParams_);
	reader.read(iosParams_);	
}

void LaunchAppAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("AndroidParams", androidParams_);
	w.writeTag("IOSParams", iosParams_);
}

void LaunchAppAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(androidParams_, curNode, "AndroidParams"));
		else if (r.getNodeContentIfName(iosParams_, curNode, "IOSParams"));
	}
}
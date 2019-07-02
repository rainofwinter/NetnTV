#include "stdafx.h"
#include "BroadcastMessageAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Document.h"
#include "Global.h"
#include "ReceiveMessageEvent.h"

using namespace std;

BroadcastMessageAction::BroadcastMessageAction()
{	
	scene_ = 0;
	targetType_ = SceneTarget;
}

BroadcastMessageAction::~BroadcastMessageAction()
{	
}

bool BroadcastMessageAction::dependsOn(Scene * scene) const 
{
	if (targetType_ == BroadcastMessageAction::ExternalTarget) return false;
	else return scene == scene_;
}

Action & BroadcastMessageAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (BroadcastMessageAction &)rhs;
	return *this;
}

bool BroadcastMessageAction::remapReferences(const ElementMapping & mapping)
{
	scene_ = mapping.mapScene(scene_);
	return 
		((targetType_ == ExternalTarget) || scene_ != 0) && 
		Action::remapReferences(mapping);
}

bool BroadcastMessageAction::start(SceneObject * object, float time)
{
	Scene * scene = object->parentScene();

	/*
	Allow BroadcastMessageAction to be executed even if the parent scene isn't
	current: 
	For example: after a scene set action, to send a message to the new current scene.

	Beware of regressions from this.
	*/
	/*
	if (!scene->isCurrentScene())
		return true;
	*/

	Document * document = scene->parentDocument();	

	if (targetType_ == SceneTarget)
		document->broadcastMessage(scene_, message_);
	else
	{
		document->broadcastExternalMessage(message_);
	}
	return true;
}

bool BroadcastMessageAction::hasDuration() const
{
	return false;
}

bool BroadcastMessageAction::isExecuting() const
{
	return false;
}

bool BroadcastMessageAction::update(SceneObject * object, float time)
{
	return false;
}

void BroadcastMessageAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(message_, "message");	
	writer.write(scene_, "scene");	
	writer.write(targetType_, "targetType");
}

void BroadcastMessageAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	reader.read(message_);
	if (version >= 1)
	{
		reader.read(scene_);
	}

	if (version >= 2)
	{
		reader.read(targetType_);
	}
}

void BroadcastMessageAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Message", message_);
	w.writeTag("Scene", w.idMapping().getId(scene_));
	w.writeTag("TargetType", (unsigned int)targetType_);
}

void BroadcastMessageAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		
		if (r.getNodeContentIfName(message_, curNode, "Message"));
		else if (r.getNodeContentIfName(val, curNode, "Scene"))	scene_ = r.idMapping().getScene(val);
		else if (r.getNodeContentIfName(val, curNode, "Message")) targetType_ = (TargetType)val;
	}
}

bool BroadcastMessageAction::startAppObj(AppObject * object, float time)
{
	Document * document = Global::instance().curDocument();	

	document->broadcastExternalMessage(message_);

	return true;
}

bool BroadcastMessageAction::updateAppObj(AppObject * object, float time)
{
	return false;
}
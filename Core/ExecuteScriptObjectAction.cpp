#include "stdafx.h"
#include "ExecuteScriptObjectAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Document.h"
#include "Global.h"

using namespace std;

ExecuteScriptObjectAction::ExecuteScriptObjectAction()
{	
	scene_ = 0;
	targetType_ = SceneTarget;
}

ExecuteScriptObjectAction::~ExecuteScriptObjectAction()
{	
}

bool ExecuteScriptObjectAction::dependsOn(Scene * scene) const 
{
	if (targetType_ == ExecuteScriptObjectAction::ExternalTarget) return false;
	else return scene == scene_;
}

Action & ExecuteScriptObjectAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (ExecuteScriptObjectAction &)rhs;
	return *this;
}

bool ExecuteScriptObjectAction::remapReferences(const ElementMapping & mapping)
{
	scene_ = mapping.mapScene(scene_);
	return 
		((targetType_ == ExternalTarget) || scene_ != 0) && 
		Action::remapReferences(mapping);
}

bool ExecuteScriptObjectAction::start(SceneObject * object, float time)
{
	Document * document = object->parentScene()->parentDocument();
	if (targetType_ == SceneTarget)
		document->broadcastMessage(scene_, message_);
	else
		Global::instance().broadcastExternalMessage(message_);
	return true;
}

bool ExecuteScriptObjectAction::hasDuration() const
{
	return false;
}

bool ExecuteScriptObjectAction::isExecuting() const
{
	return false;
}

bool ExecuteScriptObjectAction::update(SceneObject * object, float time)
{
	return false;
}

void ExecuteScriptObjectAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(message_, "message");	
	writer.write(scene_, "scene");	
	writer.write(targetType_, "targetType");
}

void ExecuteScriptObjectAction::read(Reader & reader, unsigned char version)
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

void ExecuteScriptObjectAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Message", message_);
	w.writeTag("Scene", w.idMapping().getId(scene_));
	w.writeTag("TargetType", (unsigned int)targetType_);
}

void ExecuteScriptObjectAction::readXml(XmlReader & r, xmlNode * parent)
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
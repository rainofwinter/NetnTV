#include "stdafx.h"
#include "SceneChangeAction.h"
#include "Image.h"
#include "Scene.h"
#include "Group.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "Document.h"
#include "PageFlip.h"
#include "ElementMapping.h"
#include "SceneContainer.h"
#include "Global.h"
#include "DocumentTemplate.h"

enum EffectSceneChangeV0
{		
	PageFlipEffect = 0,
	FadeOutEffect		
};

SceneChangeAction::SceneChangeAction()
{
	scene_ = 0;
	mode_ = Specific;
	direction_ = Next;
}

SceneChangeAction::~SceneChangeAction()
{	
}

bool SceneChangeAction::dependsOn(SceneObject * obj) const 
{
	/*
	SceneObject * curObj = targetObject_;
	while (curObj)
	{
		if (curObj == obj) return true;
		curObj = curObj->parent();
	}*/

	return false;	
}

bool SceneChangeAction::dependsOn(Scene * scene) const
{
	if (mode_ == Adjacent)
	{
		return false;
	}
	else
		return scene == scene_;
}

Action & SceneChangeAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (SceneChangeAction &)rhs;
	return *this;
}

bool SceneChangeAction::remapReferences(const ElementMapping & mapping)
{
	document_ = mapping.destinationDocument();
	scene_ = mapping.mapScene(scene_);
	return (mode_ == Adjacent || scene_ != 0) && Action::remapReferences(mapping);
}

bool SceneChangeAction::supportsObject(SceneObject * obj) const
{
	return !obj;
}

bool SceneChangeAction::start(SceneObject * object, float time)
{	
	if (!object->parentScene()->isCurrentScene()) return false;

	Scene * scene = NULL;
	if (mode_ == Specific)
		scene = scene_;
	else		
	{
		if (direction_ == Previous)		
			scene = document_->documentTemplate()->prevScene();		
		else		
			scene = document_->documentTemplate()->nextScene();	
	}

	if (scene)
		document_->sceneChangeTo(scene);
	
	return true;
}

bool SceneChangeAction::hasDuration() const
{
	return false;
}

bool SceneChangeAction::isExecuting() const
{
	return false;
}

bool SceneChangeAction::update(SceneObject * object, float time)
{
	return false;
}

void SceneChangeAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(mode_, "mode");
	writer.write(direction_, "direction");
	writer.write(scene_, "scene");	
	
}

void SceneChangeAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	if (version < 3)
		reader.read(document_);
	else
		document_ = Global::instance().curDocument();
	
	if (version >= 2)
	{
		reader.read(mode_);
		reader.read(direction_);
		reader.read(scene_);
	}
	else
	{			
		reader.read(scene_);

		if (version == 0)
		{		
			EffectSceneChangeV0 effect;
			float duration;

			reader.read(effect);
			reader.read(duration);
		}
	}
}

void SceneChangeAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Mode", (unsigned int)mode_);
	w.writeTag("Direction", (unsigned int)direction_);
	w.writeTag("Scene", w.idMapping().getId(scene_));
}

void SceneChangeAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(val, curNode, "Mode")) mode_ = (ChangeMode)val;
		else if (r.getNodeContentIfName(val, curNode, "Direction")) direction_ = (Direction)val;
		else if(r.getNodeContentIfName(val, curNode, "Scene"))
		{
			scene_ = r.idMapping().getScene(val);
		}
	}

	document_ = Global::instance().curDocument();
}
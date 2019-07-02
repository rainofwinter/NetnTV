#include "stdafx.h"
#include "SceneChangerShowSceneAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "SceneChanger.h"
#include "ElementMapping.h"

SceneChangerShowSceneAction::SceneChangerShowSceneAction()
{
	scene_ = 0;
	mode_ = Specific;
	direction_ = Next;
}

SceneChangerShowSceneAction::~SceneChangerShowSceneAction()
{	
}

bool SceneChangerShowSceneAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

bool SceneChangerShowSceneAction::dependsOn(Scene * scene) const
{
	if (mode_ == Adjacent)
	{
		return false;
	}
	else 
		return scene == scene_;
}

Action & SceneChangerShowSceneAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (SceneChangerShowSceneAction &)rhs;
	return *this;
}

bool SceneChangerShowSceneAction::remapReferences(const ElementMapping & mapping)
{
	scene_ = mapping.mapScene(scene_);
	return (mode_ == Adjacent || scene_ != 0) && Action::remapReferences(mapping);
}

bool SceneChangerShowSceneAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == SceneChanger().type();
}

bool SceneChangerShowSceneAction::start(SceneObject * object, float time)
{
	//Scene * parentScene = object->parentScene();
	//if (!parentScene->isCurrentScene()) return false;

	SceneChanger * obj = (SceneChanger *)targetObject_;

	Scene * scene = NULL;
	if (mode_ == Specific)
		scene = scene_;
	else		
	{
		if (direction_ == Previous)		
			scene = obj->prevScene();		
		else		
			scene = obj->nextScene();	
	}

	if (scene) obj->sceneChangeTo(scene);
	return true;
}

bool SceneChangerShowSceneAction::hasDuration() const
{
	return true;
}

bool SceneChangerShowSceneAction::isExecuting() const
{
	return false;
}

bool SceneChangerShowSceneAction::update(SceneObject * object, float time)
{
	return false;
}

void SceneChangerShowSceneAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(mode_, "mode");
	writer.write(direction_, "direction");
	writer.write(scene_, "scene");
	
}

void SceneChangerShowSceneAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	if (version < 1)
	{
		reader.read(scene_);
	}
	else
	{			
		reader.read(mode_);
		reader.read(direction_);
		reader.read(scene_);
	}
}

void SceneChangerShowSceneAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Mode", (unsigned int)mode_);
	w.writeTag("Direction", (unsigned int)direction_);
	w.writeTag("Scene", w.idMapping().getId(scene_));
}

void SceneChangerShowSceneAction::readXml(XmlReader & r, xmlNode * parent)
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
}
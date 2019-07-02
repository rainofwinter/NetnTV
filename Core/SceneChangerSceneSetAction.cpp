#include "stdafx.h"
#include "SceneChangerSceneSetAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "SceneChanger.h"
#include "ElementMapping.h"

SceneChangerSceneSetAction::SceneChangerSceneSetAction()
{
	scene_ = 0;
	mode_ = Specific;
	direction_ = Next;
}

SceneChangerSceneSetAction::~SceneChangerSceneSetAction()
{	
}

bool SceneChangerSceneSetAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

bool SceneChangerSceneSetAction::dependsOn(Scene * scene) const
{
	if (mode_ == Adjacent)
	{
		return false;
	}
	else 
		return scene == scene_;
}

Action & SceneChangerSceneSetAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (SceneChangerSceneSetAction &)rhs;
	return *this;
}

bool SceneChangerSceneSetAction::remapReferences(const ElementMapping & mapping)
{
	scene_ = mapping.mapScene(scene_);
	return (mode_ == Adjacent || scene_ != 0) && Action::remapReferences(mapping);
}

bool SceneChangerSceneSetAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == SceneChanger().type();
}

bool SceneChangerSceneSetAction::start(SceneObject * object, float time)
{	
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

	if (scene) obj->sceneSet(scene);
	return true;
}

bool SceneChangerSceneSetAction::hasDuration() const
{
	return false;
}

bool SceneChangerSceneSetAction::isExecuting() const
{
	return false;
}

bool SceneChangerSceneSetAction::update(SceneObject * object, float time)
{
	return false;
}

void SceneChangerSceneSetAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(mode_, "mode");
	writer.write(direction_, "direction");
	writer.write(scene_, "scene");
	
}

void SceneChangerSceneSetAction::read(Reader & reader, unsigned char version)
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

void SceneChangerSceneSetAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Mode", (unsigned int)mode_);
	w.writeTag("Direction", (unsigned int)direction_);
	w.writeTag("Scene", w.idMapping().getId(scene_));
}

void SceneChangerSceneSetAction::readXml(XmlReader & r, xmlNode * parent)
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
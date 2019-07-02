#include "stdafx.h"
#include "ElementMapping.h"
#include "Document.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Animation.h"
#include "AppObject.h"

using namespace std;

ElementMapping::ElementMapping()
:
document_(0), scene_(0)
{
}

ElementMapping::~ElementMapping()
{
}


void ElementMapping::setDestinationData(Document * document, Scene * scene)
{
	document_ = document;
	scene_ = scene;
}

SceneObject * ElementMapping::mapObject(SceneObject * object) const
{
	map<SceneObject *, SceneObject *>::const_iterator iter = objMap_.find(object);
	if (iter == objMap_.end()) 
	{
		if (!object) return 0;
		if (scene_ && object->parentScene() == scene_) return object;		
		else 
		{
			return 0;
		}
	}	
	else return (*iter).second;

}

AppObject * ElementMapping::mapAppObject(AppObject * object) const
{
	map<AppObject *, AppObject *>::const_iterator iter = appObjMap_.find(object);
	if (iter == appObjMap_.end()) return 0;
	else return (*iter).second;

}

Scene * ElementMapping::mapScene(Scene * scene) const
{
	if (document_ && document_->findScene(scene)) return scene;
	map<Scene *, Scene *>::const_iterator iter = sceneMap_.find(scene);
	if (iter == sceneMap_.end()) return 0;
	return (*iter).second;
	
}

Animation * ElementMapping::mapAnimation(Animation * animation) const
{
	if (scene_ && scene_->findAnimation(animation)) return animation;
	map<Animation *, Animation *>::const_iterator iter = animationMap_.find(animation);
	if (iter == animationMap_.end()) return 0;
	return (*iter).second;
}

void ElementMapping::addAnimationMapping(Animation * key, Animation * value)
{
	animationMap_[key] = value;
}

void ElementMapping::addSceneMapping(Scene * key, Scene * value)
{
	sceneMap_[key] = value;
}

void ElementMapping::addObjectMapping(SceneObject * key, SceneObject * value)
{
	objMap_[key] = value;
}

void ElementMapping::addAppObjectMapping(AppObject * key, AppObject * value)
{
	appObjMap_[key] = value;
}
#include "stdafx.h"
#include "EditorScene.h"
#include "Scene.h"
#include "EditorObject.h"
#include "EditorAction.h"
#include "PropertyPage.h"
#include "GLWidget.h"
#include "EditorGlobal.h"
#include "EditorAppObject.h" 
#include "SceneObject.h"
#include "MaskObject.h"

using namespace std;

EditorScene::EditorScene()
{
	scene_ = 0;
}

EditorScene::EditorScene(Scene * scene)
{
	setScene(scene);
}

EditorScene::~EditorScene()
{
}

void EditorScene::init()
{
	editorObject((SceneObject *)scene_->root())->init();
}

void EditorScene::Draw(GfxRenderer * gl) const
{
	//draw scene background
	const Color & bgColor = scene_->bgColor();
	glClearColor(bgColor.r, bgColor.g, bgColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | Global::extraGlClearOptions());
	scene_->handleObjectsListChanged();
	scene_->draw(gl);
}

EditorObjectSPtr EditorScene::editorObject(SceneObject * sceneObj)
{
	map<SceneObject *, EditorObjectSPtr>::const_iterator iter;
	iter = editorObjects_.find(sceneObj);
	if (iter == editorObjects_.end())
	{
		EditorObject * editorObj = 
			EditorGlobal::instance().createEditorObject(this, sceneObj);
		if (!editorObj) return EditorObjectSPtr();
		EditorObjectSPtr sptr(editorObj);
		editorObjects_[sceneObj] = sptr;
		return sptr;
	}
	else
		return (*iter).second;
}

EditorAppObjectSPtr EditorScene::editorAppObject(AppObject * appObj)
{
	map<AppObject *, EditorAppObjectSPtr>::const_iterator iter;
	iter = editorAppObjects_.find(appObj);
	if (iter == editorAppObjects_.end())
	{
		EditorAppObject * editorObj = 
			EditorGlobal::instance().createEditorAppObject(this, appObj);
		if (!editorObj) return EditorAppObjectSPtr();
		EditorAppObjectSPtr sptr(editorObj);
		editorAppObjects_[appObj] = sptr;
		return sptr;
	}
	else
		return (*iter).second;
}

EditorActionSPtr EditorScene::editorAction(Action * action)
{
	map<Action *, EditorActionSPtr>::const_iterator iter;
	iter = editorActions_.find(action);
	if (iter == editorActions_.end())
	{
		EditorAction * editorAction = 
			EditorGlobal::instance().createEditorAction(this, action);
		if (!editorAction) return EditorActionSPtr();
		EditorActionSPtr sptr(editorAction);
		editorActions_[action] = sptr;
		return sptr;
	}
	else
		return (*iter).second;

}

void EditorScene::addEditorObject(const EditorObjectSPtr &edObj)
{
	editorObjects_[edObj->sceneObject()] = edObj;
}

void EditorScene::addEditorAppObject(const EditorAppObjectSPtr &edObj)
{
	editorAppObjects_[edObj->appObject()] = edObj;
}

void EditorScene::deleteEditorObject(EditorObject *edObj)
{
	map<SceneObject *, EditorObjectSPtr>::iterator iter;
	for (iter = editorObjects_.begin(); iter != editorObjects_.end(); ++iter)
	{
		if ((*iter).second.get() == edObj)
		{
			editorObjects_.erase(iter);
			break;
		}
	}
}

void EditorScene::deleteEditorAppObject(EditorAppObject *edObj)
{	
	map<AppObject *, EditorAppObjectSPtr>::iterator iter;
	for (iter = editorAppObjects_.begin(); iter != editorAppObjects_.end(); ++iter)
	{
		if ((*iter).second.get() == edObj)
		{
			editorAppObjects_.erase(iter);
			break;
		}
	}
}


void EditorScene::addEditorAction(const EditorActionSPtr &edAction)
{
	editorActions_[edAction->action()] = edAction;
}

void EditorScene::deleteEditorAction(EditorAction *edAction)
{
	map<Action *, EditorActionSPtr>::iterator iter;
	for (iter = editorActions_.begin(); iter != editorActions_.end(); ++iter)
	{
		if ((*iter).second.get() == edAction)
		{
			editorActions_.erase(iter);
			break;
		}
	}
}

void EditorScene::intersectAll(const EditorDocument * document, 
	const Ray & ray, std::vector<SceneObject *> * objects)
{
	objects->clear();
	intersectAll(document, (SceneObject *)scene_->root(), ray, objects);

}

void EditorScene::intersectAll(const EditorDocument * document,
	const SceneObject * parentObject, const Ray & ray, 
	std::vector<SceneObject *> * objects)
{
	Vector3 intPt;
		
	BOOST_FOREACH(const SceneObjectSPtr & child, parentObject->children())
	{
		EditorObject * edObj = editorObject(child.get()).get();
		if (edObj && edObj->intersect(document, ray, &intPt)) objects->push_back(child.get());
		intersectAll(document, child.get(), ray, objects);
	}


	MaskObject * maskObject = const_cast<SceneObject *>(parentObject)->maskObject();
	if (maskObject)
	{
		SceneObject * child = (SceneObject *)maskObject->maskingObjectGroup();
		EditorObject * edObj = editorObject(child).get();
		if (edObj && edObj->intersect(document, ray, &intPt)) objects->push_back(child);
		intersectAll(document, child, ray, objects);
	}
}
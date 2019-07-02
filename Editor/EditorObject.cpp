#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorScene.h"
#include "EditorObject.h"
#include "SceneObject.h"
#include "GLWidget.h"
#include "Document.h"
#include "Scene.h"
#include "CustomObject.h"


EditorObject::EditorObject()
{
	sceneObject_ = 0;
	
	//uiVisible_ = true;
	uiLocked_ = false;
}

EditorObject::~EditorObject()
{	
}

GfxRenderer * EditorObject::renderer() const
{
	return parentScene_->scene()->parentDocument()->renderer();
}


void EditorObject::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{
	if (/*isThisAndAncestorsVisible() && */sceneObject_->isThisAndAncestorsVisible())
		sceneObject_->drawObject(gl);
}


void EditorObject::draw(GfxRenderer * gl, const EditorDocument * document) const
{
	VisualAttrib * attr = sceneObject_->visualAttrib();
	if (attr)
	{
		gl->pushMatrix();
		gl->multMatrix(attr->transformMatrix());
	}

	drawObject(gl, document);
	
	BOOST_FOREACH(const SceneObjectSPtr & child, sceneObject_->children())
	{
		const_cast<EditorDocument *>(document)->draw(child.get(), gl);
	}

	if (attr) gl->popMatrix();
}
/*
bool EditorObject::isAncestorNotUiVisible() const
{
	SceneObject * obj = sceneObject_->parent();	
	while(obj)
	{		
		if (!parentScene_->editorObject(obj)->isUiVisible()) return true;
		obj = obj->parent();		
	}

	return false;
}
*/
bool EditorObject::isAncestorUiLocked() const
{
	if (!sceneObject_) return false;

	SceneObject * obj = sceneObject_->parent();	
	while(obj)
	{		
		if (parentScene_->editorObject(obj)->isUiLocked()) return true;
		obj = obj->parent();		
	}

	return false;
}
/*
bool EditorObject::isThisAndAncestorsVisible() const
{
	return uiVisible_ && !isAncestorNotUiVisible();
}

*/
bool EditorObject::isThisAndAncestorsUnlocked() const
{
	return !uiLocked_ && !isAncestorUiLocked();
}


void EditorObject::init()
{
	BOOST_FOREACH(const SceneObjectSPtr & child, sceneObject_->children())
	{
		EditorObject * editorChild = parentScene_->editorObject(child.get()).get();
		if (editorChild) editorChild->init();
	}
}

SceneObject * EditorObject::intersect(
	const EditorDocument * document, const Ray & ray, Vector3 * intPt)
{
	return sceneObject_->intersect(intPt, ray);
}

///////////////////////////////////////////////////////////////////////////////

boost::uuids::uuid EditorCustomObject::sceneObjectType() const
{
	return CustomObject().type();
}
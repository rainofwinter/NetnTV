#include "stdafx.h"
#include "EditorMaskObject.h"
#include "MaskObject.h"
#include "GfxRenderer.h"
#include "EditorDocument.h"
#include "Group.h"
#include "Command.h"
#include "Scene.h"
#include "GLWidget.h"
using namespace std;

EditorMaskObject::EditorMaskObject()
{
	expanded_ = true;
	showMasking_ = false;
}

EditorMaskObject::~EditorMaskObject()
{
}

SceneObject * EditorMaskObject::createSceneObject(EditorDocument * document) const
{
	Scene * curScene = document->selectedScene();
	if (curScene->zBuffer())
	{
		QMessageBox::information(0, QObject::tr("Error"), 
			QObject::tr("There can be no MaskObjects in a scene with Z-buffering enabled"));

		return 0;
	}	

	return new MaskObject;
}

boost::uuids::uuid EditorMaskObject::sceneObjectType() const
{
	return MaskObject().type();
}

PropertyPage * EditorMaskObject::propertyPage() const
{
	return new EditorMaskObjectPropertyPage;
}


void EditorMaskObject::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{
	SceneObject * parent = sceneObject_->parent();
	
	bool check = true;
	while(parent)
	{
		check = !(parent->type() == MaskObject().type());

		if(!check) break;
		parent = parent->parent();
	}

	if (/*isThisAndAncestorsVisible() && */sceneObject_->isThisAndAncestorsVisible() && check)
	{
	//if (!sceneObject_->isThisAndAncestorsVisible()) return;
	
		MaskObject * maskObj = sceneObject_->maskObject();
		if (!showMasking_)
		{
			BOOST_FOREACH(const SceneObjectSPtr & child, sceneObject_->children())
				child->draw(gl);	
			maskObj->maskingObjectGroup()->draw(gl);
		}
		else 
		{		
			gl->beginMaskDrawing();		
			maskObj->maskingObjectGroup()->draw(gl);		
			gl->endMaskDrawing();

			gl->beginMaskedDrawing();
			BOOST_FOREACH(const SceneObjectSPtr & child, sceneObject_->children())
				child->draw(gl);			
			gl->endMaskedDrawing();		
		}
	}
}


void EditorMaskObject::draw(GfxRenderer * gl, const EditorDocument * document) const
{
	/*if (!sceneObject_->isThisAndAncestorsVisible()) return;
	VisualAttrib * attr = sceneObject_->visualAttrib();
	
	gl->pushMatrix();
	gl->multMatrix(attr->transformMatrix());

	MaskObject * maskObj = sceneObject_->maskObject();
	if (!showMasking_)
	{
		BOOST_FOREACH(const SceneObjectSPtr & child, sceneObject_->children())
			const_cast<EditorDocument *>(document)->draw(child.get(), gl);

			
		const_cast<EditorDocument *>(document)->draw((SceneObject *)maskObj->maskingObjectGroup(), gl);
	}
	else 
	{		
		gl->beginMaskDrawing();		
		maskObj->maskingObjectGroup()->draw(gl);		
		gl->endMaskDrawing();

		gl->beginMaskedDrawing();
		BOOST_FOREACH(const SceneObjectSPtr & child, sceneObject_->children())
			child->draw(gl);			
		gl->endMaskedDrawing();		
	}
	

	gl->popMatrix();*/
}

SceneObject * EditorMaskObject::intersect(
	const EditorDocument * document, const Ray & ray, Vector3 * intPt)
{
	Vector3 intPt0, intPt1;
	SceneObject * intObj0, * intObj1;
	
	intObj0 = sceneObject_->intersect(&intPt0, ray);
	intObj1 = sceneObject_->maskObject()->maskingObjectGroup()->intersect(&intPt1, ray);

	if (intObj1)
	{
		*intPt = intPt1;
		return intObj1;
	}
	
	*intPt = intPt0;
	return intObj0;
}

///////////////////////////////////////////////////////////////////////////////
EditorMaskObjectPropertyPage::EditorMaskObjectPropertyPage()
{
	setGroupBox(false);

	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;

	setName(tr("MaskObject"));
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);
	startGroup(tr("MaskObject"));
	showMasking_ = new BoolProperty(tr("Show masking in editor"));
	addProperty(showMasking_, PropertyPage::Vertical);
	

}

EditorMaskObjectPropertyPage::~EditorMaskObjectPropertyPage()
{
}

void EditorMaskObjectPropertyPage::update()
{
	core_->update();
	visual_->update();

	EditorMaskObject * edMaskObj = 
		(EditorMaskObject *)document_->editorObject(document_->selectedObject()).get();

	showMasking_->setValue(edMaskObj->showMasking());
}

void EditorMaskObjectPropertyPage::onChanged(Property * property)
{
	EditorMaskObject * edMaskObj = 
		(EditorMaskObject *)document_->editorObject(document_->selectedObject()).get();

	if (property == showMasking_)
	{
		bool val = showMasking_->value();
		document_->doCommand(new ChangeShowMaskingCmd(document_, edMaskObj, val));
	}

	update();
}
#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorScene.h"
#include "EditorBoundingBoxObject.h"
#include "BoundingBoxObject.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "FileUtils.h"
#include "Reader.h"

using namespace std;

EditorBoundingBoxObject::EditorBoundingBoxObject()
{
}

EditorBoundingBoxObject::~EditorBoundingBoxObject()
{
}

SceneObject * EditorBoundingBoxObject::createSceneObject(EditorDocument * document) const
{		
	BoundingBoxObject * boundingBoxObject;
	boundingBoxObject = new BoundingBoxObject;	
	Transform transform = boundingBoxObject->transform();
	transform.setScaling(100.0f, 100.0f, 100.0f);
	boundingBoxObject->setTransform(transform);
	return boundingBoxObject;
}

boost::uuids::uuid EditorBoundingBoxObject::sceneObjectType() const
{
	return BoundingBoxObject().type();
}

PropertyPage * EditorBoundingBoxObject::propertyPage() const
{
	return new EditorBoundingBoxObjectPropertyPage;
}

void EditorBoundingBoxObject::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{
	BoundingBoxObject * obj = (BoundingBoxObject *)sceneObject_;
	BoundingBox bbox = obj->extents();

	GLfloat vertices[] = {
		static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z), 
		static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z), 
		static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z), 
		static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z),

		static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
		static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
		static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
		static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z),

		static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z), 
		static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z), 
		static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
		static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z),

		static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z), 
		static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z), 
		static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
		static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z),

		static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z), 
		static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
		static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
		static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z),

		static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z), 
		static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
		static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
		static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z)
	};

	gl->useColorProgram();
	if (document->isObjectSelected(sceneObject_) || 
		document->isObjectAncestorSelected(sceneObject_))
	{
		gl->setColorProgramColor(0, 1, 0, 1);
	}
	else
	{
		const Color & bgColor = parentScene_->scene()->bgColor();
		Color wireFrameColor(0.5f, 0.5f, 0.5f, 1.0f);

		if (abs(wireFrameColor.r - bgColor.r) < 20 && 
			abs(wireFrameColor.g - bgColor.g) < 20 &&
			abs(wireFrameColor.b - bgColor.b) < 20)
		{
			wireFrameColor.r = 0.75f;
			wireFrameColor.g = 0.75f;
			wireFrameColor.b = 0.75f;
		}
		gl->setColorProgramColor(
			wireFrameColor.r, wireFrameColor.g, wireFrameColor.b, wireFrameColor.a);
		
	}
				
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonOffset(-1, 0);
	
	gl->enableVertexAttribArrayPosition();
	gl->bindArrayBuffer(0);
	gl->vertexAttribPositionPointer(0, (char *)vertices);
	gl->applyCurrentShaderMatrix();
	glDrawArrays(GL_QUADS, 0, 6*4);
	glPolygonOffset(0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
}


///////////////////////////////////////////////////////////////////////////////


EditorBoundingBoxObjectPropertyPage::EditorBoundingBoxObjectPropertyPage()
{
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("BoundingBox"));
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);
	
}

EditorBoundingBoxObjectPropertyPage::~EditorBoundingBoxObjectPropertyPage()
{
}

void EditorBoundingBoxObjectPropertyPage::setDocument(EditorDocument * document)
{
	//this function sets document_ field appropriately
	PropertyPage::setDocument(document);	
}

void EditorBoundingBoxObjectPropertyPage::update()
{
	core_->update();
	visual_->update();	
}

void EditorBoundingBoxObjectPropertyPage::onChanged(Property * property)
{
	update();
}

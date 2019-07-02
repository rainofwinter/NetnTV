#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorLightObject.h"
#include "LightObject.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "Texture.h"
#include "FileUtils.h"
#include "Model.h"

#include "EditorGlobal.h"
#include "EditorScene.h"


using namespace std;
EditorLightObject::EditorLightObject()
{
	float lightDim = 75.0f;
	lightModelMatrix_ = 
		Matrix::Scale(lightDim, lightDim, lightDim) *
			Matrix::Rotate(M_PI, 0.0f, 0.0f, 1.0f);
}

EditorLightObject::~EditorLightObject()
{
}

SceneObject * EditorLightObject::createSceneObject(EditorDocument * document) const
{	
	LightObject * lightObject = new LightObject;	
	return lightObject;
}

boost::uuids::uuid EditorLightObject::sceneObjectType() const
{
	return LightObject().type();
}

PropertyPage * EditorLightObject::propertyPage() const
{
	return new EditorLightObjectPropertyPage;
}

void EditorLightObject::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
	gl->useTextureProgram();
	
	if (/*isThisAndAncestorsVisible() && */sceneObject_->isThisAndAncestorsVisible())
		sceneObject_->drawObject(gl);

	Model * lightModel = document->glWidget()->lightModel();	
	gl->pushMatrix();
	gl->multMatrix(lightModelMatrix_);
	
	gl->useColorProgram();

	if (document->isObjectSelected(sceneObject_) || 
		document->isObjectAncestorSelected(sceneObject_))
	{	
		gl->setColorProgramColor(0.0f, 1.0f, 0.0f, 1.0f);
		lightModel->drawWireframe(gl);
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
		
		lightModel->drawWireframe(gl);
	}
	
	gl->popMatrix();
}

SceneObject * EditorLightObject::intersect(
	const EditorDocument * document, const Ray & ray, Vector3 * intPt)
{
	Matrix invMatrix = 
		sceneObject_->parentTransform() * 
		sceneObject_->visualAttrib()->transformMatrix() * 
		lightModelMatrix_;		
	invMatrix = invMatrix.inverse();
	Ray invRay;
	Vector3 b = invMatrix * (ray.origin + ray.dir);
	invRay.origin = invMatrix * ray.origin;
	invRay.dir = b - invRay.origin;

	Model * lightModel = document->glWidget()->lightModel();

	if (lightModel->intersect(intPt, invRay))
		return sceneObject_;
	else
		return NULL;	
}


///////////////////////////////////////////////////////////////////////////////
EditorLightObjectPropertyPage::EditorLightObjectPropertyPage() : 
intensityValidator_(0.0, 1000.0f, 4, 0)
{
	
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("Light"));

	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);
	
	startGroup(tr("Light"));

	intensity_ = new EditProperty(tr("intensity"), &intensityValidator_);
	addProperty(intensity_);

	addSpacing(4);

	specularIntensity_ = new EditProperty(tr("specular intensity"), &intensityValidator_);
	addProperty(specularIntensity_);

	endGroup();
	
}

EditorLightObjectPropertyPage::~EditorLightObjectPropertyPage()
{
}

void EditorLightObjectPropertyPage::update()
{
	core_->update();
	visual_->update();

	LightObject * light = (LightObject *)document_->selectedObject();

	QString str;
	str.sprintf("%0.3f", light->intensity());
	intensity_->setValue(str);

	str.sprintf("%0.3f", light->specularIntensity());
	specularIntensity_->setValue(str);
}

void EditorLightObjectPropertyPage::onChanged(Property * property)
{
	LightObject * cam = (LightObject *)document_->selectedObject();

	if (property == intensity_)
	{
		float val = intensity_->value().toFloat();
		document_->doCommand(
			makeChangePropertyCmd(ChangeObjectProperty, 
			cam, val, &LightObject::intensity, &LightObject::setIntensity));
	}

	if (property == specularIntensity_)
	{
		float val = specularIntensity_->value().toFloat();
		document_->doCommand(
			makeChangePropertyCmd(ChangeObjectProperty, 
			cam, val, &LightObject::specularIntensity, &LightObject::setSpecularIntensity));
	}

	update();
}

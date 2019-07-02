#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorCameraObject.h"
#include "CameraObject.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "Texture.h"
#include "FileUtils.h"
#include "Model.h"

#include "EditorGlobal.h"
#include "EditorScene.h"

#include "Camera.h"

using namespace std;
EditorCameraObject::EditorCameraObject()
{
	float camDim = 75.0f;
	camModelMatrix_ = 
		Matrix::Scale(camDim, camDim, camDim) *
			Matrix::Rotate(M_PI, 0.0f, 0.0f, 1.0f);
}

EditorCameraObject::~EditorCameraObject()
{
}

SceneObject * EditorCameraObject::createSceneObject(EditorDocument * document) const
{	
	CameraObject * camObject = new CameraObject;

	Scene * scene = document->selectedScene();
	float screenWidth = scene->screenWidth();
	float screenHeight = scene->screenHeight();
	float aspect = screenWidth / screenHeight;

	camObject->camera()->SetTo2DArea(screenWidth/2, screenHeight/2,
		screenWidth, screenHeight, 60, aspect);
	camObject->setTransform(camObject->syncCameraTransform(camObject->transform()));

	return camObject;
}

boost::uuids::uuid EditorCameraObject::sceneObjectType() const
{
	return CameraObject().type();
}

PropertyPage * EditorCameraObject::propertyPage() const
{
	return new EditorCameraObjectPropertyPage;
}

void EditorCameraObject::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{
	//don't draw camera if we are currently looking through it
	if (parentScene_->scene()->userCamera() == sceneObject_) return;

	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
	gl->useTextureProgram();
	
	if (/*isThisAndAncestorsVisible() && */sceneObject_->isThisAndAncestorsVisible())
		sceneObject_->drawObject(gl);

	Model * camModel = document->glWidget()->camModel();	
	gl->pushMatrix();
	gl->multMatrix(camModelMatrix_);
	
	gl->useColorProgram();

	if (document->isObjectSelected(sceneObject_) || 
		document->isObjectAncestorSelected(sceneObject_))
	{	
		gl->setColorProgramColor(0.0f, 1.0f, 0.0f, 1.0f);
		camModel->drawWireframe(gl);
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
		
		camModel->drawWireframe(gl);
	}
	
	
	/*
	BoundingBox bb = camModel->extents();
	
	if (document->isObjectSelected(sceneObject_) || 
		document->isObjectAncestorSelected(sceneObject_))
	{		
		gl->useColorProgram();
		gl->setColorProgramColor(0, 1, 0, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);
		
		int prevDepthFunc;
		glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);
		glDepthFunc(GL_LEQUAL);
		gl->drawBox(bb.minPt, bb.maxPt);
		glDepthFunc(prevDepthFunc);

		glPolygonOffset(0, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}
	*/

	gl->popMatrix();
}

SceneObject * EditorCameraObject::intersect(
	const EditorDocument * document, const Ray & ray, Vector3 * intPt)
{
	Matrix invMatrix = 
		sceneObject_->parentTransform() * 
		sceneObject_->visualAttrib()->transformMatrix() * 
		camModelMatrix_;		
	invMatrix = invMatrix.inverse();
	Ray invRay;
	Vector3 b = invMatrix * (ray.origin + ray.dir);
	invRay.origin = invMatrix * ray.origin;
	invRay.dir = b - invRay.origin;


	Model * camModel = document->glWidget()->camModel();

	//don't select the current camera.
	if (sceneObject_ == document->selectedScene()->userCamera()) return NULL;

	if (camModel->intersect(intPt, invRay))
		return sceneObject_;
	else
		return NULL;	
}


///////////////////////////////////////////////////////////////////////////////
EditorCameraObjectPropertyPage::EditorCameraObjectPropertyPage() : 
fovValidator_(10, 170, 4, 0),
nearPlaneValidator_(0.001, 1000.0f, 4, 0),
farPlaneValidator_(10.0f, 100000000.0f, 4, 0)
{
	
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("Camera"));

	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);
	
	startGroup(tr("Camera"));

	fovY_ = new EditProperty(tr("fov Y"), &fovValidator_);
	addProperty(fovY_);

	nearPlane_ = new EditProperty(tr("near plane"), &nearPlaneValidator_);
	addProperty(nearPlane_);

	farPlane_ = new EditProperty(tr("far plane"), &farPlaneValidator_);
	addProperty(farPlane_);

	/*
	fileName_ = new FileNameProperty(
		tr("filename"), tr("Images (*.png *.jpg *.jpeg)"));
	addProperty(fileName_);

	addSpacing(4);
	width_ = new EditProperty(tr("width"), &dimValidator_);
	addProperty(width_);
	height_ = new EditProperty(tr("height"), &dimValidator_);
	addProperty(height_);
	*/
	endGroup();
	
}

EditorCameraObjectPropertyPage::~EditorCameraObjectPropertyPage()
{
}

void EditorCameraObjectPropertyPage::update()
{
	core_->update();
	visual_->update();

	CameraObject * cam = (CameraObject *)document_->selectedObject();
	QString str;
	str.sprintf("%0.3f", cam->fovY());
	fovY_->setValue(str);

	str.sprintf("%0.3f", cam->nearPlane());
	nearPlane_->setValue(str);

	str.sprintf("%0.3f", cam->farPlane());
	farPlane_->setValue(str);
}

void EditorCameraObjectPropertyPage::onChanged(Property * property)
{
	CameraObject * cam = (CameraObject *)document_->selectedObject();

	if (property == fovY_)
	{
		float fovY = fovY_->value().toFloat();
		document_->doCommand(
			makeChangePropertyCmd(ChangeObjectProperty, 
			cam, fovY, &CameraObject::fovY, &CameraObject::setFovY));
	}

	else if (property == nearPlane_)
	{
		float val = nearPlane_->value().toFloat();
		document_->doCommand(
			makeChangePropertyCmd(ChangeObjectProperty, 
			cam, val, &CameraObject::nearPlane, &CameraObject::setNearPlane));
	}

	else if (property == farPlane_)
	{
		float val = farPlane_->value().toFloat();
		document_->doCommand(
			makeChangePropertyCmd(ChangeObjectProperty, 
			cam, val, &CameraObject::farPlane, &CameraObject::setFarPlane));
	}


	update();
}

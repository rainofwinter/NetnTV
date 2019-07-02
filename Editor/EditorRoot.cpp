#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorRoot.h"
#include "Root.h"
#include "Scene.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "CameraObject.h"

using namespace std;

EditorRoot::EditorRoot()
{
	expanded_ = true;
}

EditorRoot::~EditorRoot()
{
}

SceneObject * EditorRoot::createSceneObject(EditorDocument * document) const
{
	return new Root;
}

boost::uuids::uuid EditorRoot::sceneObjectType() const
{
	return Root().type();
}

PropertyPage * EditorRoot::propertyPage() const
{
	return new EditorRootPropertyPage;
}


///////////////////////////////////////////////////////////////////////////////
EditorRootPropertyPage::EditorRootPropertyPage() 
:
doubleValidator_(0, 99999, 4, 0),
intensityValidator_(0.0, 1000.0f, 4, 0)
{	
	setGroupBox(false);
	
	setName(tr("Root"));

	startGroup(tr("Screen Size"));

	screenWidth_ = new EditProperty(tr("screen width"), &doubleValidator_);
	screenHeight_ = new EditProperty(tr("screen height"), &doubleValidator_);	
	addProperty(screenWidth_);
	addProperty(screenHeight_);	
	addSpacing(4);
	resetCamButton_ = new QPushButton(tr("Reset camera fit to screen"));
	resetCam100Button_ = new QPushButton(tr("Reset camera 1:1"));

	connect(resetCamButton_, SIGNAL(clicked()), this, SLOT(onResetCamera())); 
	connect(resetCam100Button_, SIGNAL(clicked()), this, SLOT(onResetCamera100()));
	addWidget(resetCamButton_);
	addWidget(resetCam100Button_);
	endGroup();
/*
	objectSorting_ = new ComboBoxProperty(tr("object sorting"));
	objectSorting_->addItem(tr("scene order"));
	objectSorting_->addItem(tr("z depth"));
	addSpacing(4);
	addProperty(objectSorting_);*/

	addSpacing(4);
	bgColor_ = new ColorProperty(tr("Background"));
	addSpacing(4);
	addProperty(bgColor_);

	addSpacing(4);
	camera_ = new ComboBoxProperty(tr("Camera"));	
	addProperty(camera_);	
	
	addSpacing(4);
	zBuffer_ = new BoolProperty(tr("Z-Buffer"));
	addProperty(zBuffer_);

	
	addSpacing(8);
	startGroup(tr("Pinch zoom"));
	zoom_ = new BoolProperty(tr("Zoom"));	
	addProperty(zoom_);	

	addSpacing(4);
	zoomMag_ = new ComboBoxProperty(tr("ZoomMag"));	
	zoomMag_->addItem(tr("X2.0"));
	zoomMag_->addItem(tr("X3.0"));
	zoomMag_->addItem(tr("X4.0"));
	addProperty(zoomMag_);	
	endGroup();

	addSpacing(8);
	//trackingUrl_ = new EditProperty(tr("Tracking URL"));
	//addProperty(trackingUrl_);
	//addSpacing(4);
	startGroup(tr("3d Lighting"));
	lightAmbient_ = new EditProperty(tr("ambient lighting"), &intensityValidator_);
	addProperty(lightAmbient_, PropertyPage::Vertical);
	addSpacing(4);
	lightIntensity_ = new EditProperty(tr("default light intensity"), &intensityValidator_);
	addProperty(lightIntensity_, PropertyPage::Vertical);
	lightSpecularIntensity_ = new EditProperty(tr("default specular intensity"), &intensityValidator_);
	addProperty(lightSpecularIntensity_, PropertyPage::Vertical);
	endGroup();
}

EditorRootPropertyPage::~EditorRootPropertyPage()
{
}

void EditorRootPropertyPage::populateCameras()
{
	Scene * scene = document_->selectedScene();
	vector<CameraObject *> cameraObjects;
	scene->cameraObjects(&cameraObjects);

	camera_->clearItems();
	cameras_.clear();
	cameraIndices_.clear();

	camera_->addItem(tr("<default>"));
	cameras_.push_back(NULL);
	cameraIndices_[NULL] = 0;

	BOOST_FOREACH(CameraObject * cameraObject, cameraObjects)
	{
		camera_->addItem(stdStringToQString(cameraObject->id()));
		cameras_.push_back(cameraObject);
		cameraIndices_[cameraObject] = (int)(cameras_.size() - 1);
	}
}

void EditorRootPropertyPage::onResetCamera()
{
	view_->resetCamera();
	view_->update();
}

void EditorRootPropertyPage::onResetCamera100()
{
	view_->resetCameraOneToOne();
	view_->update();
}

void EditorRootPropertyPage::setDocument(EditorDocument * document)
{
	if (document_)
	{
		disconnect(document_, 0, this, 0);
	}

	//this function sets document_ field appropriately
	PropertyPage::setDocument(document);

	if (document)
	{
		connect(document, SIGNAL(sceneChanged()), this, SLOT(onResetEnableChange()));
		connect(document, SIGNAL(sceneSelectionChanged()), this, SLOT(onResetEnableChange()));

		onResetEnableChange();
	}
}

void EditorRootPropertyPage::onResetEnableChange()
{
	Scene * scene = document_->selectedScene();
	CameraObject * userCam = scene->userCamera();

	bool isEnabled = userCam == NULL;
	
	resetCamButton_->setEnabled(isEnabled);
	resetCam100Button_->setEnabled(isEnabled);	
}

void EditorRootPropertyPage::update()
{
	Scene * scene = document_->selectedScene();
	QString str;
	str.sprintf("%.4f", scene->screenWidth());
	screenWidth_->setValue(str);
	str.sprintf("%.4f", scene->screenHeight());
	screenHeight_->setValue(str);
/*
	if (scene->zBuffer())
		objectSorting_->setValue(1);
	else
		objectSorting_->setValue(0);*/

	bgColor_->setValue(&scene->bgColor());

	populateCameras();

	CameraObject * camObject = scene->userCamera();
	camera_->setValue(cameraIndices_[camObject]);	

	bool zBuffer = scene->zBuffer();
	zBuffer_->setValue(zBuffer);
	
	bool zoom = scene->zoom();
	zoom_->setValue(zoom);

	int zoomMagState = scene->zoomMag();
	zoomMag_->setValue(zoomMagState);

//	trackingUrl_->setValue(stdStringToQString(scene->trackingUrl()));

	lightIntensity_->setValue(QString::number(scene->lightIntensity(), 'g', 4));
	lightSpecularIntensity_->setValue(QString::number(scene->lightSpecularIntensity(), 'g', 4));
	lightAmbient_->setValue(QString::number(scene->lightAmbient(), 'g', 4));
}

void EditorRootPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);
	Scene * scene = document_->selectedScene();

	if (property == screenWidth_)
	{
		float screenWidth = screenWidth_->value().toFloat();
		document_->doCommand(makeChangePropertyCmd(
			ChangeSceneProperty, scene, screenWidth, 
			&Scene::screenWidth, &Scene::setScreenWidth));
	}

	if (property == screenHeight_)
	{
		float screenHeight = screenHeight_->value().toFloat();		
		document_->doCommand(makeChangePropertyCmd(
			ChangeSceneProperty, scene, screenHeight, 
			&Scene::screenHeight, &Scene::setScreenHeight));
	}
/*
	if (property == objectSorting_)
	{
		bool zBuffer = false;
		if (objectSorting_->value()) zBuffer = true;

		document_->doCommand(makeChangePropertyCmd(
			ChangeSceneProperty, scene, zBuffer, 
			&Scene::zBuffer, &Scene::setZBuffer));
	}*/

	if (property == bgColor_)
	{
		Color color = bgColor_->value();
		document_->doCommand(makeChangePropertyCmd(
			ChangeSceneProperty, scene, color, 
			&Scene::bgColor, &Scene::setBgColor));
	}

	if (property == camera_)
	{
		int index = camera_->value();
		document_->doCommand(new ChangeSceneCameraCmd(
			document_->glWidget(), scene, cameras_[index]));		
	}

	if (property == zBuffer_)
	{
		bool zBuffer = zBuffer_->value();
		document_->doCommand(makeChangePropertyCmd(
			ChangeSceneProperty, scene, zBuffer, 
			&Scene::zBuffer, &Scene::setZBuffer));
	}

	if (property == zoom_)
	{
		bool zoom = zoom_->value();
		document_->doCommand(makeChangePropertyCmd(
			ChangeSceneProperty, scene, zoom, 
			&Scene::zoom, &Scene::setZoom));
	}

	if (property == zoomMag_)
	{
		int zoomMagState = zoomMag_->value();
		document_->doCommand(makeChangePropertyCmd(
			ChangeSceneProperty, scene, zoomMagState, 
			&Scene::zoomMag, &Scene::setZoomMag));
	}

	/*if (property == trackingUrl_)
	{
		string value = qStringToStdString(trackingUrl_->value());
		boost::trim(value);
		regexReplace(&value, "^(.*://|)(.*)$", "http://$2");
	
		document_->doCommand(makeChangePropertyCmd(
			ChangeSceneProperty, scene, value,
			&Scene::trackingUrl, &Scene::setTrackingUrl));
	}*/

	if (property == lightIntensity_)
	{
		float val = lightIntensity_->value().toFloat();
		document_->doCommand(makeChangePropertyCmd(
			ChangeSceneProperty, scene, val, 
			&Scene::lightIntensity, &Scene::setLightIntensity));
	}

	if (property == lightSpecularIntensity_)
	{
		float val = lightSpecularIntensity_->value().toFloat();
		document_->doCommand(makeChangePropertyCmd(
			ChangeSceneProperty, scene, val, 
			&Scene::lightSpecularIntensity, &Scene::setLightSpecularIntensity));
	}

	if (property == lightAmbient_)
	{
		float val = lightAmbient_->value().toFloat();
		document_->doCommand(makeChangePropertyCmd(
			ChangeSceneProperty, scene, val, 
			&Scene::lightAmbient, &Scene::setLightAmbient));
	}

	//just in case one of the commands above failed
	update();

}


void EditorRootPropertyPage::setCameraIndex(int index, EditorDocument * document)
{
	if (index < cameras_.size())
	{
		camera_->setValue(index);
		document->doCommand(new ChangeSceneCameraCmd(
			document->glWidget(),  document->selectedScene(), cameras_[index]));
	}
}
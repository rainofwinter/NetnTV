#include "stdafx.h"
#include "EditorSetCameraAction.h"
#include "SetCameraAction.h"
#include "CameraObject.h"
#include "EditorDocument.h"
#include "Utils.h"

ActionPropertyPage * EditorSetCameraAction::propertyPage() const
{
	return new EditorSetCameraPropertyPage();
}

boost::uuids::uuid EditorSetCameraAction::actionType() const 
{
	return SetCameraAction().type();
}

QString EditorSetCameraAction::toString(Action * action) const
{
	SetCameraAction * camAction = (SetCameraAction *)action;
	QString typeStr = QString::fromLocal8Bit(action->typeStr());

	QString objName = "<default>";
	
	if (camAction->cameraObject())
		objName = QString::fromLocal8Bit(camAction->cameraObject()->id().c_str());

	return typeStr + " - " + objName;
	
	
}

///////////////////////////////////////////////////////////////////////////////
EditorSetCameraPropertyPage::EditorSetCameraPropertyPage()
{	
	setGroupBox(false);
	
	cameraObj_ = new ComboBoxProperty(tr("camera"));
	addProperty(cameraObj_);	
}

EditorSetCameraPropertyPage::~EditorSetCameraPropertyPage()
{
}

void EditorSetCameraPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);
	populateCameras();
}

void EditorSetCameraPropertyPage::populateCameras()
{
	Scene * scene = document_->selectedScene();
	cameraObjs_.clear();
	cameraObjs_.push_back(NULL);
	scene->cameraObjects(&cameraObjs_);

	cameraObj_->clearItems();
	BOOST_FOREACH(CameraObject * cam, cameraObjs_)
	{
		if (cam)
			cameraObj_->addItem(stdStringToQString(cam->id()));
		else
			cameraObj_->addItem(tr("<default>"));
	}
}


void EditorSetCameraPropertyPage::update()
{
	SetCameraAction * action = (SetCameraAction *)action_;

	CameraObject * camObj = action->cameraObject();

	int camIndex = -1;

	for (int i = 0; i < (int)cameraObjs_.size(); ++i)
	{
		if (cameraObjs_[i] == camObj) camIndex = i;
	}

	cameraObj_->setValue(camIndex);

}

void EditorSetCameraPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	SetCameraAction * action = (SetCameraAction *)action_;

	action->setCameraObject(cameraObjs_[cameraObj_->value()]);
	
	update();
}

bool EditorSetCameraPropertyPage::isValid() const
{
	return (cameraObj_->value() >= 0);
}
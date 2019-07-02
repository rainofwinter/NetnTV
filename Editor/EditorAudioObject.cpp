#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorAudioObject.h"
#include "AudioObject.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "FileUtils.h"

#include "EditorGlobal.h"
#include "EditorScene.h"


using namespace std;
EditorAudioObject::EditorAudioObject()
{
}

EditorAudioObject::~EditorAudioObject()
{
}

SceneObject * EditorAudioObject::createSceneObject(EditorDocument * document) const
{
	AudioObject * audioObject = new AudioObject;
	return audioObject;
}

boost::uuids::uuid EditorAudioObject::sceneObjectType() const
{
	return AudioObject().type();
}

PropertyPage * EditorAudioObject::propertyPage() const
{
	return new EditorAudioObjectPropertyPage;
}

void EditorAudioObject::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{
	return;
}

SceneObject * EditorAudioObject::intersect(
	const EditorDocument * document, const Ray & ray, Vector3 * intPt)
{
	return NULL;	
}


///////////////////////////////////////////////////////////////////////////////
EditorAudioObjectPropertyPage::EditorAudioObjectPropertyPage()
{	
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	
	setName(tr("Audio"));

	addPropertyPage(core_);
	addSpacing(4);
	
	startGroup(tr("Audio"));

	sourceType_ = new ComboBoxProperty(tr("Source type"));
	sourceType_->addItem(tr("File name"));
	sourceType_->addItem(tr("Recorded"));
	addProperty(sourceType_);
	addSpacing(4);

	fileName_ = new FileNameProperty(
		tr("filename"), tr("Audio (*.mp3)"));
	addProperty(fileName_);

	addSpacing(4);
	repeat_ = new BoolProperty(tr("Repeat"));
	addProperty(repeat_);
	endGroup();	
}

EditorAudioObjectPropertyPage::~EditorAudioObjectPropertyPage()
{
}

void EditorAudioObjectPropertyPage::update()
{
	core_->update();
	AudioObject * obj = (AudioObject *)document_->selectedObject();


	sourceType_->setValue((int)obj->sourceType());
	if (obj->sourceType() == AudioObject::File)
	{
		fileName_->setEnabled(true);
	}
	else
	{
		fileName_->setEnabled(false);
	}


	fileName_->setValue(stdStringToQString(obj->fileName()));
	repeat_->setValue(obj->repeat());
}


void EditorAudioObjectPropertyPage::onChanged(Property * property)
{
	AudioObject * obj = (AudioObject *)document_->selectedObject();

	if (property == fileName_)
	{
		std::string fileName = qStringToStdString(fileName_->value());
		fileName = convertToRelativePath(Global::instance().readDirectory(), fileName);
		document_->doCommand(
			makeChangePropertyCmd(ChangeObjectProperty, 
			obj, fileName, &AudioObject::fileName, &AudioObject::setFileName));
	}

	if (property == repeat_)
	{
		bool val = repeat_->value();
		document_->doCommand(
			makeChangePropertyCmd(ChangeObjectProperty, 
			obj, val, &AudioObject::repeat, &AudioObject::setRepeat));
	}

	if (property == sourceType_)
	{
		AudioObject::SourceType sourceType = (AudioObject::SourceType)sourceType_->value();
		document_->doCommand(
			makeChangePropertyCmd(ChangeObjectProperty, 
			obj, sourceType, &AudioObject::sourceType, &AudioObject::setSourceType));
	}


	update();
}

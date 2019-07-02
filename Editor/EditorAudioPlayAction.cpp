#include "stdafx.h"
#include "SceneObject.h"
#include "EditorAudioPlayAction.h"
#include "AudioPlayAudioAction.h"
#include "Utils.h"

using namespace std;

ActionPropertyPage * EditorAudioPlayAction::propertyPage() const
{
	return new EditorAudioPlayPropertyPage();
}

boost::uuids::uuid EditorAudioPlayAction::actionType() const 
{
	return AudioPlayAudioAction().type();
}

QString EditorAudioPlayAction::toString(Action * pAction) const
{
	AudioPlayAudioAction * action = (AudioPlayAudioAction *)pAction;
	
	QString startTimeStr;
	startTimeStr.sprintf("%.3fs", action->startTime());
	QString typeStr =  
		QString::fromLocal8Bit(action->typeStr()) + 
		" - " + 
		stdStringToQString(action->targetObject()->id()) +
		" : " + 
		startTimeStr;
	return typeStr;
}

///////////////////////////////////////////////////////////////////////////////
EditorAudioPlayPropertyPage::EditorAudioPlayPropertyPage()
	: startTimeValidator_(0.0, 1000000.0, 3, 0)
{	
	setGroupBox(false);

	startTime_ = new EditProperty(tr("start time (seconds)"), &startTimeValidator_);
	addProperty(startTime_, PropertyPage::Vertical);

}

EditorAudioPlayPropertyPage::~EditorAudioPlayPropertyPage()
{
}

void EditorAudioPlayPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);	

}

void EditorAudioPlayPropertyPage::update()
{
	AudioPlayAudioAction * action = (AudioPlayAudioAction *)action_;
	startTime_->setValue(QString::number(action->startTime()));

}

void EditorAudioPlayPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);
	AudioPlayAudioAction * action = (AudioPlayAudioAction *)action_;

	if (property == startTime_)
	{
		action->setStartTime(boost::lexical_cast<float>(qStringToStdString(startTime_->value())));
	}

	update();
}

bool EditorAudioPlayPropertyPage::isValid() const
{
	return true;
}
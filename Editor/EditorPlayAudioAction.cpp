#include "stdafx.h"
#include "EditorPlayAudioAction.h"
#include "PlayAudioAction.h"
#include "Animation.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Utils.h"

ActionPropertyPage * EditorPlayAudioAction::propertyPage() const
{
	return new EditorPlayAudioActionPropertyPage();
}

boost::uuids::uuid EditorPlayAudioAction::actionType() const 
{
	return PlayAudioAction().type();
}

QString EditorPlayAudioAction::toString(Action * paction) const
{
	PlayAudioAction * action = (PlayAudioAction *)paction;
	QString typeStr = QString::fromLocal8Bit(action->typeStr());
	QString startTimeStr;
	startTimeStr.sprintf("%.3fs", action->startTime());
	return typeStr + " - " + stdStringToQString(action->audio()) + " : " + startTimeStr;
	
}

///////////////////////////////////////////////////////////////////////////////

EditorPlayAudioActionPropertyPage::EditorPlayAudioActionPropertyPage() :
validator_(0.0, 1000000.0, 3, 0)
{	
	setGroupBox(false);	


	videoFileName_ = new FileNameProperty(
		tr("filename"), 
		tr("Audios (*.mp3 *.aac)")
		);
	addProperty(videoFileName_, PropertyPage::Vertical);
	
	addSpacing(4);
	
	startTime_ = new EditProperty(tr("start time (seconds)"), &validator_);
	addProperty(startTime_, PropertyPage::Vertical);
}

EditorPlayAudioActionPropertyPage::~EditorPlayAudioActionPropertyPage()
{
}

void EditorPlayAudioActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);		
}

void EditorPlayAudioActionPropertyPage::update()
{
	PlayAudioAction * action = (PlayAudioAction *)action_;	
	videoFileName_->setValue(stdStringToQString(action->audio()));
	startTime_->setValue(QString::number(action->startTime()));

}

void EditorPlayAudioActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	PlayAudioAction * action = (PlayAudioAction *)action_;

	if (property == videoFileName_)
	{
		action->setAudio(qStringToStdString(videoFileName_->value()));
	}

	if (property == startTime_)
	{
		action->setStartTime(boost::lexical_cast<float>(qStringToStdString(startTime_->value())));
	}

	update();
}

bool EditorPlayAudioActionPropertyPage::isValid() const
{

	bool empty = true;
	empty = videoFileName_->value().isEmpty();
	return !empty;
}
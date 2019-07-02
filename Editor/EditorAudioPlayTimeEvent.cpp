#include "stdafx.h"
#include "EditorAudioPlayTimeEvent.h"
#include "AudioPlayTimeEvent.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Utils.h"
#include <boost/lexical_cast.hpp>

EventPropertyPage * EditorAudioPlayTimeEvent::propertyPage() const
{
	return new EditorAudioPlayTimeEventPropertyPage();
}

boost::uuids::uuid EditorAudioPlayTimeEvent::eventType() const 
{
	return AudioPlayTimeEvent().type();
}

QString EditorAudioPlayTimeEvent::toString(Event * event) const
{
	AudioPlayTimeEvent * dEvent = (AudioPlayTimeEvent *)event;
	QString typeStr = QString::fromLocal8Bit(dEvent->typeStr());
	return typeStr + " - " + QString::number(dEvent->time());
	
}

///////////////////////////////////////////////////////////////////////////////
EditorAudioPlayTimeEventPropertyPage::EditorAudioPlayTimeEventPropertyPage() :
validator_(0.0, 1000000.0, 3, 0)
{	
	setGroupBox(false);
	
	time_ = new EditProperty(tr("time (seconds)"), &validator_);
	addProperty(time_, PropertyPage::Vertical);
	
}

EditorAudioPlayTimeEventPropertyPage::~EditorAudioPlayTimeEventPropertyPage()
{
}

void EditorAudioPlayTimeEventPropertyPage::setDocument(EditorDocument * document)
{
	EventPropertyPage::setDocument(document);		
}

void EditorAudioPlayTimeEventPropertyPage::update()
{
	AudioPlayTimeEvent * event = (AudioPlayTimeEvent *)event_;
	time_->setValue(QString::number(event->time()));	
}

void EditorAudioPlayTimeEventPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	AudioPlayTimeEvent * event = (AudioPlayTimeEvent *)event_;
	event->setTime(boost::lexical_cast<float>(qStringToStdString(time_->value())));
	update();
}

bool EditorAudioPlayTimeEventPropertyPage::isValid() const
{
	return !time_->value().isEmpty();
}
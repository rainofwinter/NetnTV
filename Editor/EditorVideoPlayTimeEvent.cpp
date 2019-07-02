#include "stdafx.h"
#include "EditorVideoPlayTimeEvent.h"
#include "VideoPlayTimeEvent.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Utils.h"
#include <boost/lexical_cast.hpp>

EventPropertyPage * EditorVideoPlayTimeEvent::propertyPage() const
{
	return new EditorVideoPlayTimeEventPropertyPage();
}

boost::uuids::uuid EditorVideoPlayTimeEvent::eventType() const 
{
	return VideoPlayTimeEvent().type();
}

QString EditorVideoPlayTimeEvent::toString(Event * event) const
{
	VideoPlayTimeEvent * dEvent = (VideoPlayTimeEvent *)event;
	QString typeStr = QString::fromLocal8Bit(dEvent->typeStr());
	return typeStr + " - " + QString::number(dEvent->time());
	
}

///////////////////////////////////////////////////////////////////////////////
EditorVideoPlayTimeEventPropertyPage::EditorVideoPlayTimeEventPropertyPage() :
validator_(0.0, 1000000.0, 3, 0)
{	
	setGroupBox(false);
	
	time_ = new EditProperty(tr("time (seconds)"), &validator_);
	addProperty(time_, PropertyPage::Vertical);
	
}

EditorVideoPlayTimeEventPropertyPage::~EditorVideoPlayTimeEventPropertyPage()
{
}

void EditorVideoPlayTimeEventPropertyPage::setDocument(EditorDocument * document)
{
	EventPropertyPage::setDocument(document);		
}

void EditorVideoPlayTimeEventPropertyPage::update()
{
	VideoPlayTimeEvent * event = (VideoPlayTimeEvent *)event_;
	time_->setValue(QString::number(event->time()));	
}

void EditorVideoPlayTimeEventPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	VideoPlayTimeEvent * event = (VideoPlayTimeEvent *)event_;
	event->setTime(boost::lexical_cast<float>(qStringToStdString(time_->value())));
	update();
}

bool EditorVideoPlayTimeEventPropertyPage::isValid() const
{
	return !time_->value().isEmpty();
}
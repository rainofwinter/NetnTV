#include "stdafx.h"
#include "EditorReceiveMessageEvent.h"
#include "ReceiveMessageEvent.h"
#include "Animation.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Utils.h"

EventPropertyPage * EditorReceiveMessageEvent::propertyPage() const
{
	return new EditorReceiveMessageEventPropertyPage();
}

boost::uuids::uuid EditorReceiveMessageEvent::eventType() const 
{
	return ReceiveMessageEvent().type();
}

QString EditorReceiveMessageEvent::toString(Event * event) const
{
	ReceiveMessageEvent * dEvent = (ReceiveMessageEvent *)event;
	QString typeStr = QString::fromLocal8Bit(dEvent->typeStr());
	return typeStr + " - " + QString::fromLocal8Bit(dEvent->message().c_str());
	
}

///////////////////////////////////////////////////////////////////////////////
EditorReceiveMessageEventPropertyPage::EditorReceiveMessageEventPropertyPage()
{	
	setGroupBox(false);
	
	message_ = new EditProperty(tr("message"));
	addProperty(message_, PropertyPage::Vertical);
	
}

EditorReceiveMessageEventPropertyPage::~EditorReceiveMessageEventPropertyPage()
{
}

void EditorReceiveMessageEventPropertyPage::setDocument(EditorDocument * document)
{
	EventPropertyPage::setDocument(document);		
}

void EditorReceiveMessageEventPropertyPage::update()
{
	ReceiveMessageEvent * event = (ReceiveMessageEvent *)event_;
	message_->setValue(QString::fromLocal8Bit(event->message().c_str()));	
}

void EditorReceiveMessageEventPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	ReceiveMessageEvent * event = (ReceiveMessageEvent *)event_;
	event->setMessage(qStringToStdString(message_->value()));
	update();
}

bool EditorReceiveMessageEventPropertyPage::isValid() const
{
	return !message_->value().isEmpty();
}
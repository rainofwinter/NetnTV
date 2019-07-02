#include "stdafx.h"
#include "EditorImageGridImageClickedEvent.h"
#include "ImageGridImageClickedEvent.h"
#include "Animation.h"
#include "Scene.h"
#include "EditorDocument.h"

EventPropertyPage * EditorImageGridImageClickedEvent::propertyPage() const
{
	return new EditorImageGridImageClickedEventPropertyPage();
}

boost::uuids::uuid EditorImageGridImageClickedEvent::eventType() const 
{
	return ImageGridImageClickedEvent().type();
}

QString EditorImageGridImageClickedEvent::toString(Event * event) const
{
	ImageGridImageClickedEvent * dEvent = (ImageGridImageClickedEvent *)event;
	QString typeStr = QString::fromLocal8Bit(dEvent->typeStr());
	QString indexStr;
	indexStr.sprintf("%d", dEvent->imageIndex());
	return typeStr + " - " + indexStr;
	
}

///////////////////////////////////////////////////////////////////////////////
EditorImageGridImageClickedEventPropertyPage::EditorImageGridImageClickedEventPropertyPage()
: validator_(0, 9999, 0)
{	
	setGroupBox(false);
	
	index_ = new EditProperty(tr("image index"), &validator_);
	addProperty(index_, PropertyPage::Vertical);
	
}

EditorImageGridImageClickedEventPropertyPage::~EditorImageGridImageClickedEventPropertyPage()
{
}

void EditorImageGridImageClickedEventPropertyPage::setDocument(EditorDocument * document)
{
	EventPropertyPage::setDocument(document);		
}

void EditorImageGridImageClickedEventPropertyPage::update()
{
	ImageGridImageClickedEvent * event = (ImageGridImageClickedEvent *)event_;
	QString str;
	str.sprintf("%d", event->imageIndex());
	index_->setValue(str);	
}

void EditorImageGridImageClickedEventPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	ImageGridImageClickedEvent * event = (ImageGridImageClickedEvent *)event_;
	event->setImageIndex(index_->value().toInt());
	update();
}

bool EditorImageGridImageClickedEventPropertyPage::isValid() const
{
	return !index_->value().isEmpty();
}
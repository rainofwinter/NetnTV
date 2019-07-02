#include "stdafx.h"
#include "EditorSceneChangerPageChangedEvent.h"
#include "PageChangedEvent.h"
#include "Animation.h"
#include "Scene.h"
#include "EditorDocument.h"

EventPropertyPage * EditorSceneChangerPageChangedEvent::propertyPage() const
{
	return new EditorSceneChangerPageChangedEventPropertyPage();
}

boost::uuids::uuid EditorSceneChangerPageChangedEvent::eventType() const 
{
	return SceneChangerPageChangedEvent().type();
}

QString EditorSceneChangerPageChangedEvent::toString(Event * event) const
{
	SceneChangerPageChangedEvent * dEvent = (SceneChangerPageChangedEvent *)event;
	QString typeStr = QString::fromLocal8Bit(dEvent->typeStr());
	QString indexStr;
	indexStr.sprintf("%d", dEvent->index());
	return typeStr + " - " + indexStr;
	
}

///////////////////////////////////////////////////////////////////////////////
EditorSceneChangerPageChangedEventPropertyPage::EditorSceneChangerPageChangedEventPropertyPage()
: validator_(0, 9999, 0)
{	
	setGroupBox(false);
	
	index_ = new EditProperty(tr("page index"), &validator_);
	addProperty(index_, PropertyPage::Vertical);
	
}

EditorSceneChangerPageChangedEventPropertyPage::~EditorSceneChangerPageChangedEventPropertyPage()
{
}

void EditorSceneChangerPageChangedEventPropertyPage::setDocument(EditorDocument * document)
{
	EventPropertyPage::setDocument(document);		
}

void EditorSceneChangerPageChangedEventPropertyPage::update()
{
	SceneChangerPageChangedEvent * event = (SceneChangerPageChangedEvent *)event_;
	QString str;
	str.sprintf("%d", event->index());
	index_->setValue(str);	
}

void EditorSceneChangerPageChangedEventPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	SceneChangerPageChangedEvent * event = (SceneChangerPageChangedEvent *)event_;
	event->setIndex(index_->value().toInt());
	update();
}

bool EditorSceneChangerPageChangedEventPropertyPage::isValid() const
{
	return !index_->value().isEmpty();
}
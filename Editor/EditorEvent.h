#pragma once
#include "Event.h"

class EditorScene;
class EventPropertyPage;

class EditorEvent
{
public:	
	EditorEvent() {event_ = 0;}
	virtual ~EditorEvent() {}
	virtual EditorEvent * clone() const = 0;
	virtual EventPropertyPage * propertyPage() const {return 0;}
	virtual boost::uuids::uuid eventType() const = 0;
	virtual QString toString(Event * event) const
	{
		return QString::fromLocal8Bit(event->typeStr());
	}

	void setEvent(Event * event) {event_ = event;}
	Event * event() const {return event_;}

	void setScene(EditorScene * scene) {parentScene_ = scene;}
	EditorScene * parentScene() const {return parentScene_;}
protected:
	Event * event_;
	EditorScene * parentScene_;
};
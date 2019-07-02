#pragma once

#include "Event.h"
#include "Root.h"
#include "PhotoObject.h"

class PhotoSaveEvent : public Event
{
public:
	PhotoSaveEvent(){}
	virtual const char * typeStr() const
	{
		return "Photo Save";
	}
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()	
		("3A9265C3-A0B8-4611-9D4E-FBC28DD71445");

		return uuid;
	}

	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new PhotoSaveEvent(*this);
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return obj->type() == PhotoObject().type();
	}

	unsigned char version() const {return 0;}
private:
};
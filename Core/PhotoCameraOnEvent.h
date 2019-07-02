#pragma once

#include "Event.h"
#include "Root.h"
#include "PhotoObject.h"

class PhotoCameraOnEvent : public Event
{
public:
	PhotoCameraOnEvent(){}
	virtual const char * typeStr() const
	{
		return "Photo Camera On";
	}
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()	
		("ACF4E40C-F8E6-4F4D-A991-B9758F003F41");

		return uuid;
	}

	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new PhotoCameraOnEvent(*this);
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return obj->type() == PhotoObject().type();
	}

	unsigned char version() const {return 0;}
private:
};
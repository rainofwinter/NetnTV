#pragma once

#include "Event.h"
#include "Root.h"
#include "PhotoObject.h"

class PhotoCameraOffEvent : public Event
{
public:
	PhotoCameraOffEvent(){}
	virtual const char * typeStr() const
	{
		return "Photo Camera Off";
	}
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()	
		("003EE5EE-8FDC-4394-A9BE-18373D50A2B6");

		return uuid;
	}

	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new PhotoCameraOffEvent(*this);
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return obj->type() == PhotoObject().type();
	}

	unsigned char version() const {return 0;}
private:
};

#pragma once

#include "Event.h"
#include "Root.h"
#include "PhotoObject.h"

class PhotoTakePictureEvent : public Event
{
public:
	PhotoTakePictureEvent(){}
	virtual const char * typeStr() const
	{
		return "Photo Take a Picture";
	}
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()	
		("1C60DEEE-5071-4130-ae03-D8D3F462AAAB");

		return uuid;
	}

	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new PhotoTakePictureEvent(*this);
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return obj->type() == PhotoObject().type();
	}

	unsigned char version() const {return 0;}
private:
};
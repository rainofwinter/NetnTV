#pragma once
#include "Event.h"

class PressEvent : public CursorEvent
{
public:
	virtual const char * typeStr() const
	{
		return "Press";
	}
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("8368BBB3-AB19-A7FE-BB6A-B65CF6A31216");
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return obj->visualAttrib() != NULL;
	}

	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new PressEvent(*this);
	}

	unsigned char version() const {return 0;}
	virtual const char * xmlTag() const {return "Press";}
private:
};

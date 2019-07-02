#pragma once
#include "Event.h"

class MoveEvent : public CursorEvent
{
public:
	virtual const char * typeStr() const
	{
		return "Move";
	}

	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("23627783-CCC9-AA1E-BB6A-B65CE5A31212");
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
		return new MoveEvent(*this);
	}

	unsigned char version() const {return 0;}

private:
};

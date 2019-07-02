#pragma once

#include "Event.h"
#include "Root.h"

class ScenePreStartEvent : public Event
{
public:
	virtual const char * typeStr() const
	{
		return "Scene PreStart";
	}

	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("823BE726-924B-58E2-8688-4973D2C88598");
	}

	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new ScenePreStartEvent(*this);
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return obj->type() == Root().type();
	}

	unsigned char version() const {return 0;}
private:
};


#pragma once

#include "Event.h"
#include "Root.h"

class SceneStartEvent : public Event
{
public:
	virtual const char * typeStr() const
	{
		return "Scene Start";
	}
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("933BE726-094B-58E2-E802-4973D2C8E298");
	}

	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new SceneStartEvent(*this);
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return obj->type() == Root().type();
	}

	unsigned char version() const {return 0;}

	virtual const char * xmlTag() const {return "SceneStart";}
private:
};


#pragma once

#include "Event.h"
#include "Root.h"

class SceneStopEvent : public Event
{
public:
	virtual const char * typeStr() const
	{
		return "Scene Stop";
	}
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("38382726-094B-583D-7602-497122C8E548");
	}

	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new SceneStopEvent(*this);
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return obj->type() == Root().type();
	}

	unsigned char version() const {return 0;}
private:
};
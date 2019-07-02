#pragma once

#include "Event.h"

class VideoObjectStoppedEvent : public Event
{
public:
	VideoObjectStoppedEvent() {}

	virtual const char * typeStr() const
	{
		return "Stopped";
	}
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()		
		("9f4e1616-b81e-43e7-b9ff-0874cce903dc");

		return uuid;
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return 
			obj->type() == VideoObject().type();
	}


	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new VideoObjectStoppedEvent(*this);
	}

	unsigned char version() const {return 0;}

	void read(Reader & reader, unsigned char)
	{
		reader.readParent<Event>(this);
	}

	void write(Writer & writer) const
	{
		writer.writeParent<Event>(this);
	}

private:	
};


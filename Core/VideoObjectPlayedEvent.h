#pragma once

#include "Event.h"

class VideoObjectPlayedEvent : public Event
{
public:
	VideoObjectPlayedEvent() {}

	virtual const char * typeStr() const
	{
		return "Played";
	}
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()		
		("b42171fc-77cf-4f56-87f5-3f6eae814bba");

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
		return new VideoObjectPlayedEvent(*this);
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


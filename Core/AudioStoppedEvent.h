#pragma once

#include "Event.h"

class AudioStoppedEvent : public Event
{
public:
	AudioStoppedEvent() {}

	virtual const char * typeStr() const
	{
		return "Stopped";
	}
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()		
		("054459da-31f2-479c-93f3-791ce9430b41");

		return uuid;
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return 
			obj->type() == AudioObject().type();
	}


	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new AudioStoppedEvent(*this);
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


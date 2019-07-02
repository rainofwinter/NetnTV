#pragma once

#include "Event.h"

class AudioPlayedEvent : public Event
{
public:
	AudioPlayedEvent() {}

	virtual const char * typeStr() const
	{
		return "Played";
	}
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()		
		("763b9e0a-0829-49b7-b55e-33014166a929");

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
		return new AudioPlayedEvent(*this);
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


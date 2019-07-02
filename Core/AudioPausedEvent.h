#pragma once

#include "Event.h"

class AudioPausedEvent : public Event
{
public:
	AudioPausedEvent() {}

	virtual const char * typeStr() const
	{
		return "Paused";
	}
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()		
		("e98c1810-1d6d-46db-be83-7debeafdce1d");

		return uuid;
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return obj->type() == AudioObject().type();
	}


	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new AudioPausedEvent(*this);
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


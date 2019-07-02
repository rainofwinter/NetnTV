#pragma once

#include "Event.h"

class AudioResumedEvent : public Event
{
public:
	AudioResumedEvent() {}

	virtual const char * typeStr() const
	{
		return "Resumed";
	}
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()		
		("df8e71d6-d4f1-47a7-8de4-ba61f221c8c3");

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
		return new AudioResumedEvent(*this);
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


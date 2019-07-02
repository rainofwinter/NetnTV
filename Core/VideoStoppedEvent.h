#pragma once

#include "Event.h"
#include "Root.h"

class VideoStoppedEvent : public Event
{
public:
	VideoStoppedEvent() {}

	virtual const char * typeStr() const
	{
		return "Video Stopped";
	}
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("328222-AB19-B3DD-FB6A-B63876331218");
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return obj->type() == Root().type();
	}


	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new VideoStoppedEvent(*this);
	}

	

	unsigned char version() const {return 0;}
	virtual void read(Reader & reader, unsigned char);
	virtual void write(Writer & writer) const;

private:
};


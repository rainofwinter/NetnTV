#pragma once

#include "Event.h"
#include "Image.h"

class LoadedEvent : public Event
{
public:

	LoadedEvent() 
	{
	}

	virtual const char * typeStr() const
	{
		return "Loaded";
	}

	virtual boost::uuids::uuid type() const
	{
		return sUuid_;
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return true;
	}


	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new LoadedEvent(*this);
	}

	unsigned char version() const {return 0;}
	virtual void read(Reader & reader, unsigned char);
	virtual void write(Writer & writer) const;

private:
	static boost::uuids::uuid sUuid_;
};

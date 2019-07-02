#pragma once
#include "Event.h"

class ReleaseEvent : public CursorEvent
{
public:
	virtual const char * typeStr() const
	{
		return "Release";
	}

	virtual JSObject * createScriptObject(ScriptProcessor * s, bool clonePrivate)
	{
		return CursorEvent::createScriptObject(s, clonePrivate);
	}

	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("8FF22723-AB19-A7FE-FB6A-B65C76A31216");
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return obj->visualAttrib() != NULL;
	}

	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new ReleaseEvent(*this);
	}

	unsigned char version() const {return 0;}
	virtual const char * xmlTag() const {return "Release";}
private:
};


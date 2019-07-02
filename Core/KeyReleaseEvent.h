#pragma once
#include "Event.h"

class KeyReleaseEvent : public KeyEvent
{
public:
	virtual const char * typeStr() const
	{
		return "Key Release";
	}
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("996100C1-DB11-C2F3-DAB7-B64CFDD31CA1");
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
		return new KeyReleaseEvent(*this);
	}

	unsigned char version() const {return 0;}

private:
};

#pragma once
#include "Event.h"

class KeyPressEvent : public KeyEvent
{
public:
	virtual const char * typeStr() const
	{
		return "Key Press";
	}
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("7968B2C3-AB19-C2FC-D20A-B64CF6631901");
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
		return new KeyPressEvent(*this);
	}

	unsigned char version() const {return 0;}

private:
};

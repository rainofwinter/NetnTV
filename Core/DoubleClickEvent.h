#ifndef DoubleClickEvent_h__
#define DoubleClickEvent_h__

#include "Event.h"

class DoubleClickEvent : public CursorEvent
{
public:
	virtual const char * typeStr() const
	{
		return "Double Tap";
	}
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("87829023-AB19-D446-D00D-B65476A31216");
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
		return new DoubleClickEvent(*this);
	}

	unsigned char version() const {return 0;}

private:
};


#endif // DoubleClickEvent_h__
#ifndef ClickEvent_h__
#define ClickEvent_h__

#include "Event.h"

class ClickEvent : public CursorEvent
{
public:
	virtual const char * typeStr() const
	{
		return "Tap";
	}

	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("82626723-AB19-A7FE-FB6A-B65476A31216");
	}

	virtual bool supportsObject(SceneObject * obj) const;
	//{
	//	return obj->visualAttrib() != NULL;
	//}

	virtual bool supportsObject(AppObject * obj) const;
	//{
	//	return obj->visualAttrib() != NULL;
	//}

	bool equals(const Event & rhs) const
	{
		return Event::equals(rhs);
	}

	Event * clone() const
	{
		return new ClickEvent(*this);
	}

	unsigned char version() const {return 0;}

	AppObject * apptarget;
private:
};


#endif // ClickEvent_h__
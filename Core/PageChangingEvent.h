#ifndef PageChangingEvent_h__
#define PageChangingEvent_h__

#include "Event.h"
#include "Root.h"

class PageChangingEvent : public Event
{
public:
	PageChangingEvent() {}

	virtual const char * typeStr() const
	{
		return "Page Changing";
	}
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("22228222-AB19-B8DD-FB6A-B65876A31218");
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
		return new PageChangingEvent(*this);
	}

	

	unsigned char version() const {return 0;}
	virtual void read(Reader & reader, unsigned char);
	virtual void write(Writer & writer) const;

private:
};


#endif // PageChangingEvent_h__
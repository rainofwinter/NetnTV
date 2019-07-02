#ifndef ReceiveMessageEvent_h__
#define ReceiveMessageEvent_h__

#include "Event.h"
#include "Root.h"

class ReceiveMessageEvent : public Event
{
public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * createScriptObject(ScriptProcessor * s, bool clonePrivate);

	ReceiveMessageEvent() {}
	ReceiveMessageEvent(const std::string & message)
	{
		message_ = message;
	}

	virtual const char * typeStr() const
	{
		return "Receive Message";
	}
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("82222222-AB19-BADD-FB6A-B65476A31216");
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return obj->type() == Root().type();
	}


	bool equals(const Event & rhs) const
	{
		bool sameType = Event::equals(rhs);
		if (!sameType) return false;
		const ReceiveMessageEvent & event = (const ReceiveMessageEvent &)rhs;
		return message_ == event.message_;
	}

	Event * clone() const
	{
		return new ReceiveMessageEvent(*this);
	}

	void setMessage(const std::string & message)
	{
		message_ = message;
	}

	const std::string & message() const
	{
		return message_;
	}

	unsigned char version() const {return 0;}
	virtual void read(Reader & reader, unsigned char);
	virtual void write(Writer & writer) const;



	virtual const char * xmlTag() const {return "ReceiveMessage";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);
private:
	std::string message_;
};


#endif // ReceiveMessageEvent_h__
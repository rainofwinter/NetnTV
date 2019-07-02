#pragma once

#include "Event.h"
#include "SceneChanger.h"
#include "ImageChanger.h"

class SceneChangerPageChangedEvent : public Event
{
public:
	
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * createScriptObject(ScriptProcessor * s, bool clonePrivate);

	SceneChangerPageChangedEvent() 
	{
		index_ = 0;
	}
	SceneChangerPageChangedEvent(const int & index)
	{
		index_ = index;
	}

	virtual const char * typeStr() const
	{
		return "Scene Changed";
	}
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("22223333-AB19-2222-FB6A-333476A31216");
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return 
			obj->type() == SceneChanger().type() || 
			obj->type() == ImageChanger().type();
	}


	bool equals(const Event & rhs) const
	{
		bool sameType = Event::equals(rhs);
		if (!sameType) return false;
		const SceneChangerPageChangedEvent & event = (const SceneChangerPageChangedEvent &)rhs;
		return index_ == event.index_;
	}

	Event * clone() const
	{
		return new SceneChangerPageChangedEvent(*this);
	}

	void setIndex(const int & index)
	{
		index_ = index;
	}

	const int & index() const
	{
		return index_;
	}

	unsigned char version() const {return 0;}
	virtual void read(Reader & reader, unsigned char);
	virtual void write(Writer & writer) const;

	virtual const char * xmlTag() const {return "PageChanged";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

private:
	int index_;
};


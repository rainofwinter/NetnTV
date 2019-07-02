#pragma once

#include "Event.h"
#include "AudioObject.h"

class AudioPlayTimeEvent : public Event
{
public:
	AudioPlayTimeEvent() 
	{
		time_ = 0.0f;
	}
	AudioPlayTimeEvent(const float & time)
	{
		time_ = time;
	}

	virtual const char * typeStr() const
	{
		return "Play Time";
	}
	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()		
		("12121313-1B19-1212-1B1A-131416131216");

		return uuid;
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return 
			obj->type() == AudioObject().type();
	}


	bool equals(const Event & rhs) const
	{
		bool sameType = Event::equals(rhs);
		if (!sameType) return false;
		const AudioPlayTimeEvent & event = (const AudioPlayTimeEvent &)rhs;
		return time_ == event.time_;
	}

	Event * clone() const
	{
		return new AudioPlayTimeEvent(*this);
	}

	void setTime(const float & time)
	{
		time_ = time;
	}

	const float & time() const
	{
		return time_;
	}

	unsigned char version() const {return 0;}
	virtual void read(Reader & reader, unsigned char);
	virtual void write(Writer & writer) const;

	virtual const char * xmlTag() const {return "AudioPlayTime";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

private:	
	float time_;
};


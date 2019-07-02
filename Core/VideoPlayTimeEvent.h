#pragma once

#include "Event.h"
#include "VideoObject.h"

class VideoPlayTimeEvent : public Event
{
public:
	VideoPlayTimeEvent() 
	{
		time_ = 0.0f;
	}
	VideoPlayTimeEvent(const float & time)
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
		("18bc1da3-b31a-4f2a-ac8e-461a82961f71");
		return uuid;
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return 
			obj->type() == VideoObject().type();
	}


	bool equals(const Event & rhs) const
	{
		bool sameType = Event::equals(rhs);
		if (!sameType) return false;
		const VideoPlayTimeEvent & event = (const VideoPlayTimeEvent &)rhs;
		return time_ == event.time_;
	}

	Event * clone() const
	{
		return new VideoPlayTimeEvent(*this);
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

	virtual const char * xmlTag() const {return "VideoPlayTime";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

private:	
	float time_;
};


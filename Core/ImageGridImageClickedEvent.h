#ifndef ImageGridImageClickedEvent_h__
#define ImageGridImageClickedEvent_h__

#include "Event.h"
#include "ImageGrid.h"

class ScriptProcessor;
struct JSObject;

class ImageGridImageClickedEvent : public Event
{
public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * createScriptObject(ScriptProcessor * s, bool clonePrivate);

	ImageGridImageClickedEvent() 
	{
		imageIndex_ = 0;
	}
	ImageGridImageClickedEvent(const int & imageIndex)
	{
		imageIndex_ = imageIndex;
	}

	virtual const char * typeStr() const
	{
		return "Image Tapped";
	}

	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("22222222-AB19-2222-FB6A-333476A31216");
	}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return obj->type() == ImageGrid().type();
	}


	bool equals(const Event & rhs) const
	{
		bool sameType = Event::equals(rhs);
		if (!sameType) return false;
		const ImageGridImageClickedEvent & event = (const ImageGridImageClickedEvent &)rhs;
		return imageIndex_ == event.imageIndex_;
	}

	Event * clone() const
	{
		return new ImageGridImageClickedEvent(*this);
	}

	void setImageIndex(const int & index)
	{
		imageIndex_ = index;
	}

	const int & imageIndex() const
	{
		return imageIndex_;
	}

	unsigned char version() const {return 0;}
	virtual void read(Reader & reader, unsigned char);
	virtual void write(Writer & writer) const;

	virtual const char * xmlTag() const {return "ImageGridImageClicked";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

private:
	int imageIndex_;
};


#endif // ImageGridImageClickedEvent_h__
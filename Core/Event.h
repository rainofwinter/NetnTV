#ifndef Event_h__
#define Event_h__

class Writer;
class Reader;
class XmlWriter;
class XmlReader;
class SceneObject;
class ScriptProcessor;
class AppObject;
struct JSObject;

bool isEvent(ScriptProcessor * s, JSObject * jsObj);

class Event
{
public:
	static JSObject * createBaseScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * createScriptObject(ScriptProcessor * s, bool clonePrivate = true);
		
	Event();

	virtual ~Event();
	virtual const char * typeStr() const = 0;
	virtual boost::uuids::uuid type() const = 0;
	virtual Event * clone() const = 0;

	/**
	Returns whether the specified event is of the same type as this one
	*/
	virtual bool equals(const Event & rhs) const = 0;

	virtual bool onlyForCurrentScene() const {return false;}

	virtual bool supportsObject(SceneObject * obj) const 
	{
		return true;
	}

	virtual bool supportsObject(AppObject * obj) const 
	{
		return true;
	}
	
	virtual unsigned char version() const = 0;
	virtual void read(Reader & reader, unsigned char);
	virtual void write(Writer & writer) const;

	virtual const char * xmlTag() const {return "";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);
	
	///The object associated with this event
	SceneObject * target;
protected:
};

///////////////////////////////////////////////////////////////////////////////

class CursorEvent : public Event
{
public:

	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * createScriptObject(ScriptProcessor * s, bool clonePrivate);

	float clientX;
	float clientY;

	int id;
};

class KeyEvent : public Event
{
public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * createScriptObject(ScriptProcessor * s, bool clonePrivate);

	int keyCode;
};

void Event_finalize(JSContext * cx, JSObject * obj); 

#endif // Event_h__
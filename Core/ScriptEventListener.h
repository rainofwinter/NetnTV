#pragma once

class Event;
class ScriptProcessor;
struct JSObject;

enum ScriptEvent
{
	ScriptEventTap,
	ScriptEventDblTap,
	ScriptEventPress,
	ScriptEventMove,
	ScriptEventRelease
};


class ScriptEventListener
{
public:
	ScriptEventListener(const boost::uuids::uuid & eventType, JSObject * funcObj);
	~ScriptEventListener();

	boost::uuids::uuid eventType() const {return eventType_;}

	bool operator == (const ScriptEventListener & rhs) const;

	JSObject * functionScriptObject() const {return funcObj_;}

	bool handle(Event * event, ScriptProcessor * s);
private:

	JSObject * funcObj_;
	boost::uuids::uuid eventType_;
};
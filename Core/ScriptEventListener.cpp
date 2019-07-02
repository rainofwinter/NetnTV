#include "stdafx.h"
#include "ScriptEventListener.h"
#include "Event.h"
#include "SceneObject.h"
#include "ScriptProcessor.h"




///////////////////////////////////////////////////////////////////////////////

ScriptEventListener::ScriptEventListener(
	const boost::uuids::uuid & eventType, JSObject * funcObj)
{
	eventType_ = eventType;	
	funcObj_ = funcObj;
}


ScriptEventListener::~ScriptEventListener()
{
}


bool ScriptEventListener::operator == (const ScriptEventListener & rhs) const
{
	return eventType_ == rhs.eventType_ && funcObj_ == rhs.funcObj_;
}

bool ScriptEventListener::handle(Event * event, ScriptProcessor * s)
{
	if (eventType_ == event->type()) 
	{	
		s->executeScriptEvent(event, *this);		
		return true;
	}
	return false;
}
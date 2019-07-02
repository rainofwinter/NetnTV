#include "stdafx.h"
#include "Event.h"
#include "Writer.h"
#include "Reader.h"
#include "ScriptProcessor.h"
#include "SceneObject.h"
#include "Root.h"
#include "Scene.h"
#include <jsobj.h>

void Event_finalize(JSContext * cx, JSObject * obj)
{		
	Event * privateData = (Event *)JS_GetPrivate(cx, obj);
	delete privateData;
}


bool isEvent(ScriptProcessor * s, JSObject * jsObj)
{
	JSContext * cx = s->jsContext();
	
	bool isEvent;
	JSObject * proto = 0;

	proto = JS_GetPrototype(cx, jsObj);	
	isEvent = false;	
	while(proto)
	{
		if (proto == s->baseEventProto()) isEvent = true;
		proto = proto->proto;
	}
	
	return isEvent;
}

JSClass jsEventClass = InitClass(
	"Event", JSCLASS_HAS_PRIVATE, JS_PropertyStub, 
	JS_StrictPropertyStub, Event_finalize, 0);

enum
{
	TARGET,
	TYPE
};


static JSBool Event_getProperty(
	JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{	
	Event * evt = (Event *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	*vp = JSVAL_VOID;

	switch (nID) {
	case TARGET:
		{
			SceneObject * targetObj = evt->target;		
			JSObject * jsObj;
			if (!targetObj)
			{
				*vp = JSVAL_NULL;
			}
			else
			{	
				if (targetObj->type() == Root().type())		
					jsObj = targetObj->parentScene()->getScriptObject(s);		
				else
					jsObj = targetObj->getScriptObject(s);

				*vp = OBJECT_TO_JSVAL(jsObj);
			}
			break;	
		}
	case TYPE:
		{
			const char * typeStr = evt->typeStr();
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, typeStr));	
			break;
		}
		
	}
	return JS_TRUE;
}


JSBool Event_setProperty(
	JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp)
{	
	Event * evt = (Event *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);

	switch (nID) {
	case TARGET:
		if (!JSVAL_IS_OBJECT(*vp) || !isSceneObject(s, JSVAL_TO_OBJECT(*vp)))
		{
			JS_ReportError(cx, "Event::target - Property must be a SceneObject.");
			return JS_FALSE;
		}		
		evt->target = (SceneObject *)JS_GetPrivate(cx, JSVAL_TO_OBJECT(*vp));
		break;
	}

	return JS_TRUE;
}


JSPropertySpec EventProps[] = {
	
	{"target",	TARGET, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Event_getProperty, Event_setProperty},

	{"type",	TYPE, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_READONLY, 
	Event_getProperty, JS_StrictPropertyStub},

	{0}
};


///////////////////////////////////////////////////////////////////////////////
Event::Event()
{
	target = NULL;
}

Event::~Event()
{
}

void Event::writeXml(XmlWriter & w) const
{

}

void Event::readXml(XmlReader & r, xmlNode * parent)
{	
}

JSObject * Event::createBaseScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * jsEventProto = 
		JS_InitClass(s->jsContext(), s->jsGlobal(), 0, &jsEventClass, 0, 0, EventProps, 0, 0, 0);
	s->rootJSObject(jsEventProto);
	return jsEventProto;
}

JSObject * Event::createScriptObjectProto(ScriptProcessor * s)
{		
	return createBaseScriptObjectProto(s);
}

JSObject * Event::createScriptObject(ScriptProcessor * s, bool clonePrivate)
{
	JSObject * obj = JS_NewObject(
		s->jsContext(), &jsEventClass, s->eventProto(this), 0);
	JS_SetPrivate(s->jsContext(), obj, clonePrivate?clone():this);
	return obj;
}


bool Event::equals(const Event & rhs) const
{
	return type() == rhs.type();
}

unsigned char Event::version() const
{
	return 0;
}



void Event::write(Writer & writer) const
{
}

void Event::read(Reader & reader, unsigned char version)
{
}



////////////////////////////////////////////////////////////////////////////////
JSClass jsCursorEventClass = InitClass(
	"CursorEvent", JSCLASS_HAS_PRIVATE, JS_PropertyStub, 
	JS_StrictPropertyStub, Event_finalize, 0);

enum
{
	CursorEvent_clientX,
	CursorEvent_clientY,
	CursorEvent_id,
};


static JSBool CursorEvent_getProperty(
	JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{	
	CursorEvent * evt = (CursorEvent *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	*vp = JSVAL_VOID;
	switch (nID) {
		case CursorEvent_clientX: JS_NewNumberValue(cx, evt->clientX, vp); break;	
		case CursorEvent_clientY: JS_NewNumberValue(cx, evt->clientY, vp); break;	
		case CursorEvent_id: JS_NewNumberValue(cx, evt->id, vp); break;
	}
	return JS_TRUE;
}

JSBool CursorEvent_setProperty(
	JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp)
{	
	CursorEvent * evt = (CursorEvent *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);

	switch (nID) {
	case CursorEvent_clientX:
		getFloat(cx, *vp, &evt->clientX);
		break;
	case CursorEvent_clientY:
		getFloat(cx, *vp, &evt->clientX);
		break;
	case CursorEvent_id:
		getInt(cx, *vp, &evt->id);
		break;
	}


	return JS_TRUE;
}

JSPropertySpec CursorEventProps[] = {

	
	{"clientX",	CursorEvent_clientX, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	CursorEvent_getProperty, CursorEvent_setProperty},

	{"clientY",	CursorEvent_clientY, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	CursorEvent_getProperty, CursorEvent_setProperty},

	{"id",	CursorEvent_id, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	CursorEvent_getProperty, CursorEvent_setProperty},

	{0}
};

////////////////////////////////////////////////////////////////////////////////


JSObject * CursorEvent::createScriptObjectProto(ScriptProcessor * s)
{		
	JSContext * cx = s->jsContext();
	JSObject * jsProto = 
		JS_InitClass(cx, s->jsGlobal(), s->baseEventProto(), 
			&jsCursorEventClass, 0, 0, CursorEventProps, 0, 0, 0);
	s->rootJSObject(jsProto);
	return jsProto;
}

JSObject * CursorEvent::createScriptObject(ScriptProcessor * s, bool clonePrivate)
{
	JSContext * cx = s->jsContext();
	JSObject * proto = s->eventProto(this);
	JSObject * obj = JS_NewObject(cx, &jsCursorEventClass, proto, 0);

	JS_SetPrivate(s->jsContext(), obj, clonePrivate?clone():this);
	return obj;
}



///////////////////////////////////////////////////////////////////////////////


JSClass jsKeyEventClass = InitClass(
	"KeyEvent", JSCLASS_HAS_PRIVATE, JS_PropertyStub, 
	JS_StrictPropertyStub, Event_finalize, 0);

enum
{
	KeyEvent_keyCode
};


static JSBool KeyEvent_getProperty(
	JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{	
	KeyEvent * evt = (KeyEvent *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	*vp = JSVAL_VOID;
	switch (nID) {
		case KeyEvent_keyCode: JS_NewNumberValue(cx, evt->keyCode, vp); break;
	}
	return JS_TRUE;
}

JSBool KeyEvent_setProperty(
	JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp)
{	
	KeyEvent * evt = (KeyEvent *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);

	switch (nID) {
	case KeyEvent_keyCode:
		getInt(cx, *vp, &evt->keyCode);
		break;
	}


	return JS_TRUE;
}

JSPropertySpec KeyEventProps[] = {

	
	{"keyCode",	KeyEvent_keyCode, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	KeyEvent_getProperty, KeyEvent_setProperty},

	{0}
};

////////////////////////////////////////////////////////////////////////////////


JSObject * KeyEvent::createScriptObjectProto(ScriptProcessor * s)
{		
	JSContext * cx = s->jsContext();
	JSObject * jsProto = 
		JS_InitClass(cx, s->jsGlobal(), s->baseEventProto(), 
			&jsKeyEventClass, 0, 0, KeyEventProps, 0, 0, 0);
	s->rootJSObject(jsProto);
	return jsProto;
}

JSObject * KeyEvent::createScriptObject(ScriptProcessor * s, bool clonePrivate)
{
	JSContext * cx = s->jsContext();
	JSObject * proto = s->eventProto(this);
	JSObject * obj = JS_NewObject(cx, &jsKeyEventClass, proto, 0);

	JS_SetPrivate(s->jsContext(), obj, clonePrivate?clone():this);
	return obj;
}



///////////////////////////////////////////////////////////////////////////////
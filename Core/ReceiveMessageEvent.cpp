#include "stdafx.h"
#include "ReceiveMessageEvent.h"
#include "Reader.h"
#include "Writer.h"
#include "Xml.h"
#include "ScriptProcessor.h"

////////////////////////////////////////////////////////////////////////////////
JSClass jsReceiveMessageEventClass = InitClass(
	"ReceiveMessageEvent", JSCLASS_HAS_PRIVATE, JS_PropertyStub, 
	JS_StrictPropertyStub, Event_finalize, 0);

enum
{
	ReceiveMessageEvent_message
};


static JSBool ReceiveMessageEvent_getProperty(
	JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{	
	ReceiveMessageEvent * evt = (ReceiveMessageEvent *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	*vp = JSVAL_VOID;
	switch (nID) {
		case ReceiveMessageEvent_message: 
			JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, evt->message().c_str())));			
			break;		
	}
	return JS_TRUE;
}

JSBool ReceiveMessageEvent_setProperty(
	JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp)
{	
	ReceiveMessageEvent * evt = (ReceiveMessageEvent *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	std::string msg;

	switch (nID) {
	case ReceiveMessageEvent_message:
		getString(cx, *vp, &msg);
		evt->setMessage(msg);
		break;
	}

	return JS_TRUE;
}

JSPropertySpec ReceiveMessageEventProps[] = {

	
	{"message",	ReceiveMessageEvent_message, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	ReceiveMessageEvent_getProperty, ReceiveMessageEvent_setProperty},

	{0}
};
///////////////////////////////////////////////////////////////////////////////

JSObject * ReceiveMessageEvent::createScriptObjectProto(ScriptProcessor * s)
{		
	JSContext * cx = s->jsContext();
	JSObject * jsProto = 
		JS_InitClass(cx, s->jsGlobal(), s->baseEventProto(), 
			&jsReceiveMessageEventClass, 0, 0, ReceiveMessageEventProps, 0, 0, 0);
	s->rootJSObject(jsProto);
	return jsProto;
}

JSObject * ReceiveMessageEvent::createScriptObject(ScriptProcessor * s, bool clonePrivate)
{
	JSContext * cx = s->jsContext();
	JSObject * proto = s->eventProto(this);
	JSObject * obj = JS_NewObject(cx, &jsReceiveMessageEventClass, proto, 0);

	JS_SetPrivate(s->jsContext(), obj, clonePrivate?clone():this);
	return obj;
}

void ReceiveMessageEvent::read(Reader & reader, unsigned char)
{
	reader.readParent<Event>(this);
	reader.read(message_);
	
}

void ReceiveMessageEvent::write(Writer & writer) const
{
	writer.writeParent<Event>(this);
	writer.write(message_);
}

void ReceiveMessageEvent::writeXml(XmlWriter & w) const
{
	w.writeTag("Message", message_);
}

void ReceiveMessageEvent::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(message_, curNode, "Message"));
	}
}
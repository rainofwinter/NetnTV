#include "stdafx.h"
#include "PageChangedEvent.h"
#include "Reader.h"
#include "Writer.h"
#include "Xml.h"
#include "ScriptProcessor.h"

///////////////////////////////////////////////////////////////////////////////
JSClass jsPageChangedEventClass = InitClass(
	"PageChangedEvent", JSCLASS_HAS_PRIVATE, JS_PropertyStub, 
	JS_StrictPropertyStub, Event_finalize, 0);

enum
{
	PageChangedEvent_index,
};


static JSBool PageChangedEvent_getProperty(
	JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{	
	SceneChangerPageChangedEvent * evt = (SceneChangerPageChangedEvent *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	*vp = JSVAL_VOID;
	switch (nID) {
		case PageChangedEvent_index: JS_NewNumberValue(cx, evt->index(), vp); break;	
	}
	return JS_TRUE;
}

JSBool PageChangedEvent_setProperty(
	JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp)
{	
	SceneChangerPageChangedEvent * evt = (SceneChangerPageChangedEvent *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	int index;

	switch (nID) {
	case PageChangedEvent_index:
		getInt(cx, *vp, &index);
		evt->setIndex(index);
		break;
	}


	return JS_TRUE;
}

JSPropertySpec PageChangedEventProps[] = {
	{"index",	PageChangedEvent_index, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	PageChangedEvent_getProperty, PageChangedEvent_setProperty},

	{0}
};


////////////////////////////////////////////////////////////////////////////////

JSObject * SceneChangerPageChangedEvent::createScriptObjectProto(ScriptProcessor * s)
{		
	JSContext * cx = s->jsContext();
	JSObject * jsProto = 
		JS_InitClass(cx, s->jsGlobal(), s->baseEventProto(), 
			&jsPageChangedEventClass, 0, 0, PageChangedEventProps, 0, 0, 0);
	s->rootJSObject(jsProto);
	return jsProto;
}

JSObject * SceneChangerPageChangedEvent::createScriptObject(ScriptProcessor * s, bool clonePrivate)
{
	JSContext * cx = s->jsContext();
	JSObject * proto = s->eventProto(this);
	JSObject * obj = JS_NewObject(cx, &jsPageChangedEventClass, proto, 0);

	JS_SetPrivate(s->jsContext(), obj, clonePrivate?clone():this);
	return obj;
}


////////////////////////////////////////////////////////////////////////////////

void SceneChangerPageChangedEvent::read(Reader & reader, unsigned char)
{
	reader.readParent<Event>(this);
	reader.read(index_);
	
}

void SceneChangerPageChangedEvent::write(Writer & writer) const
{
	writer.writeParent<Event>(this);
	writer.write(index_);
}

void SceneChangerPageChangedEvent::writeXml(XmlWriter & w) const
{
	w.writeTag("Index", index_);
}

void SceneChangerPageChangedEvent::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(index_, curNode, "Index"));
	}
}
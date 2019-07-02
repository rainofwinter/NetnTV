#include "stdafx.h"
#include "ImageGridImageClickedEvent.h"
#include "Reader.h"
#include "Writer.h"
#include "Xml.h"
#include "ScriptProcessor.h"

JSClass jsImageGridImageClickedEventClass = InitClass(
	"ImageGridImageClickedEvent", JSCLASS_HAS_PRIVATE, JS_PropertyStub, 
	JS_StrictPropertyStub, Event_finalize, 0);

enum
{
	ImageGridImageClickedEvent_index
};


static JSBool ImageGridImageClickedEvent_getProperty(
	JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{	
	ImageGridImageClickedEvent * evt = 
		(ImageGridImageClickedEvent *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	*vp = JSVAL_VOID;
	switch (nID) {
		case ImageGridImageClickedEvent_index: 
			JS_NewNumberValue(cx, evt->imageIndex(), vp); break;
	}
	return JS_TRUE;
}

JSPropertySpec ImageGridImageClickedEventProps[] = {
	{"index",	ImageGridImageClickedEvent_index, 
	JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_READONLY, 
	ImageGridImageClickedEvent_getProperty, JS_StrictPropertyStub},

	{0}
};


///////////////////////////////////////////////////////////////////////////////
JSObject * ImageGridImageClickedEvent::createScriptObjectProto(ScriptProcessor * s)
{		
	JSContext * cx = s->jsContext();
	JSObject * jsProto = 
		JS_InitClass(cx, s->jsGlobal(), s->baseEventProto(), 
			&jsImageGridImageClickedEventClass, 0, 0, 
			ImageGridImageClickedEventProps, 0, 0, 0);
	s->rootJSObject(jsProto);
	return jsProto;
}

JSObject * ImageGridImageClickedEvent::createScriptObject(ScriptProcessor * s, bool clonePrivate)
{
	JSContext * cx = s->jsContext();
	JSObject * proto = s->eventProto(this);
	JSObject * obj = JS_NewObject(cx, &jsImageGridImageClickedEventClass, proto, 0);

	JS_SetPrivate(s->jsContext(), obj, clonePrivate?clone():this);
	return obj;
}


void ImageGridImageClickedEvent::read(Reader & reader, unsigned char)
{
	reader.readParent<Event>(this);
	reader.read(imageIndex_);
	
}

void ImageGridImageClickedEvent::write(Writer & writer) const
{
	writer.writeParent<Event>(this);
	writer.write(imageIndex_);
}

void ImageGridImageClickedEvent::writeXml(XmlWriter & w) const
{
	w.writeTag("ImageIndex", imageIndex_);
}

void ImageGridImageClickedEvent::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(imageIndex_, curNode, "ImageIndex"));
	}
}
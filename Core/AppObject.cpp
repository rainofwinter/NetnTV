#include "stdafx.h"
#include "AppObject.h"
#include "Exception.h"
#include "Attrib.h"
#include "Writer.h"
#include "Reader.h"
#include "Xml.h"
#include "Global.h"
#include "ClickEvent.h"
#include "EventListener.h"
#include "ScriptProcessor.h"
#include "Document.h"

#include <jsobj.h>
using namespace std;

JSClass jsAppObjectClass = InitClass(
	"AppObject", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, AppObject::gcMark);

bool isAppObject(ScriptProcessor * s, JSObject * jsObj)
{
	JSContext * cx = s->jsContext();
	
	bool isAppObject;
	JSObject * proto = 0;

	proto = JS_GetPrototype(cx, jsObj);	
	isAppObject = false;	
	while(proto)
	{
		if (proto == s->baseAppObjectProto()) isAppObject = true;
		proto = proto->proto;
	}
	
	return isAppObject;
}

JSBool AppObject_getVisible(JSContext *cx, uintN argc, jsval *vp)
{
	AppObject * thisObj = (AppObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	
	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(thisObj->visible()));

	return JS_TRUE;
}

JSBool AppObject_setVisible(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	AppObject * thisObj = (AppObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSBool jsBool;
	if (!JS_ConvertArguments(cx, argc, argv, "b", &jsBool)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	thisObj->setVisible((bool)jsBool);
	Global::instance().appObjectisVisible();
	s->document()->triggerRedraw();
	return JS_TRUE;
}

JSFunctionSpec AppObjectFuncs[] = {
	JS_FS("getVisible", AppObject_getVisible, 0, 0),
	JS_FS("setVisible", AppObject_setVisible, 1, 0),

    JS_FS_END
};

enum
{
	AppObject_parent
};

static JSBool AppObject_getProperty(
	JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{	
	/*AppObject * thisObj = (AppObject *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	*vp = JSVAL_VOID;

	switch (nID) {
	case AppObject_parent:
		{
			AppObject * parent = thisObj->parent();
			Scene * parentScene = thisObj->parentScene();
			if (parent->type() == Root().type())
			{
				JS_SET_RVAL(cx, vp, 
					OBJECT_TO_JSVAL(parentScene->getScriptObject(s)));
			}
			else if (parent)
			{
				JS_SET_RVAL(cx, vp, 
					OBJECT_TO_JSVAL(parent->getScriptObject(s)));
			}
			else
			{
				JS_SET_RVAL(cx, vp, JSVAL_NULL);
			}
		}
		break;
		
	}*/
	return JS_TRUE;
}

JSPropertySpec AppObjectProps[] = {	

	{"parent",	AppObject_parent, 
	JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_READONLY, 
	AppObject_getProperty, JS_StrictPropertyStub},

	{0}
};
///////////////////////////////////////////////////////////////////////////////

boost::uuids::string_generator AppObject::sUuidGen_;


JSObject * AppObject::createBaseScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * jsAppObjectProto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), 0, &jsAppObjectClass,
		0, 0, AppObjectProps, AppObjectFuncs, 0, 0);

	s->rootJSObject(jsAppObjectProto);
	return jsAppObjectProto;
}

JSObject * AppObject::createScriptObjectProto(ScriptProcessor * s)
{			
	return createBaseScriptObjectProto(s);
}

JSObject * AppObject::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsAppObjectClass, s->appObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);
	}

	return scriptObject_;
}

void AppObject::gcMark(JSTracer * trc, JSObject * obj)
{
	JSContext * cx = trc->context;
	 AppObject * o = (AppObject *)JS_GetPrivate(cx, obj);
	if (o)
	{
		vector<ScriptEventListener>::iterator iter;		
		for (iter = o->scriptEventListeners_.begin(); iter != o->scriptEventListeners_.end(); ++iter)
		{				
			JS_TRACER_SET_DEBUG_VARS(trc)
			JS_CallTracer(trc, (*iter).functionScriptObject(), JSTRACE_OBJECT);			
		}		
		
		BOOST_FOREACH(AppObjectSPtr & child, o->children_)
		{
			if (child->scriptObject_)
			{
				JS_TRACER_SET_DEBUG_VARS(trc)
				JS_CallTracer(trc, child->scriptObject_, JSTRACE_OBJECT);
			}
		}
		
	}
}



//rhs should be the "parent"
void AppObjectOffset::addOffset(const AppObjectOffset & rhs)
{
	sx = sx * rhs.sx;
	sy = sy * rhs.sy;

	dx = rhs.sx * dx + rhs.dx;
	dy = rhs.sy * dy + rhs.dy;

	float lcx, lcy, lcw, lch;
	lcx = rhs.sx * clipX + rhs.dx;
	lcy = rhs.sy * clipY + rhs.dy;
	lcw = clipWidth * rhs.sx;
	lch = clipHeight * rhs.sy;
	
	if (!rhs.clip)
	{
		clipX = lcx;
		clipY = lcy;
		clipWidth = lcw;
		clipHeight = lch;
	}
	else //intersect clipping regions
	{	
		float rcx, rcy, rcw, rch;

		rcx = rhs.clipX;
		rcy = rhs.clipY;
		rcw = rhs.clipWidth;
		rch = rhs.clipHeight;

		if (clip)
		{
			if (lcx < rcx)
			{
				clipX = rcx;
				if (lcx + lcw < rcx + rcw)				
					clipWidth = lcx + lcw - rcx;
				else
					clipWidth = rcw;
			}
			else
			{
				clipX = lcx;
				if (rcx + rcw < lcx + lcw)
					clipWidth = rcx + rcw - lcx;
				else
					clipWidth = lcw;
			}

			if (lcy < rcy)
			{
				clipY = rcy;
				if (lcy + lch < rcy + rch)
					clipHeight = lcy + lch - rcy;
				else
					clipHeight = rch;
			}
			else
			{
				clipY = lcy;
				if (rcy + rch < lcy + lch)
					clipHeight = rcy + rch - lcy;
				else
					clipHeight = lch;
			}

			clip = true;
		}
		else
		{
			clip = rhs.clip;
			clipX = rcx;
			clipY = rcy;
			clipWidth = rcw;
			clipHeight = rch;
		}
	}	
}


void AppObject::create()
{
	init_ = false;
	stringId_ = "New App-Level Object";
//	parentScene_ = 0;
	affectedByOffset_ = true;
	handleEvents_ = false;
	scriptObject_ = 0;
	x_ = 0;
	y_ = 0;
	opacity_ = 1.0f;
	visible_ = true;
	width_ = 640;
	height_ = 480;
}

AppObject::AppObject()
{
	create();
}

AppObject::AppObject(const AppObject & rhs)
{
	create();
		
	//don't copy init_ field for now
	containerObjects_ = rhs.containerObjects_;
	stringId_ = rhs.stringId_;
	affectedByOffset_ = rhs.affectedByOffset_;
	handleEvents_ = rhs.handleEvents_;
	opacity_ = rhs.opacity_;
	visible_ = rhs.visible_;
	x_ = rhs.x_;
	y_ = rhs.y_;
	width_ = rhs.width_;
	height_ = rhs.height_;
}

void AppObject::referencedFiles(std::vector<std::string> * refFiles) const
{
	BOOST_FOREACH(EventListenerSPtr listener, eventListeners_)
	{
		listener->referencedFiles(refFiles);
	}
}

int AppObject::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int startIndex)
{	
	int i = startIndex;
	BOOST_FOREACH(EventListenerSPtr listener, eventListeners_)
	{
		i = listener->setReferencedFiles(baseDirectory, refFiles, i);
	}
	return i;
}

AppObject::~AppObject()
{
	uninit();
}


void AppObject::init()
{
	uninit();
	init_ = true;
}

void AppObject::uninit()
{
	if (!init_) return;
	init_ = false;
}

bool AppObject::handleEvents() const
{
	 
	 VisualAttrib * attr = const_cast<AppObject *>(this)->visualAttrib();
	 if (handleEvents_)
	 {
		 if (!attr) return true;
		 else return attr->isVisible();
	 }
	 else
	 {
		 return false;
	 }
	 
 
 }

/*
void AppObject::setParentScene(Scene * parentScene)
{
	parentScene_ = parentScene;
}
*/

void AppObject::setContainerObjects(const ContainerObjects & containerObjects)
{
	containerObjects_ = containerObjects;
}

BoundingBox AppObject::worldExtents() const
{
	BoundingBox bbox;

	bbox.minPt.x = (float)x_;
	bbox.minPt.y = (float)y_;
	bbox.minPt.z = 0;

	bbox.maxPt.x = (float)(x_ + width_);
	bbox.maxPt.y = (float)(y_ + height_);
	bbox.maxPt.z = 0;

	return bbox;
		}
		

unsigned char AppObject::version() const
{
	return 2;
}

Scene * AppObject::parentScene() const
{
	if (containerObjects_.empty()) return NULL;

	return containerObjects_.front().second;
}

void AppObject::write(Writer & writer) const
{
	writer.write(stringId_, "stringId");
	writer.write(x_, "x");
	writer.write(y_, "y");
	writer.write(width_, "width");
	writer.write(height_, "height");

	writer.write(opacity_, "opacity");
	writer.write(visible_, "visible");
	writer.write(eventListeners_, "eventListeners");
}

void AppObject::read(Reader & reader, unsigned char version) 
{
	reader.read(stringId_);	
	if (version >= 1)
	{
		reader.read(x_);
		reader.read(y_);
		reader.read(width_);
		reader.read(height_);

		reader.read(opacity_);
		reader.read(visible_);
		if (version >= 2)
		{
			reader.read(eventListeners_);
		}
	}
}

void AppObject::writeXml(XmlWriter & w) const
{
	w.writeTag("StringId", stringId_);
	w.writeTag("X", x_);
	w.writeTag("Y", y_);
	w.writeTag("Width", width_);
	w.writeTag("Height", height_);
	w.writeTag("Opacity", opacity_);
	w.writeTag("Visible", visible_);
	
}

void AppObject::readXml(XmlReader & r, xmlNode * parent)
{
	unsigned int id = 0;
	r.getNodeAttribute(id, parent, "Id");
	if (!id)
		throw XmlException(parent, "No Id");
	r.idMapping().setId(this, id);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(stringId_, curNode, "StringId"));
		else if (r.getNodeContentIfName(x_, curNode, "X"));
		else if (r.getNodeContentIfName(y_, curNode, "Y"));
		else if (r.getNodeContentIfName(width_, curNode, "Width"));
		else if (r.getNodeContentIfName(height_, curNode, "Height"));
		else if (r.getNodeContentIfName(opacity_, curNode, "Opacity"));
		else if (r.getNodeContentIfName(visible_, curNode, "Visible"));
	}
}


//
//// add event function
bool AppObject::clickEvent(const Vector2 & mousePos, int pressId)
{
	static ClickEvent event;
	event.apptarget = this;
	event.id = pressId;
	event.clientX = mousePos.x;
	event.clientY = mousePos.y;	

	bool handled = handleEvent(&event, Global::currentTime());
	return handled;
}


bool AppObject::handleEvent(Event * event, float sceneTime)
{
	bool handled = false;
	BOOST_FOREACH(EventListenerSPtr & listener, eventListeners_)
	{
		if (!event->onlyForCurrentScene())
			handled |= listener->handleAppObj(this, event, sceneTime);
	}
	
	return handled;
}

EventListenerSPtr AppObject::deleteListener(EventListener * listener)
{
	vector<EventListenerSPtr>::iterator iter = eventListeners_.begin();
	for (; iter != eventListeners_.end(); ++iter)
	{
		EventListenerSPtr & curListener = (*iter);
		if (curListener.get() == listener)
		{
			eventListeners_.erase(iter);
			return curListener;
		}
	}

	return EventListenerSPtr();
}

EventListenerSPtr AppObject::findListener(EventListener * listener)
{
	vector<EventListenerSPtr>::iterator iter = eventListeners_.begin();
	for (; iter != eventListeners_.end(); ++iter)
	{
		EventListenerSPtr & curListener = (*iter);
		if (curListener.get() == listener) return curListener;
	}

	return EventListenerSPtr();
}

void AppObject::setEventListeners(
	const std::vector<EventListenerSPtr> & listeners)
{
	eventListeners_ = listeners;
}
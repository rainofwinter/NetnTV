#include "stdafx.h"
#include "SceneObject.h"
#include "EventListener.h"
#include "Exception.h"
#include "Attrib.h"
#include "Writer.h"
#include "Reader.h"
#include "Scene.h"
#include "PressEvent.h"
#include "ReleaseEvent.h"
#include "MoveEvent.h"
#include "ClickEvent.h"
#include "DoubleClickEvent.h"
#include "TempRenderObject.h"
#include "GfxRenderer.h"
#include "ElementMapping.h"
#include "ReceiveMessageEvent.h"
#include "Global.h"
#include "Document.h"
#include "ScriptProcessor.h"
#include "LoadedEvent.h"
#include "Xml.h"
#include <jsobj.h>
using namespace std;

JSClass jsSceneObjectClass = InitClass(
	"SceneObject", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);

bool isSceneObject(ScriptProcessor * s, JSObject * jsObj)
{
	JSContext * cx = s->jsContext();
	
	bool isSceneObject;
	JSObject * proto = 0;

	proto = JS_GetPrototype(cx, jsObj);	
	isSceneObject = false;	
	while(proto)
	{
		if (proto == s->baseSceneObjectProto()) isSceneObject = true;
		proto = proto->proto;
	}
	
	return isSceneObject;
}

JSBool SceneObject_addEventListener(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	jschar * type;
	JSObject * listener;
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	if (!JS_ConvertArguments(cx, argc, argv, "Wo", &type, &listener))
	{
		JS_ReportError(cx, "Incorrect parameters");
        return JS_FALSE;
	}

	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	SceneObject * sceneObj = (SceneObject *)JS_GetPrivate(cx, obj);
	string typeStr = jsCharStrToStdString(type);

	Event * event = Global::instance().event(sceneObj, typeStr);
	if (!event) 
	{
		JS_ReportError(cx, "Unrecognized event type");
		return JS_FALSE;
	}
	
	if (!JS_ObjectIsFunction(cx, listener))
	{
		JS_ReportError(cx, "Event listener must be a function");
		return JS_FALSE;
	}
	
	bool added = 
		sceneObj->addScriptEventListener(ScriptEventListener(event->type(), listener));

	return JS_TRUE;
}


JSBool SceneObject_removeEventListener(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	jschar * type;
	JSObject * listener;
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	if (!JS_ConvertArguments(cx, argc, argv, "Wo", &type, &listener))
        return JS_FALSE;

	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	SceneObject * sceneObj = (SceneObject *)JS_GetPrivate(cx, obj);
	string typeStr = jsCharStrToStdString(type);

	Event * event = Global::instance().event(sceneObj, typeStr);
	if (!event) 
	{
		JS_ReportError(cx, "Unrecognized event type");
		return JS_FALSE;	
	}
	
	bool removed = 
		sceneObj->removeScriptEventListener(ScriptEventListener(event->type(), listener));

	if (removed) s->unrootJSObject(listener);
	
	return JS_TRUE;
};

JSBool SceneObject_getId(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "")) return JS_FALSE;
	
	const char * idStr = thisObj->id().c_str();
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, idStr)));
	return JS_TRUE;
};

JSBool SceneObject_setId(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	jschar * jsId;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &jsId)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	std::string id = jsCharStrToStdString(jsId);		
	thisObj->setId(id);
	
	return JS_TRUE;
};

JSBool SceneObject_getType(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "")) return JS_FALSE;

	const char * idStr = thisObj->typeStr();
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, idStr)));
	return JS_TRUE;
};

JSBool SceneObject_getTransformMatrix(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "")) return JS_FALSE;

	VisualAttrib * attr = thisObj->visualAttrib();
	if (!attr) 
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	else
	{
		JSObject * jsTransform = attr->transformMatrix().createScriptObject(s);	
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsTransform));
	}
	return JS_TRUE;
};

JSBool SceneObject_getTotalTransformMatrix(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "")) return JS_FALSE;

	VisualAttrib * attr = thisObj->visualAttrib();
	if (!attr) 
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	else
	{
		JSObject * jsTransform = 
			(thisObj->parentTransform() * attr->transformMatrix()).createScriptObject(s);
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsTransform));
	}
	return JS_TRUE;
};

JSBool SceneObject_getTransform(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "")) return JS_FALSE;

	VisualAttrib * attr = thisObj->visualAttrib();
	if (!attr) 
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	else
	{
		JSObject * jsTransform = attr->transform().createScriptObject(s);	
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsTransform));
	}
	return JS_TRUE;
};


JSBool SceneObject_setTransform(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsTransform;

	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsTransform)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	
	VisualAttrib * attr = thisObj->visualAttrib();

	if (jsTransform)
	{			
		if (JS_GetPrototype(cx, jsTransform) == s->transformProto())
		{		
			if (attr)
			{	
			//This doesn't work if the script has changed the script transform object.
			//It becomes out of sync with the private Transform object.
				Transform * transform = (Transform *)JS_GetPrivate(cx, jsTransform);
				attr->setTransform(*transform);		
			}
		}
		else if (JS_GetPrototype(cx, jsTransform) == s->matrixProto())
		{
			Matrix * matrix = (Matrix *)JS_GetPrivate(cx, jsTransform);
			attr->setTransformMatrix(*matrix);
		}
		else
		{
			if (attr)
			{
				Transform transform = Transform::fromJsonScriptObject(s, jsTransform);
				attr->setTransform(transform);		
			}
		}
		s->document()->triggerRedraw();
	}

	return JS_TRUE;
};

JSBool SceneObject_getParentScene(JSContext *cx, uintN argc, jsval *vp)
{
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	Scene * parentScene = thisObj->parentScene();	
	if (parentScene)	
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(parentScene->getScriptObject(s)));
	return JS_TRUE;
};

JSBool SceneObject_getParent(JSContext *cx, uintN argc, jsval *vp)
{
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	SceneObject * parent = thisObj->parent();
	if (parent)	
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(parent->getScriptObject(s)));
	return JS_TRUE;
};


JSBool SceneObject_getVisible(JSContext *cx, uintN argc, jsval *vp)
{
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	VisualAttrib * attr = thisObj->visualAttrib();
	if (!attr) 
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	else
		JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(attr->isVisible()));

	return JS_TRUE;
};

JSBool SceneObject_setVisible(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSBool jsBool;
	if (!JS_ConvertArguments(cx, argc, argv, "b", &jsBool)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	VisualAttrib * attr = thisObj->visualAttrib();
	if (attr) attr->setVisible((bool)jsBool);
	s->document()->triggerRedraw();
	return JS_TRUE;
};

JSBool SceneObject_getOpacity(JSContext *cx, uintN argc, jsval *vp)
{
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	VisualAttrib * attr = thisObj->visualAttrib();
	if (!attr) 
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	else
		JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL((double)attr->opacity()));

	return JS_TRUE;
};

JSBool SceneObject_setOpacity(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsdouble jsDouble;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsDouble)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	VisualAttrib * attr = thisObj->visualAttrib();
	if (attr) attr->setOpacity((float)jsDouble);
	s->document()->triggerRedraw();
	return JS_TRUE;
};



JSBool SceneObject_init(JSContext *cx, uintN argc, jsval *vp)
{
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	thisObj->init(s->document()->renderer());
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool SceneObject_uninit(JSContext *cx, uintN argc, jsval *vp)
{
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	thisObj->uninit();
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool SceneObject_isInit(JSContext *cx, uintN argc, jsval *vp)
{
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));	
	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL((JSBool)thisObj->isInit()));
	return JS_TRUE;
}

JSBool SceneObject_getWorldExtents(JSContext *cx, uintN argc, jsval *vp)
{
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	BoundingBox bb = thisObj->worldExtents();

	JSObject * jsBb = bb.createJsonScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsBb));
	return JS_TRUE;
};

JSBool SceneObject_getExtents(JSContext *cx, uintN argc, jsval *vp)
{
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	BoundingBox bb = thisObj->extents();

	JSObject * jsBb = bb.createJsonScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsBb));
	return JS_TRUE;
}

JSBool SceneObject_createEvent(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	const jschar * jsType;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &jsType)) return JS_FALSE;
	string type = jsCharStrToStdString(jsType);
	Event * event = Global::instance().createEvent(thisObj, type);
	if (!event)
	{
		JS_ReportError(cx, "SceneObject::createEvent - Invalid type.");
		return JS_FALSE;
	}
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsEvt = event->createScriptObject(s, false);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsEvt));
	return JS_TRUE;
}

JSBool SceneObject_dispatchEvent(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * jsEvt;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsEvt)) return JS_FALSE;
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!isEvent(s, jsEvt))
	{
		JS_ReportError(cx, "SceneObject::dispatchEvent - Parameter must be an Event.");
		return JS_FALSE;
	}

	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	Event * event = (Event *)JS_GetPrivate(cx, jsEvt);
	thisObj->handleEvent(event, Global::currentTime());
	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};


JSFunctionSpec SceneObjectFuncs[] = {
	JS_FS("addEventListener", SceneObject_addEventListener, 2, 0),
	JS_FS("removeEventListener", SceneObject_removeEventListener, 2, 0),

	JS_FS("getId", SceneObject_getId, 0, 0),
	JS_FS("setId", SceneObject_setId, 1, 0),

	JS_FS("getType", SceneObject_getType, 0, 0),

	JS_FS("getTotalTransformMatrix", SceneObject_getTotalTransformMatrix, 0, 0),
	JS_FS("getTransformMatrix", SceneObject_getTransformMatrix, 0, 0),
	JS_FS("getTransform", SceneObject_getTransform, 0, 0),
	JS_FS("setTransform", SceneObject_setTransform, 1, 0),

	JS_FS("getVisible", SceneObject_getVisible, 0, 0),
	JS_FS("setVisible", SceneObject_setVisible, 1, 0),

	JS_FS("getOpacity", SceneObject_getOpacity, 0, 0),
	JS_FS("setOpacity", SceneObject_setOpacity, 1, 0),

	JS_FS("init", SceneObject_init, 0, 0),
	JS_FS("uninit", SceneObject_uninit, 0, 0),
	JS_FS("isInit", SceneObject_isInit, 0, 0),

	JS_FS("getWorldExtents", SceneObject_getWorldExtents, 0, 0),
	JS_FS("getExtents", SceneObject_getExtents, 0, 0),

	JS_FS("createEvent", SceneObject_createEvent, 1, 0),
	JS_FS("dispatchEvent", SceneObject_dispatchEvent, 1, 0),

	JS_FS("getParentScene", SceneObject_getParentScene, 0, 0),
	JS_FS("getParent", SceneObject_getParent, 0, 0),

    JS_FS_END
};


//-----------------------------------------------------------------------------

enum
{
	SceneObject_parent
};

static JSBool SceneObject_getProperty(
	JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{	
	SceneObject * thisObj = (SceneObject *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	*vp = JSVAL_VOID;

	switch (nID) {
	case SceneObject_parent:
		{
			SceneObject * parent = thisObj->parent();
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
		
	}
	return JS_TRUE;
}

JSPropertySpec SceneObjectProps[] = {	

	{"parent",	SceneObject_parent, 
	JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_READONLY, 
	SceneObject_getProperty, JS_StrictPropertyStub},

	{0}
};

///////////////////////////////////////////////////////////////////////////////

boost::uuids::string_generator SceneObject::sUuidGen_;


JSObject * SceneObject::createBaseScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * jsSceneObjectProto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), 0, &jsSceneObjectClass,
		0, 0, SceneObjectProps, SceneObjectFuncs, 0, 0);

	s->rootJSObject(jsSceneObjectProto);
	return jsSceneObjectProto;
}

JSObject * SceneObject::createScriptObjectProto(ScriptProcessor * s)
{			
	return createBaseScriptObjectProto(s);
}

JSObject * SceneObject::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsSceneObjectClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void SceneObject::gcMark(JSTracer * trc, JSObject * obj)
{
	JSContext * cx = trc->context;
	SceneObject * o = (SceneObject *)JS_GetPrivate(cx, obj);
	if (o)
	{
		vector<ScriptEventListener>::iterator iter;		
		for (iter = o->scriptEventListeners_.begin(); iter != o->scriptEventListeners_.end(); ++iter)
		{				
			JS_TRACER_SET_DEBUG_VARS(trc)
			JS_CallTracer(trc, (*iter).functionScriptObject(), JSTRACE_OBJECT);			
		}		
		
		BOOST_FOREACH(SceneObjectSPtr & child, o->children_)
		{
			if (child->scriptObject_)
			{
				JS_TRACER_SET_DEBUG_VARS(trc)
				JS_CallTracer(trc, child->scriptObject_, JSTRACE_OBJECT);
			}
		}
		
	}
}




void SceneObject::create()
{
	gl_ = 0;
	stringId_ = "New Object";
	parentScene_ = 0;
	parent_ = 0;
	scriptObject_ = 0;
	isLoadedDispatched_ = false;
}

SceneObject::SceneObject()
{
	create();
}

SceneObject::SceneObject(const SceneObject & rhs, ElementMapping * elementMapping)
{
	create();
	if (elementMapping)
		elementMapping->addObjectMapping(const_cast<SceneObject *>(&rhs), this);

	BOOST_FOREACH(SceneObjectSPtr rhsChild, rhs.children_)	
	{
		children_.push_back(SceneObjectSPtr(rhsChild->clone(elementMapping)));
		children_.back()->parent_ = this;
	}

	eventListeners_.clear();
		
	BOOST_FOREACH(EventListenerSPtr eventListener, rhs.eventListeners_)	
		eventListeners_.push_back(EventListenerSPtr(
			new EventListener(*eventListener.get())));	

	parent_ = rhs.parent_;
	parentScene_ = rhs.parentScene_;
	stringId_ = rhs.stringId_;
}

void SceneObject::remapReferences(const ElementMapping & elementMapping)
{
	parentScene_ = elementMapping.mapScene(parentScene_);

	BOOST_FOREACH(SceneObjectSPtr child, children_)		
		child->remapReferences(elementMapping);	

	BOOST_FOREACH(EventListenerSPtr listener, eventListeners_)
	{
		listener->remapReferences(elementMapping);
	}
}

void SceneObject::referencedFiles(std::vector<std::string> * refFiles) const
{
	BOOST_FOREACH(EventListenerSPtr listener, eventListeners_)
	{
		listener->referencedFiles(refFiles);
	}

	BOOST_FOREACH(SceneObjectSPtr child, children_)		
		child->referencedFiles(refFiles);
}

int SceneObject::setReferencedFiles(
	const std::string & baseDirectory,
	const vector<string> & refFiles, int startIndex)
{
	int i = startIndex;
	BOOST_FOREACH(EventListenerSPtr listener, eventListeners_)
	{
		i = listener->setReferencedFiles(baseDirectory, refFiles, i);
	}

	BOOST_FOREACH(SceneObjectSPtr child, children_)		
		i = child->setReferencedFiles(baseDirectory, refFiles, i);

	return i;
}

const Transform & SceneObject::transform() const
{
	return const_cast<SceneObject *>(this)->visualAttrib()->transform();
}

void SceneObject::setTransform(const Transform & transform)
{
	visualAttrib()->setTransform(transform);
}

void SceneObject::draw(GfxRenderer * gl) const
{	
	bool hasTransform = false;
	VisualAttrib * attr = const_cast<SceneObject *>(this)->visualAttrib();
	if (attr) 
	{
		if (!attr->isVisible() || totalOpacity() == 0) return;
		hasTransform = !attr->transform().isIdentity();

		if (hasTransform)
		{
			gl->pushMatrix();
			gl->multMatrix(attr->transformMatrix());
		}
	}

	drawObject(gl);
	BOOST_FOREACH(const SceneObjectSPtr & obj, children_)
	{		
		obj->draw(gl);
	}
	
	
	vector<TempRenderObject *>::iterator iter = tempRenderObjects_.begin();
	for (; iter != tempRenderObjects_.end();)
	{
		TempRenderObject * tempRObj = *iter;
		tempRObj->draw(gl);
		//gl->drawRect(100, 612, 512, -512);
		if (tempRObj->isFinished())	
		{
			iter = tempRenderObjects_.erase(iter);
			tempRObj->uninit();
			delete tempRObj;
		}
		else		
			++iter;		
	}	
	
	if (hasTransform) gl->popMatrix();

}


SceneObject::~SceneObject()
{	
	//don't call uninit here, let that be done in derived classes.
	BOOST_FOREACH(TempRenderObject * obj, tempRenderObjects_)
		delete obj;

}

GfxRenderer * SceneObject::renderer() const
{
	return parentScene_->parentDocument()->renderer();
}

void SceneObject::init(GfxRenderer * gl, bool firstTime)
{		
	if (firstTime) isLoadedDispatched_ = false;
	if (gl_) uninit();
	gl_ = gl;	
	for (int i = 0; i < (int)children_.size(); ++i) children_[i]->init(gl, firstTime);
}

void SceneObject::uninit()
{
	gl_ = 0;
	for (int i = 0; i < (int)children_.size(); ++i) children_[i]->uninit();
}

void SceneObject::addChild(const SceneObjectSPtr & child)
{
	parentScene_->flagObjectsListChanged();
	SceneObject * prevParent = child->parent();
	if (prevParent) prevParent->removeChild(child.get());

	children_.push_back(child);
	child->setParent(this);		
	child->triggerOnSetTransform();	
}

void SceneObject::triggerOnSetTransform()
{	
	VisualAttrib * attr = visualAttrib();
	if (attr) attr->onSetTransform();
}

SceneObjectSPtr SceneObject::removeChild(SceneObject * child)
{
	vector<SceneObjectSPtr>::iterator iter;
	for (iter = children_.begin(); iter != children_.end(); ++iter)
	{
		SceneObjectSPtr curChild = *iter;
		if (curChild.get() == child)
		{
			curChild->setParent(0);			
			children_.erase(iter);
			
			child->triggerOnSetTransform();		

			//TODO maybe do an auto uninit here?::
			return curChild;
		}
	}
	return SceneObjectSPtr();
}

void SceneObject::setParent(SceneObject * parent)
{
	parent_ = parent;
	if (parent)
	{
		setParentScene(parent->parentScene());		
	}
	else
	{ 
		//leave parentScene_ valid (don't set it to null)
		
		//actually, try setting parentScene_ to null
		setParentScene(NULL);		
	}
	
}

void SceneObject::setAsSubObject(SceneObject * obj)
{
	obj->parentScene_ = parentScene_;	
	obj->parent_ = this;
}

void SceneObject::insertBefore(
	const SceneObjectSPtr & newObj, SceneObject * refObj)
{
	parentScene_->flagObjectsListChanged();
	vector<SceneObjectSPtr>::iterator insertIter, iter;
	if (newObj.get() == refObj) return;

	SceneObject * prevParent = newObj->parent();
	if (prevParent) prevParent->removeChild(newObj.get());
	/*
	if (newObj->parent_) throw Exception(InvalidArguments, 
		"newObj must not be contained in the scene");*/

	insertIter = children_.end();	
	for (iter = children_.begin(); iter != children_.end(); ++iter)	
		if ((*iter).get() == refObj) insertIter = iter;

	/*
	if (insertIter == children_.end() && refObj) throw Exception(InvalidArguments,
		"refObj not found");*/

	children_.insert(insertIter, newObj);
	newObj->parent_ = this;
	newObj->triggerOnSetTransform();
	newObj->setParentScene(parentScene_);		
}

void SceneObject::insertAfter(
	const SceneObjectSPtr & newObj, SceneObject * refObj)
{
	parentScene_->flagObjectsListChanged();
	vector<SceneObjectSPtr>::iterator insertIter, iter;
	if (newObj.get() == refObj) return;
	/*
	if (newObj->parent_) throw Exception(InvalidArguments, 
		"newObj must not be contained in the scene");
	*/
	SceneObject * prevParent = newObj->parent();
	if (prevParent) prevParent->removeChild(newObj.get());

	insertIter = children_.end();	
	for (iter = children_.begin(); iter != children_.end(); ++iter)	
		if ((*iter).get() == refObj) 
		{
			insertIter = iter;
		}
	/*
	if (insertIter == children_.end() && refObj) throw Exception(InvalidArguments,
		"refObj not found");
	*/
	if (insertIter != children_.end()) ++insertIter;

	children_.insert(insertIter, newObj);
	newObj->parent_ = this;
	newObj->triggerOnSetTransform();
	newObj->setParentScene(parentScene_);
}

SceneObjectSPtr SceneObject::deleteChild(SceneObject * child)
{
	parentScene_->flagObjectsListChanged();
	vector<SceneObjectSPtr>::iterator iter = children_.begin();
	for (; iter != children_.end(); ++iter)
	{
		SceneObjectSPtr curChild = *iter;
		if (curChild.get() == child)
		{
			children_.erase(iter);
			curChild->parent_ = 0;
			curChild->setParentScene(0);
			return curChild;
		}
	}
	return SceneObjectSPtr();
}

void SceneObject::deleteChildren()
{
	parentScene_->flagObjectsListChanged();
	vector<SceneObjectSPtr>::iterator iter = children_.begin();
	for (; iter != children_.end();)
	{
		SceneObjectSPtr curChild = *iter;
		curChild->parent_ = 0;
		curChild->setParentScene(0);					
		iter = children_.erase(iter);		
	}	
}

void SceneObject::setParentScene(Scene * parentScene)
{
	parentScene_ = parentScene;
	BOOST_FOREACH(const SceneObjectSPtr & child, children())
	{
		child->setParentScene(parentScene);		
	}
}

EventListenerSPtr SceneObject::deleteListener(EventListener * listener)
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

EventListenerSPtr SceneObject::findListener(EventListener * listener)
{
	vector<EventListenerSPtr>::iterator iter = eventListeners_.begin();
	for (; iter != eventListeners_.end(); ++iter)
	{
		EventListenerSPtr & curListener = (*iter);
		if (curListener.get() == listener) return curListener;
	}

	return EventListenerSPtr();
}


void SceneObject::setEventListeners(
	const std::vector<EventListenerSPtr> & listeners)
{
	eventListeners_ = listeners;
}

void SceneObject::insertBefore(
	const EventListenerSPtr & newObj, EventListener * refObj)
{
	vector<EventListenerSPtr>::iterator insertIter, removeIter, iter;
	if (newObj.get() == refObj) return;

	removeIter = eventListeners_.end();
	insertIter = eventListeners_.end();	
	for (iter = eventListeners_.begin(); iter != eventListeners_.end(); ++iter)	
	{
		if ((*iter) == newObj) removeIter = iter;	
		if ((*iter).get() == refObj) insertIter = iter;
	}

	if (removeIter == eventListeners_.end()) throw Exception(InvalidArguments);
	if (insertIter == eventListeners_.end() && refObj) throw Exception(InvalidArguments);


	eventListeners_.erase(removeIter);

	insertIter = eventListeners_.end();	
	for (iter = eventListeners_.begin(); iter != eventListeners_.end(); ++iter)	
		if ((*iter).get() == refObj) insertIter = iter;	

	eventListeners_.insert(insertIter, newObj);

}

void SceneObject::setChildren(const std::vector<SceneObjectSPtr> & children)
{
	parentScene_->flagObjectsListChanged();
	BOOST_FOREACH(SceneObjectSPtr & child, children_)
	{
		child->parent_ = 0;
		child->setParentScene(0);		
		child->triggerOnSetTransform();
	}
	children_ = children;
	BOOST_FOREACH(SceneObjectSPtr & child, children_)
	{
		child->parent_ = this;
		child->setParentScene(parentScene_);
		child->triggerOnSetTransform();
	}	
}

bool SceneObject::clickEvent(const Vector2 & mousePos, int pressId)
{
	static ClickEvent event;
	event.target = this;
	event.id = pressId;
	event.clientX = mousePos.x;
	event.clientY = mousePos.y;	

	bool handled = handleEvent(&event, Global::currentTime());
	return handled;
}

bool SceneObject::doubleClickEvent(const Vector2 & mousePos, int pressId)
{
	static DoubleClickEvent event;
	event.target = this;
	event.id = pressId;
	event.clientX = mousePos.x;
	event.clientY = mousePos.y;	

	bool handled = handleEvent(&event, Global::currentTime());
	return handled;
}

bool SceneObject::pressEvent(const Vector2 & startPos, int pressId)
{	
	static PressEvent event;
	event.target = this;
	event.clientX = startPos.x;
	event.clientY = startPos.y;	
	event.id = pressId;

	bool handled = handleEvent(&event, Global::currentTime());
	return handled;
}

bool SceneObject::releaseEvent(const Vector2 & pos, int pressId)
{
	static ReleaseEvent event;
	event.target = this;
	event.clientX = pos.x;
	event.clientY = pos.y;	
	event.id = pressId;
	
	bool handled = handleEvent(&event, Global::currentTime());
	return handled;
}

bool SceneObject::moveEvent(const Vector2 & pos, int pressId)
{	
	static MoveEvent event;
	event.target = this;

	event.clientX = pos.x;
	event.clientY = pos.y;	
	event.id = pressId;
	
	bool handled = handleEvent(&event, Global::currentTime());
	return handled;
}

bool SceneObject::receiveMessageEvent(const std::string & message)
{
	static ReceiveMessageEvent event;
	bool handled = false;
/*
	BOOST_FOREACH(SceneObjectSPtr child, children_)	
		handled |= child->receiveMessageEvent(message);
*/
	event.setMessage(message);	
	
	handled |= handleEvent(&event, Global::currentTime());
		
	return handled;
}

bool SceneObject::handleEvent(Event * event, float sceneTime)
{
	bool handled = false;
	BOOST_FOREACH(EventListenerSPtr & listener, eventListeners_)
	{
		if (parentScene_->isCurrentScene() || !event->onlyForCurrentScene())
			handled |= listener->handle(this, event, sceneTime);
	}
	
	//check that parentScene_ is not null.
	//For example, it is null in the Editor during object creation

	if (parentScene_)
	{
		std::vector<ScriptEventListener *> listeners;		
		listeners.reserve(scriptEventListeners_.size());
		BOOST_FOREACH(ScriptEventListener & listener, scriptEventListeners_) listeners.push_back(&listener);

		ScriptProcessor * s = parentScene_->parentDocument()->scriptProcessor();	
		
		//the scriptEventListeners_ list might change while handling its events. 
		//For example, a script event handler function may register more events 
		//handlers. So use listeners instead for the loop
		BOOST_FOREACH(ScriptEventListener * listener, listeners)
		{
			if (parentScene_->isCurrentScene() || !event->onlyForCurrentScene())
				handled |= listener->handle(event, s);
		}
	}
	return handled;
}

void SceneObject::start(float docTime)
{
	BOOST_FOREACH(SceneObjectSPtr child, children_)
		child->start(docTime);
}

void SceneObject::preStart(float docTime)
{
	BOOST_FOREACH(SceneObjectSPtr child, children_)
		child->preStart(docTime);
}

void SceneObject::stopEventListeners()
{
	BOOST_FOREACH(EventListenerSPtr & listener, eventListeners_)
	{
		listener->stop();
	}

	BOOST_FOREACH(SceneObjectSPtr child, children_)
		child->stopEventListeners();
}

void SceneObject::initAppObjects()
{
	BOOST_FOREACH(SceneObjectSPtr child, children_)
		child->initAppObjects();
}

void SceneObject::uninitAppObjects()
{
	BOOST_FOREACH(SceneObjectSPtr child, children_)
		child->uninitAppObjects();
}


void SceneObject::stop()
{
	BOOST_FOREACH(SceneObjectSPtr child, children_)
		child->stop();

	BOOST_FOREACH(EventListenerSPtr listener, eventListeners_)
	{
		listener->stop();
	}	
}

void SceneObject::appObjectStop()
{
	BOOST_FOREACH(SceneObjectSPtr child, children_)
		child->appObjectStop();
}

bool SceneObject::update(float sceneTime)
{
	bool needToRedraw = false;
	BOOST_FOREACH(SceneObjectSPtr & child, children_)
		needToRedraw |= child->update(sceneTime);

	BOOST_FOREACH(EventListenerSPtr &listener, eventListeners_)
		needToRedraw |= listener->update(this, sceneTime);	

	return needToRedraw;
}

bool SceneObject::asyncLoadUpdate()
{
	bool everythingLoaded = true;
	BOOST_FOREACH(SceneObjectSPtr & child, children_)
		everythingLoaded &= child->asyncLoadUpdate();

	//handleLoadedEventDispatch(everythingLoaded);

	return everythingLoaded;
}

void SceneObject::handleLoadedEventDispatch(bool everythingLoaded)
{
	if (everythingLoaded && !isLoadedDispatched_)
	{
		LoadedEvent loadedEvent;
		loadedEvent.target = this;
		handleEvent(&loadedEvent, Global::currentTime());
		isLoadedDispatched_ = true;		
	}
}

bool SceneObject::isLoaded() const
{
	bool isLoaded = true;
	BOOST_FOREACH(const SceneObjectSPtr & child, children_)
		isLoaded &= child->isLoaded();
	return isLoaded;
}

Matrix SceneObject::parentTransform() const
{
	Matrix ret = Matrix::Identity();
	SceneObject * parentObj = parent();

	while (parentObj)
	{
		VisualAttrib * attr = parentObj->visualAttrib();
		if (attr)
		{
			ret = attr->transformMatrix() * ret;
		}

		parentObj = parentObj->parent();		
	}

	return ret;	
}

float SceneObject::totalOpacity() const
{
	float opacity = 1.0;
	SceneObject * obj = const_cast<SceneObject *>(this);

	while (obj)
	{
		VisualAttrib * attr = obj->visualAttrib();
		if (attr) opacity *= attr->opacity();
		
		obj = obj->parent();		
	}
	return opacity;	
}

bool SceneObject::isThisAndAncestorsVisible() const
{
	bool ret = true;
	SceneObject * curObj = const_cast<SceneObject *>(this);
	while(curObj)
	{
		VisualAttrib * attr = curObj->visualAttrib();
		if (attr)
		{
			ret &= attr->isVisible();
		}
		curObj = curObj->parent();
	}

	return ret;
}

SceneObject * SceneObject::intersect(Vector3 * intPt, const Ray & ray)
{

	bool zBuffer = parentScene_->zBuffer();

	VisualAttrib * attr = visualAttrib();
	if (attr && !attr->isVisible()) return 0;

	SceneObject * intObj = 0;

	if (zBuffer)
	{
		float minDist = FLT_MAX;
		BOOST_FOREACH(SceneObjectSPtr child, children_)
		{
			Vector3 curIntPt;
			SceneObject * curIntObj = child->intersect(&curIntPt, ray);		

			if (curIntObj)
			{
				float dist = (ray.origin - curIntPt).magnitude();
				if (dist <= minDist)
				{
					*intPt = curIntPt;
					intObj = curIntObj;
					minDist = dist;
				}
			}
		}	
	}
	else
	{
		BOOST_FOREACH(SceneObjectSPtr child, children_)
		{
			SceneObject * curIntObj = child->intersect(intPt, ray);		
			if (curIntObj) intObj = curIntObj;
		}
	}
	return intObj;
}

BoundingBox SceneObject::extents() const
{
	BoundingBox ret;
	BOOST_FOREACH(const SceneObjectSPtr & child, children_)
	{
		VisualAttrib * attr = child->visualAttrib();
		if (!attr) continue;
		const Matrix & matrix = attr->transform().computeMatrix();
		ret = ret.unite(child->extents().transform(matrix));
	}
	return ret;
}

BoundingBox SceneObject::worldExtents() const
{
	Vector3 corners[8];	
	BoundingBox bbox = extents();
	Vector3 minPt = bbox.minPt;
	Vector3 maxPt = bbox.maxPt;

	corners[0] = Vector3(minPt.x, minPt.y, minPt.z);
	corners[1] = Vector3(minPt.x, maxPt.y, minPt.z);
	corners[2] = Vector3(maxPt.x, maxPt.y, minPt.z);
	corners[3] = Vector3(maxPt.x, minPt.y, minPt.z);
	corners[4] = Vector3(minPt.x, minPt.y, maxPt.z);
	corners[5] = Vector3(minPt.x, maxPt.y, maxPt.z);
	corners[6] = Vector3(maxPt.x, maxPt.y, maxPt.z);
	corners[7] = Vector3(maxPt.x, minPt.y, maxPt.z);

	Matrix totalTransform = parentTransform() * transform().computeMatrix();

	maxPt = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	minPt = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);

	for (int i = 0; i < 8; ++i)
	{
		corners[i] = totalTransform * corners[i];
		if (corners[i].x < minPt.x) minPt.x = corners[i].x;
		if (corners[i].y < minPt.y) minPt.y = corners[i].y;
		if (corners[i].z < minPt.z) minPt.z = corners[i].z;

		if (corners[i].x > maxPt.x) maxPt.x = corners[i].x;
		if (corners[i].y > maxPt.y) maxPt.y = corners[i].y;
		if (corners[i].z > maxPt.z) maxPt.z = corners[i].z;
	}

	return BoundingBox(minPt, maxPt);
}

void SceneObject::addTempRenderObject(GfxRenderer * gl, TempRenderObject * tempRenderObject)
{
	tempRenderObject->init(gl);
	tempRenderObjects_.push_back(tempRenderObject);
}

bool SceneObject::dependsOn(Scene * scene) const
{
	BOOST_FOREACH(SceneObjectSPtr object, children_)
	{
		if (object->dependsOn(scene)) return true;
	}
	return false;
}

bool SceneObject::isShowing(Scene * scene) const
{	
	BOOST_FOREACH(SceneObjectSPtr object, children_)
	{
		if (object->isShowing(scene)) return true;
	}
	return false;
}

void SceneObject::dependsOnScenes(std::vector<Scene *> * dependsOnScenes, bool recursive) const
{
	BOOST_FOREACH(SceneObjectSPtr object, children_)
		object->dependsOnScenes(dependsOnScenes, recursive);
}

void SceneObject::showingScenes(std::set<Scene *> * showingScenes) const
{
	BOOST_FOREACH(SceneObjectSPtr object, children_)
		object->showingScenes(showingScenes);
}

bool SceneObject::addScriptEventListener(const ScriptEventListener & handler)
{
	scriptEventListeners_.push_back(handler);
	return true;
}

bool SceneObject::removeScriptEventListener(const ScriptEventListener & handler)
{
	bool removed = false;
	vector<ScriptEventListener>::iterator iter;
	for (iter = scriptEventListeners_.begin(); iter != scriptEventListeners_.end();)
	{
		ScriptEventListener & cur = *iter;
		if (cur == handler)
		{
			iter = scriptEventListeners_.erase(iter);
			removed = true;
		}
		else ++iter;
	}

	return removed;
}

unsigned char SceneObject::version() const
{
	return 1;
}

void SceneObject::write(Writer & writer) const
{
	writer.write(stringId_, "stringId");
	writer.write(children_, "children");
	writer.write(eventListeners_, "eventListeners");
}

void SceneObject::writeXml(XmlWriter & w) const
{
	w.writeTag("StringId", stringId_);
	w.startTag("EventListeners");
	BOOST_FOREACH(EventListenerSPtr listener, eventListeners_)
	{
		w.startTag("EventListener");
		listener->writeXml(w);
		w.endTag();
	}
	w.endTag();
}

void SceneObject::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(stringId_, curNode, "StringId"));
		else if (r.isNodeName(curNode, "EventListeners"))
		{
			for(xmlNode * listenerNode = curNode->children; listenerNode; listenerNode = listenerNode->next)
			{		
				if (listenerNode->type != XML_ELEMENT_NODE) continue;	
				if (r.isNodeName(listenerNode, "EventListener"))
				{
					EventListenerSPtr listener(new EventListener);		
					listener->readXml(r, listenerNode);
					eventListeners_.push_back(listener);
					listenerNode = listenerNode->next;
				}
			}
		}		
	}
}

void SceneObject::read(Reader & reader, unsigned char version) 
{
	if (version < 1) 
	{
		unsigned int internalId;
		reader.read(internalId);
	}
	reader.read(stringId_);
	reader.read(children_);
	BOOST_FOREACH(SceneObjectSPtr child, children_)
	{
		//can't use setParentScene because it's recursive nature can conflict
		//with the Reader pointer reading mechanism...
		//It's complicated, many times there doesn't seem to be a problem...
		//but in a select few cases there is a crash...
		child->parent_ = this;
		//parentScene_ will be set later in Scene::read
	}
	reader.read(eventListeners_);
}

void SceneObject::getSceneObjectsByID(
	const boost::uuids::uuid & type, std::vector<SceneObject *> * objs) const
{	
	BOOST_FOREACH(const SceneObjectSPtr & child, children_)
	{
		if (child->type() == type)
			objs->push_back(child.get());

		child->getSceneObjectsByID(type, objs);
	}
}

const char * SceneObject::xmlTag() const
{
	return "";
}
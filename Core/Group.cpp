#include "stdafx.h"
#include "Group.h"
#include "Xml.h"
#include "Writer.h"
#include "Reader.h"
#include "ScriptProcessor.h"
#include "Scene.h"
#include "Global.h"
#include "Document.h"

using namespace std;

JSClass jsGroupClass = InitClass(
	"Group", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);

/**

*/
SceneObjectSPtr popSceneObjectSPtr(ScriptProcessor * s, SceneObject * obj)
{
	if (!obj->parent())
		return s->removeKeepAliveData(obj);
	else
		return obj->parentScene()->findObject(obj->parent(), obj);
}

void pushSceneObjectSPtr(ScriptProcessor * s, SceneObjectSPtr & objSPtr)
{
	s->addKeepAliveData(objSPtr);
}


JSBool Group_getSceneObjectById(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	const jschar * id;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &id)) return JS_FALSE;

	string idStr = jsCharStrToStdString(id);
	Group * grp = 0;
	SceneObject * sceneObj = 0;
	grp = (Group *)JS_GetPrivate(cx, obj);
	//TODO make it so that this works even if parentScene is NULL.
	if (grp && grp->parentScene())
	{
		sceneObj = grp->parentScene()->findObject(grp, idStr).get();
	}

	if (!sceneObj)
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	else
	{
		//create a JSObject based on the scene object
		JSObject * jsObject = sceneObj->getScriptObject(s);
	
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsObject));
	}

	return JS_TRUE;
}

JSBool Group_appendChild(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Group * thisObj = (Group *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject *jsNewChild;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsNewChild)) 
		return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (!jsNewChild)
	{
		JS_ReportError(cx, "Group::appendChild - parameter is null");
		return JS_FALSE;
	}
	else if (!isSceneObject(s, jsNewChild))
	{
		JS_ReportError(cx, "Group::appendChild - parameter must be a SceneObject");
		return JS_FALSE;
	}

	SceneObject * newChild;
	newChild = (SceneObject *)JS_GetPrivate(cx, jsNewChild);

	SceneObjectSPtr newChildSPtr = popSceneObjectSPtr(s, newChild);
	thisObj->addChild(newChildSPtr);

	return JS_TRUE;
};

JSBool Group_removeChild(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Group * thisObj = (Group *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject *jsChild;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsChild)) 
		return JS_FALSE;	

	if (!isSceneObject(s, jsChild))
	{
		JS_ReportError(cx, "Group::appendChild - parameter must be a SceneObject");
		return JS_FALSE;
	}

	SceneObject * child;
	child = (SceneObject *)JS_GetPrivate(cx, jsChild);
	SceneObjectSPtr childSPtr = thisObj->removeChild(child);

	if (childSPtr)
	{		
		pushSceneObjectSPtr(s, childSPtr);
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(childSPtr->getScriptObject(s)));
	}
	else JS_SET_RVAL(cx, vp, JSVAL_NULL);
	
	return JS_TRUE;
}

JSBool Group_insertAfter(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Group * thisObj = (Group *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsRefChild, *jsNewChild;
	if (!JS_ConvertArguments(cx, argc, argv, "oo", &jsNewChild, &jsRefChild)) 
		return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (!isSceneObject(s, jsRefChild))
	{
		JS_ReportError(cx, "Group::insertAfter - parameters must be SceneObjects");
		return JS_FALSE;
	}

	if (!isSceneObject(s, jsNewChild))
	{
		JS_ReportError(cx, "Group::insertAfter - parameters must be SceneObjects");
		return JS_FALSE;
	}

	SceneObject * refChild, * newChild;
	refChild = (SceneObject *)JS_GetPrivate(cx, jsRefChild);
	newChild = (SceneObject *)JS_GetPrivate(cx, jsNewChild);

	SceneObjectSPtr newChildSPtr = popSceneObjectSPtr(s, newChild);

	thisObj->insertAfter(newChildSPtr, refChild);
	return JS_TRUE;
};

JSBool Group_getChildren(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Group * thisObj = (Group *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	const vector<SceneObjectSPtr> & children = thisObj->children();

	JSObject * jsChildren = JS_NewArrayObject(cx, (jsint)children.size(), 0);

	for (int i = 0; i < (int)children.size(); ++i)
	{
		jsval val;
		val = OBJECT_TO_JSVAL(children[i]->getScriptObject(s));
		JS_SetElement(cx, jsChildren, (jsint)i, &val);
	}

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsChildren));	

	return JS_TRUE;
};

JSBool Group_createSceneObject(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Group * grp = (Group *)JS_GetPrivate(cx, obj);
	
	const jschar * jsStr;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &jsStr)) return JS_FALSE;
	std::string typeStr = jsCharStrToStdString(jsStr);

	SceneObject * newObj = Global::instance().createSceneObject(typeStr);

	if (newObj) 
	{
		grp->addChild(SceneObjectSPtr(newObj));
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(newObj->getScriptObject(s)));
	}
	else
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	
	return JS_TRUE;
}

JSBool Group_showAll(JSContext *cx, uintN argc, jsval *vp)
{
	Group * thisObj = (Group *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	const vector<SceneObjectSPtr> & children = thisObj->children();

	for (int i = 0; i < (int)children.size(); ++i)	
		children[i]->visualAttrib()->setVisible(true);	

	s->document()->triggerRedraw();

	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	return JS_TRUE;
};

JSBool Group_hideAll(JSContext *cx, uintN argc, jsval *vp)
{
	Group * thisObj = (Group *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	const vector<SceneObjectSPtr> & children = thisObj->children();

	for (int i = 0; i < (int)children.size(); ++i)	
		children[i]->visualAttrib()->setVisible(false);	

	s->document()->triggerRedraw();

	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	return JS_TRUE;
};


JSFunctionSpec GroupFuncs[] = {
	JS_FS("appendChild", Group_appendChild, 1, 0),
	JS_FS("insertAfter", Group_insertAfter, 2, 0),
	JS_FS("getChildren", Group_getChildren, 0, 0),
	JS_FS("removeChild", Group_removeChild, 1, 0),
	JS_FS("createSceneObject", Group_createSceneObject, 1, 0),
	JS_FS("showAll", Group_showAll, 0, 0),
	JS_FS("hideAll", Group_hideAll, 0, 0),

	JS_FS("getSceneObjectById", Group_getSceneObjectById, 1, 0),

    JS_FS_END
};

///////////////////////////////////////////////////////////////////////////////

ParentObjectVisualAttrib::ParentObjectVisualAttrib(SceneObject * sceneObject)
{
	sceneObject_ = sceneObject;
}

ParentObjectVisualAttrib & ParentObjectVisualAttrib::operator = (const ParentObjectVisualAttrib & rhs)
{
	*(VisualAttrib *)this = *(VisualAttrib *)&rhs;
	return *this;
}

ParentObjectVisualAttrib & ParentObjectVisualAttrib::operator = (const VisualAttrib & rhs)
{
	*(VisualAttrib *)this = rhs;
	return *this;
}

void ParentObjectVisualAttrib::onSetTransform()
{
	BOOST_FOREACH(const SceneObjectSPtr & child, sceneObject_->children())
	{
		triggerOnSetTransform(child.get());		
	}
}

void ParentObjectVisualAttrib::write(Writer & writer) const
{
	writer.writeParent<VisualAttrib>(this);
}

void ParentObjectVisualAttrib::read(Reader & reader, unsigned char version)
{
	reader.readParent<VisualAttrib>(this);
}

///////////////////////////////////////////////////////////////////////////////

JSObject * Group::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsGroupClass,
		0, 0, 0, GroupFuncs, 0, 0);

	return proto;
}

JSObject * Group::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsGroupClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}


Group::Group() : visualAttrib_(this)
{
	setId("Group");
}

Group::Group(const Group & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping), visualAttrib_(this)
{
	visualAttrib_ = rhs.visualAttrib_;
}

void Group::drawObject(GfxRenderer * gl) const
{		
}



BoundingBox Group::extents() const
{
	return SceneObject::extents();
}

void Group::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_);
}

void Group::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();

	w.startTag("Children");
	for (unsigned i = 0; i < (unsigned)children_.size(); ++i)
	{
		w.writeObject(children_[i].get());
	}
	w.endTag();
}

void Group::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	if (version < 1)
	{
		VisualAttrib visualAttrib;
		reader.read(visualAttrib);
		visualAttrib_ = visualAttrib;
	}
	else
	{
		reader.read(visualAttrib_);
	}
}

void Group::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;	
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
		}
		else if (r.isNodeName(curNode, "Children"))
		{
			readXmlChildren(r, curNode);
		}	
	}
}

void Group::readXmlChildren(XmlReader & r, xmlNode * parent)
{	
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{				
		if (curNode->type != XML_ELEMENT_NODE) continue;
		SceneObjectSPtr s;
		if (r.getSceneObject(s, curNode)) children_.push_back(s);
	}
}

void Group::requestLocalSaveObject()
{
	BOOST_FOREACH(const SceneObjectSPtr & obj, children_)
	{
		obj->requestLocalSaveObject();
	}
}
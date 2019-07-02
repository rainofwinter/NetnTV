#include "stdafx.h"
#include "Rectangle.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ScriptProcessor.h"
#include "Document.h"

using namespace std;
namespace Studio
{

JSClass jsRectangleClass = InitClass(
	"Rectangle", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);

JSBool Rectangle_setWidth(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);
	Rectangle * thisObj = (Rectangle *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsDouble;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsDouble)) return JS_FALSE;
	thisObj->setWidth((int)jsDouble);
	s->document()->triggerRedraw();
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Rectangle_setHeight(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);
	Rectangle * thisObj = (Rectangle *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsDouble;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsDouble)) return JS_FALSE;
	thisObj->setHeight((int)jsDouble);
	s->document()->triggerRedraw();
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Rectangle_getWidth(JSContext *cx, uintN argc, jsval *vp)
{	
	Rectangle * thisObj = (Rectangle *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL((jsdouble)thisObj->width()));
	return JS_TRUE;
};

JSBool Rectangle_getHeight(JSContext *cx, uintN argc, jsval *vp)
{
	Rectangle * thisObj = (Rectangle *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL((jsdouble)thisObj->height()));
	return JS_TRUE;
};

JSBool Rectangle_setColor(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);
	Rectangle * thisObj = (Rectangle *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	
	Color color = Color::fromScriptObject(s, jsObj);
	thisObj->setColor(color);	
	s->document()->triggerRedraw();
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSFunctionSpec RectangleFuncs[] = {
	JS_FS("setWidth", Rectangle_setWidth, 1, 0),
	JS_FS("setHeight", Rectangle_setHeight, 1, 0),

	JS_FS("getWidth", Rectangle_getWidth, 0, 0),
	JS_FS("getHeight", Rectangle_getHeight, 0, 0),

	JS_FS("setColor", Rectangle_setColor, 1, 0),
    JS_FS_END
};


///////////////////////////////////////////////////////////////////////////////



JSObject * Rectangle::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsRectangleClass,
		0, 0, 0, RectangleFuncs, 0, 0);

	return proto;
}

JSObject * Rectangle::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsRectangleClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void Rectangle::create()
{
	width_ = 100;
	height_ = 100;	
}

Rectangle::Rectangle()
{
	create();
	setId("Rectangle");
}

Rectangle::Rectangle(const Rectangle & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();
	visualAttrib_ = rhs.visualAttrib_;
	color_ = rhs.color_;
	height_ = rhs.height_;
	width_ = rhs.width_;
}


Rectangle::~Rectangle()
{
	uninit();
}


void Rectangle::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
}

void Rectangle::uninit()
{
	SceneObject::uninit();
}
void Rectangle::drawObject(GfxRenderer * gl) const
{
	gl->useColorProgram();
	gl->setColorProgramColor(color_.r, color_.g, color_.b, totalOpacity());
	
	gl->drawRect(0, 0, width_, height_);
}

BoundingBox Rectangle::extents() const
{
	return BoundingBox(
		Vector3(0, 0, 0), 
		Vector3(width_, height_, 0));
}

SceneObject * Rectangle::intersect(Vector3 * out, const Ray & ray)
{
	if (!visualAttrib_.isVisible()) return 0;
	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();

	Vector3 a = trans * Vector3(0, 0, 0);
	Vector3 b = trans * Vector3(0, height_, 0);
	Vector3 c = trans * Vector3(width_, height_, 0);
	Vector3 d = trans * Vector3(width_, 0, 0);

	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return this;
	}
	else return 0;

}

void Rectangle::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");	
	writer.write(width_);	
	writer.write(height_);
	writer.write(color_);	
}

void Rectangle::writeXml(XmlWriter & w) const
{	
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");	
	visualAttrib_.writeXml(w);
	w.endTag();

	w.writeTag("Width", width_);
	w.writeTag("Height", height_);
	w.writeTag("Color", color_);
}

void Rectangle::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);
	reader.read(width_);	
	reader.read(height_);
	reader.read(color_);
}

void Rectangle::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;		
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(width_, curNode, "Width"));
		else if (r.getNodeContentIfName(height_, curNode, "Height"));
		else if (r.getNodeContentIfName(color_, curNode, "Color"));
	}
}
}
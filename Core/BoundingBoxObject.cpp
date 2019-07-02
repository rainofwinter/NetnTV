#include "stdafx.h"

#include "BoundingBoxObject.h"
#include "ScriptProcessor.h"

#include "Writer.h"
#include "Reader.h"
#include "Xml.h"

using namespace std;

JSClass jsBoundingBoxClass = InitClass(
	"BoundingBox", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);


JSBool BoundingBoxObject_intersectObject(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	BoundingBoxObject * ThisBbo = (BoundingBoxObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	

	if (!jsObj || !isSceneObject(s, jsObj))
	{
		JS_ReportError(cx, "BoundingBox::intersectObject - parameter must be a SceneObject");
		return JS_FALSE;
	}
	SceneObject * obj = (SceneObject *)JS_GetPrivate(cx, jsObj);
	boost::uuids::uuid type = obj->type();
	if (type != BoundingBoxObject().type())
	{
		JS_ReportError(cx, "BoundingBox::intersectObject - parameter must be a BoundingBoxObject");
		return JS_FALSE;
	}
	BoundingBoxObject * bbo = (BoundingBoxObject *)obj;
	
	bool ret = ThisBbo->intersect(bbo);

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(ret));
	return JS_TRUE;
};

JSFunctionSpec BoundingBoxFuncs[] = {
	JS_FS("intersectObject", BoundingBoxObject_intersectObject, 1, 0),
	JS_FS_END
};


///////////////////////////////////////////////////////////////////////////////
JSObject * BoundingBoxObject::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsBoundingBoxClass,
		0, 0, 0, BoundingBoxFuncs, 0, 0);

	return proto;
}

JSObject * BoundingBoxObject::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsBoundingBoxClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void BoundingBoxObject::create()
{
}

BoundingBoxObject::BoundingBoxObject()
{
	create();
	setId("BoundingBox");
}

BoundingBoxObject::BoundingBoxObject(const BoundingBoxObject & rhs, ElementMapping * elementMapping)
	: SceneObject(rhs, elementMapping)
{
	create();
	visualAttrib_ = rhs.visualAttrib_;
}


BoundingBoxObject::~BoundingBoxObject()
{
}


void BoundingBoxObject::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
}

void BoundingBoxObject::uninit()
{
	SceneObject::uninit();
}


bool BoundingBoxObject::update(float sceneTime)
{	
	bool needRedraw = SceneObject::update(sceneTime);
	return needRedraw;
}


bool BoundingBoxObject::asyncLoadUpdate()
{
	return SceneObject::asyncLoadUpdate();
}

bool BoundingBoxObject::isLoaded() const
{
	return SceneObject::isLoaded();
}

void BoundingBoxObject::drawObject(GfxRenderer * gl) const
{
	SceneObject::drawObject(gl);
}


BoundingBox BoundingBoxObject::extents() const
{	
	return BoundingBox(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
}

void BoundingBoxObject::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");
}

void BoundingBoxObject::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);	
}

void BoundingBoxObject::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();
}

void BoundingBoxObject::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);
	std::string filename;

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{
		if (curNode->type != XML_ELEMENT_NODE) continue;		
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
	}
}

OBoundingBox BoundingBoxObject::toObb() const
{
	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();
	OBoundingBox ret;
	ret.origin = trans * Vector3(0.0f, 0.0f, 0.0f);

	ret.xAxis = trans * Vector3(1.0f, 0.0f, 0.0f) - ret.origin;
	ret.yAxis = trans * Vector3(0.0f, 1.0f, 0.0f) - ret.origin;
	ret.zAxis = trans * Vector3(0.0f, 0.0f, 1.0f) - ret.origin;

	return ret;
}

bool BoundingBoxObject::intersect(BoundingBoxObject * bbo) const
{
	return toObb().intersect(bbo->toObb());
}

SceneObject * BoundingBoxObject::intersect(Vector3 * out, const Ray & ray)
{
	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();
	Matrix inverse = trans.inverse();

	Vector3 a = ray.origin;
	Vector3 b = ray.origin + ray.dir;

	Ray invRay;
	invRay.origin = inverse * a;
	invRay.dir = inverse * b - invRay.origin;
	
	BoundingBox bBox;
	bBox.minPt.x = 0.0f;
	bBox.minPt.y = 0.0f;
	bBox.minPt.z = 0.0f;

	bBox.maxPt.x = 1.0f;
	bBox.maxPt.x = 1.0f;
	bBox.maxPt.x = 1.0f;

	float t;
	bool inter = bBox.intersect(&t, invRay);
	if (inter)
	{
		Vector3 pt = invRay.origin + t * invRay.dir;
		*out = trans * pt;
		return this;		
	}
	if (!inter) return NULL;
}

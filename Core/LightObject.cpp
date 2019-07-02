#include "stdafx.h"
#include "LightObject.h"
#include "ScriptProcessor.h"

#include "GfxRenderer.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "Global.h"

using namespace std;

JSClass jsLightClass = InitClass(
	"Light", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);

JSBool Light_setLightDirection(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	LightObject * thisObj = (LightObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsVec;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsVec)) return JS_FALSE;
	thisObj->setLightDirection(Vector3::fromScriptObject(s, jsVec));
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Light_getLightDirection(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	LightObject * thisObj = (LightObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	const Vector3 & lightDirection = thisObj->lightDirection();
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(lightDirection.createScriptObject(s)));
	return JS_TRUE;
}

JSFunctionSpec LightFuncs[] = {
	JS_FS("setLightDirection", Light_setLightDirection, 1, 0),
	JS_FS("getLightDirection", Light_getLightDirection, 0, 0),
    JS_FS_END
};


///////////////////////////////////////////////////////////////////////////////

LightVisualAttrib::LightVisualAttrib(LightObject * parentObject) 
{
	lightObject_ = parentObject;
}

void LightVisualAttrib::onSetTransform()
{
	VisualAttrib::onSetTransform();
	Matrix transform = lightObject_->parentTransform() * transformMatrix();

	Vector3 start(0.0f, 0.0f, 0.0f);
	Vector3 end(0.0f, 0.0f, 1.0f);
	
	
	Vector3 newStart = transform * start;		
	Vector3 newEnd = transform * end;

	lightObject_->lightDirection_ = (newEnd - newStart).normalize();

	
}

void LightVisualAttrib::onGetTransform() const
{
	if (lightObject_->syncTransformNeeded_)
	{
		lightObject_->setTransformDirect(lightObject_->syncLightTransform(transform_));
	}
}

void LightVisualAttrib::write(Writer & writer) const
{
	writer.writeParent<VisualAttrib>(this);
}

void LightVisualAttrib::read(Reader & reader, unsigned char version)
{
	reader.readParent<VisualAttrib>(this);
}

///////////////////////////////////////////////////////////////////////////////
JSObject * LightObject::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsLightClass,
		0, 0, 0, LightFuncs, 0, 0);

	return proto;
}

JSObject * LightObject::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsLightClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void LightObject::create()
{
	lightDirection_ = Vector3(0.0f, 0.0f, 1.0f);
	syncTransformNeeded_ = false;
	intensity_ = 1.0f;
	specularIntensity_ = 1.0f;
}

LightObject::LightObject() : visualAttrib_(this)
{
	create();
	setId("Light");
}

LightObject::LightObject(const LightObject & rhs, ElementMapping * elementMapping)
: visualAttrib_(this), SceneObject(rhs, elementMapping)
{
	create();
	visualAttrib_ = rhs.visualAttrib_;
	visualAttrib_.lightObject_ = this;

	lightDirection_ = rhs.lightDirection_;
	syncTransformNeeded_ = rhs.syncTransformNeeded_;
	intensity_ = rhs.intensity_;
}


LightObject::~LightObject()
{
}


void LightObject::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
}

void LightObject::uninit()
{
	SceneObject::uninit();
}


bool LightObject::update(float sceneTime)
{	
	bool needRedraw = SceneObject::update(sceneTime);
	return needRedraw;
}

void LightObject::setLightDirection(const Vector3 & lightDirection)
{
	lightDirection_ = lightDirection.normalize();
	setSyncTransformNeeded(true);
}


bool LightObject::asyncLoadUpdate()
{
	return SceneObject::asyncLoadUpdate();
}

bool LightObject::isLoaded() const
{
	return SceneObject::isLoaded();
}

void LightObject::drawObject(GfxRenderer * gl) const
{
	SceneObject::drawObject(gl);
}


BoundingBox LightObject::extents() const
{
	const float cEpsilon = 0.001f;
	return BoundingBox(Vector3(-cEpsilon, -cEpsilon, -cEpsilon), Vector3(cEpsilon, cEpsilon, cEpsilon));
}

Transform LightObject::syncLightTransform(const Transform & curTransform) const
{
	Matrix M = Matrix::Rotate(Vector3(0.0f, 0.0f, 1.0f), lightDirection_);

	const Vector3 & pos = curTransform.translation();
	M = Matrix::Translate(pos.x, pos.y, pos.z) * M;

	Transform T(M);

	const Vector3 & curPivot = curTransform.pivot();
	Vector3 curScaling = curTransform.scaling();
	Vector3 tScaling = T.scaling();

	float scaleSignX = (tScaling.x > 0.0f)?1.0f:-1.0f;
	float scaleSignY = (tScaling.y > 0.0f)?1.0f:-1.0f;
	float scaleSignZ = (tScaling.z > 0.0f)?1.0f:-1.0f;

	Vector3 newScaling = Vector3(fabs(curScaling.x), fabs(curScaling.y), fabs(curScaling.z));
	newScaling.x *= scaleSignX;
	newScaling.y *= scaleSignY;
	newScaling.z *= scaleSignZ;

	T.setScaling(newScaling);
	T.setPivot(curPivot);

	return T;
}

void LightObject::setTransformDirect(const Transform & transform)
{
	visualAttrib_.setTransformDirect(transform);
}

void LightObject::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");	
	writer.write(lightDirection_, "lightDirection");
	writer.write(intensity_, "intensity");
	writer.write(specularIntensity_, "specular intensity");
}

void LightObject::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);
	reader.read(lightDirection_);	
	reader.read(intensity_);
	if (version >= 4)
		reader.read(specularIntensity_);
}


void LightObject::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();

	w.writeTag("LightDirection", lightDirection_);
	w.writeTag("Intensity", intensity_);
	w.writeTag("SpecularIntensity", specularIntensity_);
}

void LightObject::readXml(XmlReader & r, xmlNode * parent)
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
		else if (r.getNodeContentIfName(lightDirection_, curNode, "LightDirection"));
		else if (r.getNodeContentIfName(intensity_, curNode, "Intensity"));
		else if (r.getNodeContentIfName(specularIntensity_, curNode, "Specular Intensity"));
	}
}


SceneObject * LightObject::intersect(Vector3 * out, const Ray & ray)
{
	return SceneObject::intersect(out, ray);
}


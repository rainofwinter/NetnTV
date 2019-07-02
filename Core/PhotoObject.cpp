#include "StdAfx.h"
#include "PhotoObject.h"
#include "Writer.h"
#include "Reader.h"
#include "SceneObject.h"
#include "Global.h"
#include "ScriptProcessor.h"
#include "Texture.h"
#include "Document.h"
#include "PhotoSaveEvent.h"
#include "PhotoTakePictureEvent.h"
#include "PhotoCameraOnEvent.h"
#include "PhotoCameraOffEvent.h"
#include "Scene.h"

using namespace std;

extern JSClass jsPixelBufferClass;

JSClass jsPhotoObjectClass = InitClass(
	"PhotoObject", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);

JSBool PhotoObject_setWidth(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	PhotoObject * thisObj = (PhotoObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsDouble;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsDouble)) return JS_FALSE;
	thisObj->setWidth((int)jsDouble);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool PhotoObject_setHeight(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	PhotoObject * thisObj = (PhotoObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsDouble;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsDouble)) return JS_FALSE;
	thisObj->setHeight((int)jsDouble);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool PhotoObject_getWidth(JSContext *cx, uintN argc, jsval *vp)
{
	PhotoObject * thisObj = (PhotoObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(thisObj->width()));
	return JS_TRUE;
}

JSBool PhotoObject_getHeight(JSContext *cx, uintN argc, jsval *vp)
{
	PhotoObject * thisObj = (PhotoObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(thisObj->height()));
	return JS_TRUE;
}

JSBool PhotoObject_getPhotoName(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	PhotoObject * thisObj = (PhotoObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "")) return JS_FALSE;
	
	const char * idStr = thisObj->getPhotoName().c_str();
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, idStr)));
	return JS_TRUE;
}

JSBool PhotoObject_getX(JSContext *cx, uintN argc, jsval *vp)
{
	PhotoObject * thisObj = (PhotoObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(thisObj->x()));
	return JS_TRUE;
}

JSBool PhotoObject_getY(JSContext *cx, uintN argc, jsval *vp)
{
	PhotoObject * thisObj = (PhotoObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(thisObj->y()));
	return JS_TRUE;
}

JSBool PhotoObject_setX(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	PhotoObject * thisObj = (PhotoObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsDouble;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsDouble)) return JS_FALSE;
	thisObj->setX((int)jsDouble);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool PhotoObject_setY(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	PhotoObject * thisObj = (PhotoObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsDouble;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsDouble)) return JS_FALSE;
	thisObj->setY((int)jsDouble);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSFunctionSpec PhotoObjectFuncs[] = {
	JS_FS("setWidth", PhotoObject_setWidth, 1, 0),
	JS_FS("setHeight", PhotoObject_setHeight, 1, 0),

	JS_FS("getWidth", PhotoObject_getWidth, 0, 0),
	JS_FS("getHeight", PhotoObject_getHeight, 0, 0),
	
	JS_FS("setX", PhotoObject_setX, 1, 0),
	JS_FS("setY", PhotoObject_setY, 1, 0),

	JS_FS("getX", PhotoObject_getX, 0, 0),
	JS_FS("getY", PhotoObject_getY, 0, 0),

	JS_FS("getPhotoName", PhotoObject_getPhotoName, 0, 0),

    JS_FS_END
};

///////////////////////////////////////////////////////////////////////////////
JSObject * PhotoObject::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsPhotoObjectClass,
		0, 0, 0, PhotoObjectFuncs, 0, 0);

	return proto;
}

JSObject * PhotoObject::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsPhotoObjectClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void PhotoObject::create()
{
	width_ = 400;
	height_ = 300;
	trans_ = Vector2(width_, height_);
	format_ = Texture::UncompressedRgba32;
	x_ = 0;
	y_ = 0;
}

PhotoObject::PhotoObject()
{
	create();
	setId("PhotoObject");
}

PhotoObject::PhotoObject(const PhotoObject & rhs, ElementMapping * elementMapping)
: Image(rhs, elementMapping)
{
	create();
	fileName_ = rhs.fileName_;
	height_ = rhs.height_;
	width_ = rhs.width_;
	x_ = rhs.x_;
	y_ = rhs.y_;
	visualAttrib_ = rhs.visualAttrib_;
}

PhotoObject::~PhotoObject()
{
}

void PhotoObject::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);

	writer.write(width_, "width");
	writer.write(height_, "height");
	writer.write(x_, "x");
	writer.write(y_, "y");
	writer.write(fileName_, "fileName");
}

void PhotoObject::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(width_);
	reader.read(height_);
	Transform t = visualAttrib_.transform();
	reader.read(x_);
	reader.read(y_);

	t.setTranslation(x_, y_, 0.0f);
	visualAttrib_.setTransform(t);

	
	reader.read(fileName_);
	Image::setFileName(fileName_);
	Image::setWidth(width_);
	Image::setHeight(height_);
}

BoundingBox PhotoObject::extents() const
{
	return BoundingBox(
		Vector3(0, 0, 0), 
		Vector3(width(), height(), 0));
}

void PhotoObject::referencedFiles(std::vector<std::string> * refFiles) const
{
	SceneObject::referencedFiles(refFiles);
	refFiles->push_back(fileName_);
}

int PhotoObject::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);
	fileName_ = refFiles[index++];
	return index;
}

bool PhotoObject::intersectRect(Vector3 * out, const Ray & ray)
{
	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();

	Vector3 a = trans * Vector3(0, 0, 0);
	Vector3 b = trans * Vector3(0, height(), 0);
	Vector3 c = trans * Vector3(width(), height(), 0);
	Vector3 d = trans * Vector3(width(), 0, 0);
	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return true;
	}
	else return false;
}


SceneObject * PhotoObject::intersect(Vector3 * intPt, const Ray & ray)
{
	if (!visualAttrib_.isVisible()) return 0;
	if (intersectRect(intPt, ray)) return this;
	else return 0;
}

void PhotoObject::setFileName(const std::string & fileName)
{
		fileName_ = fileName;
		Image::setFileName(fileName_);	
}

void PhotoObject::setPhotoName(const std::string & fileName)
{
	Image::setFileName(fileName);
	Image::setWidth(width_);
	Image::setHeight(height_);
	init(Global::instance().curDocument()->renderer());
	Global::instance().curDocument()->triggerRedraw();
	Global::instance().curDocument()->photoCaptureComplete();
	if(!visualAttrib_.isVisible())
	{
		visualAttrib_.setVisible(true);
	}
	
}

void PhotoObject::setWidth(const float & width)
{
	Image::setWidth(width);
	trans_.x = width;
	width_ = Image::width();
}

void PhotoObject::setHeight(const float & height)
{
	Image::setHeight(height);
	trans_.y = height;
	height_ = Image::height();
}

void PhotoObject::photoDefaultImage()
{
	if(!visualAttrib_.isVisible())
	{
		visualAttrib_.setVisible(true);
	}
	if(fileName_ != "")
	{
		Image::setFileName(fileName_);
		Image::setWidth(width_);
		Image::setHeight(height_);
		init(Global::instance().curDocument()->renderer());
	}
}

void PhotoObject::photoSave()
{
	PhotoSaveEvent event;
	event.target = this;
	if (handleEvent(&event, Global::currentTime()))
		parentScene_->parentDocument()->triggerRedraw();
}

void PhotoObject::photoTakePicture()
{
	PhotoTakePictureEvent event;
	event.target = this;
	if (handleEvent(&event, Global::currentTime()))
		parentScene_->parentDocument()->triggerRedraw();
}

void PhotoObject::photoCameraOn()
{
	PhotoCameraOnEvent event;
	event.target = this;
	if (handleEvent(&event, Global::currentTime()))
		parentScene_->parentDocument()->triggerRedraw();
}

void PhotoObject::photoCameraOff()
{
	PhotoCameraOffEvent event;
	event.target = this;
	if (handleEvent(&event, Global::currentTime()))
		parentScene_->parentDocument()->triggerRedraw();
}

void PhotoObject::setWidHei(const Vector2 & trans)
{
	Image::setWidth(trans.x);
	Image::setHeight(trans.y);
	width_ = Image::width();
	height_ = Image::height();
}

#include "stdafx.h"
#include "CameraObject.h"
#include "ScriptProcessor.h"

#include "GfxRenderer.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "Global.h"
#include "Camera.h"

using namespace std;

JSClass jsCameraClass = InitClass(
	"Camera", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);

JSBool Camera_setEye(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	CameraObject * thisObj = (CameraObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsVec;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsVec)) return JS_FALSE;
	thisObj->camera()->setEye(Vector3::fromScriptObject(s, jsVec));	
	thisObj->setSyncTransformNeeded(true);	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Camera_getEye(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	CameraObject * thisObj = (CameraObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	const Vector3 & eye = thisObj->camera()->eye();
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(eye.createScriptObject(s)));

	return JS_TRUE;
}

JSBool Camera_setLookAt(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	CameraObject * thisObj = (CameraObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsVec;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsVec)) return JS_FALSE;
	thisObj->camera()->setLookAt(Vector3::fromScriptObject(s, jsVec));	
	thisObj->setSyncTransformNeeded(true);	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Camera_getLookAt(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	CameraObject * thisObj = (CameraObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	const Vector3 & eye = thisObj->camera()->lookAt();
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(eye.createScriptObject(s)));

	return JS_TRUE;
}

JSBool Camera_setUp(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	CameraObject * thisObj = (CameraObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsVec;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsVec)) return JS_FALSE;
	thisObj->camera()->setUp(Vector3::fromScriptObject(s, jsVec));	
	thisObj->setSyncTransformNeeded(true);	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Camera_getUp(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	CameraObject * thisObj = (CameraObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	const Vector3 & eye = thisObj->camera()->up();
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(eye.createScriptObject(s)));

	return JS_TRUE;
}

JSFunctionSpec CameraFuncs[] = {
	JS_FS("setEye", Camera_setEye, 1, 0),
	JS_FS("getEye", Camera_getEye, 0, 0),
	JS_FS("setLookAt", Camera_setLookAt, 1, 0),
	JS_FS("getLookAt", Camera_getLookAt, 0, 0),
	JS_FS("setUp", Camera_setUp, 1, 0),
	JS_FS("getUp", Camera_getUp, 0, 0),
    JS_FS_END
};


///////////////////////////////////////////////////////////////////////////////

CameraVisualAttrib::CameraVisualAttrib(CameraObject * parentObject) 
{
	cameraObject_ = parentObject;
}

void CameraVisualAttrib::onSetTransform()
{
	VisualAttrib::onSetTransform();
	Camera * cam = cameraObject_->camera_;

	if (cam)
	{
		Matrix transform = cameraObject_->parentTransform() * transformMatrix();

		Vector3 eye(0.0f, 0.0f, 0.0f);
		Vector3 forward(0.0f, 0.0f, 1.0f);		
		Vector3 up(0.0f, -1.0f, 0.0f);
		Vector3 right(1.0f, 0.0f, 0.0f);
		
		
		Vector3 newEye = transform * eye;		
		Vector3 newLookAt = 
			cam->lookDistance() * (transform * forward - newEye).normalize() +
			newEye;
		Vector3 newUp = transform * up - newEye;

		cam->eye_ = newEye;
		cam->lookAt_ = newLookAt;
		cam->up_ = newUp;
		cam->updateMatrix();

	}
	
}

void CameraVisualAttrib::onGetTransform() const
{
	if (cameraObject_->syncTransformNeeded_)
	{
		cameraObject_->setTransformDirect(cameraObject_->syncCameraTransform(transform_));
	}
}

void CameraVisualAttrib::write(Writer & writer) const
{
	writer.writeParent<VisualAttrib>(this);
}

void CameraVisualAttrib::read(Reader & reader, unsigned char version)
{
	reader.readParent<VisualAttrib>(this);
}

///////////////////////////////////////////////////////////////////////////////
JSObject * CameraObject::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsCameraClass,
		0, 0, 0, CameraFuncs, 0, 0);

	return proto;
}

JSObject * CameraObject::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsCameraClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void CameraObject::create()
{
	camera_ = new Camera;
	syncTransformNeeded_ = false;
}

CameraObject::CameraObject() : visualAttrib_(this)
{
	create();
	setId("Camera");
}

CameraObject::CameraObject(const CameraObject & rhs, ElementMapping * elementMapping)
: visualAttrib_(this), SceneObject(rhs, elementMapping)
{
	create();
	if (rhs.camera_) 
	{
		delete camera_;
		camera_ = new Camera(*rhs.camera_);
	}
	else camera_ = NULL;
}


CameraObject::~CameraObject()
{	
	delete camera_;	
}


void CameraObject::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
}

void CameraObject::uninit()
{
	SceneObject::uninit();
}


bool CameraObject::update(float sceneTime)
{	
	bool needRedraw = SceneObject::update(sceneTime);

	return needRedraw;
}

Ray CameraObject::unproject(const Vector2 & projCoords) const
{
	return camera_->unproject(projCoords);
}

bool CameraObject::asyncLoadUpdate()
{
	return SceneObject::asyncLoadUpdate();
}

bool CameraObject::isLoaded() const
{
	return SceneObject::isLoaded();
}

void CameraObject::drawObject(GfxRenderer * gl) const
{
	SceneObject::drawObject(gl);
}


BoundingBox CameraObject::extents() const
{
	const float cEpsilon = 0.001f;
	return BoundingBox(Vector3(-cEpsilon, -cEpsilon, -cEpsilon), Vector3(cEpsilon, cEpsilon, cEpsilon));
}

Transform CameraObject::syncCameraTransform(const Transform & curTransform) const
{
	const float cEpsilon = 0.0001f;
	Vector3 eye = camera_->eye();
	Vector3 lookAt = camera_->lookAt();
	Vector3 up = camera_->up();

	//Rotate z axis to match rotAxis
	Matrix M = Matrix::Rotate(Vector3(0, 0, 1), lookAt - eye);
	//-y axis ends up at mY after the rotation
	Vector3 mY = M*Vector3(0, -1.0f, 0);

	//must rotate local -y axis to match camera up vector. Rotation must be
	//along rotAxis	
	float rotAngle = acos((mY * up) / up.magnitude());	
	Vector3 cross = mY ^ up;

	if (cross.magnitude() > cEpsilon)	
		M = Matrix::Rotate(rotAngle, cross.x, cross.y, cross.z) * M;

	//move origin to camera eye location
	M = Matrix::Translate(eye.x, eye.y, eye.z) * M;

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

void CameraObject::setTransformDirect(const Transform & transform)
{
	visualAttrib_.setTransformDirect(transform);
}

void CameraObject::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");	
	writer.write(camera_, "camera");
}

void CameraObject::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);

	if (version < 1)
	{
		VisualAttrib visualAttrib;
		reader.read(visualAttrib);
		visualAttrib_.setTransform(visualAttrib.transform());
		visualAttrib_.setOpacity(visualAttrib.opacity());
		visualAttrib_.setVisible(visualAttrib.isVisible());
	}
	else
		reader.read(visualAttrib_);		

	if (version >= 2)
	{
		delete camera_;
		reader.read(camera_);		
		if (version == 2 && !camera_)
		{
			camera_ = new Camera;
		}
	}	
}


void CameraObject::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();

	w.startTag("Camera");
	camera_->writeXml(w);
	w.endTag();
}

void CameraObject::readXml(XmlReader & r, xmlNode * parent)
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
		else if (r.isNodeName(curNode, "Camera"))
		{
			camera_->readXml(r, curNode);
			curNode = curNode->next;
		}
	}
}

SceneObject * CameraObject::intersect(Vector3 * out, const Ray & ray)
{
	return SceneObject::intersect(out, ray);
}

void CameraObject::setTo2dArea(float centerX, float centerY, 
	float width, float height, float fovY, float aspect)
{
	camera_->SetTo2DArea(centerX, centerY, width, height, fovY, aspect);
	setTransformDirect(syncCameraTransform(transform()));
}

void CameraObject::setCamera(const Camera & camera)
{
	float prevAspectRatio = camera_->aspectRatio();
	*camera_ = camera;
	camera_->setAspectRatio(prevAspectRatio);
	setTransformDirect(syncCameraTransform(transform()));
}

void CameraObject::setAspectRatio(const float & aspectRatio)
{
	camera_->setAspectRatio(aspectRatio);
}

void CameraObject::setFovY(const float & fovY)
{
	camera_->setFovY(fovY);
}

const float & CameraObject::fovY() const
{
	return camera_->fovY();
}

void CameraObject::setNearPlane(const float & nearPlane)
{
	camera_->setNearPlane(nearPlane);
}

const float & CameraObject::nearPlane() const
{
	return camera_->nearPlane();
}

void CameraObject::setFarPlane(const float & farPlane)
{
	camera_->setFarPlane(farPlane);
}

const float & CameraObject::farPlane() const
{
	return camera_->farPlane();
}
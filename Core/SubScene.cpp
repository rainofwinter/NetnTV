#include "stdafx.h"
#include "Subscene.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Xml.h"
#include "Scene.h"
#include "Reader.h"
#include "Camera.h"
#include "glUtils.h"
#include "Global.h"
#include "VideoPlayer.h"
#include "Document.h"
#include "DocumentTemplate.h"
#include "Exception.h"
#include "ElementMapping.h"
//#include "TextureRenderer.h"
#include "ScriptProcessor.h"

using namespace std;

JSClass jsSubsceneClass = InitClass(
	"Subscene", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);

JSBool Subscene_setWindowWidth(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Subscene * thisObj = (Subscene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsDouble;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsDouble)) return JS_FALSE;
	thisObj->setWindowWidth((float)jsDouble);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Subscene_setWindowHeight(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Subscene * thisObj = (Subscene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsDouble;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsDouble)) return JS_FALSE;
	thisObj->setWindowHeight((float)jsDouble);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Subscene_getWindowWidth(JSContext *cx, uintN argc, jsval *vp)
{	
	Subscene * thisObj = (Subscene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL((double)thisObj->windowWidth()));
	return JS_TRUE;
};

JSBool Subscene_getWindowHeight(JSContext *cx, uintN argc, jsval *vp)
{
	Subscene * thisObj = (Subscene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL((double)thisObj->windowHeight()));
	return JS_TRUE;
};


JSBool Subscene_setScene(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Subscene * thisObj = (Subscene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsScene;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsScene)) return JS_FALSE;
	
	if (!jsScene)
	{
		thisObj->setScene(NULL);
	}
	else
	{
		if (JS_GetPrototype(cx, jsScene) != s->sceneProto())
		{
			JS_ReportError(cx, "Subscene::setScene - parameter must be a Scene.");
			return JS_FALSE;
		}

		Scene * scene = (Scene *)JS_GetPrivate(cx, jsScene);	
		thisObj->setScene(scene);
	}
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Subscene_getScene(JSContext *cx, uintN argc, jsval *vp)
{
	Subscene * thisObj = (Subscene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Scene * scene = thisObj->scene();
	if (scene)	
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(scene->getScriptObject(s)));	
	else
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	return JS_TRUE;
};


JSBool Subscene_setMode(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Subscene * thisObj = (Subscene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	uint32 val;
	if (!JS_ConvertArguments(cx, argc, argv, "u", &val)) return JS_FALSE;
	Subscene::Mode mode = (Subscene::Mode)val;
	thisObj->setMode(mode);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Subscene_setTransparentBg(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Subscene * thisObj = (Subscene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSBool val;
	if (!JS_ConvertArguments(cx, argc, argv, "b", &val)) return JS_FALSE;	
	thisObj->setTransparentBg((bool)val);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Subscene_setPosition(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Subscene * thisObj = (Subscene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsdouble jsX, jsY;
	if (!JS_ConvertArguments(cx, argc, argv, "dd", &jsX, &jsY)) return JS_FALSE;	

	if (thisObj->mode() == Subscene::ModeDraggable)
		thisObj->setDragPosition(Vector2((float)jsX, (float)jsY));
	else
		thisObj->setScrollPosition(Vector2((float)jsX, (float)jsY));

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Subscene_getPosition(JSContext *cx, uintN argc, jsval *vp)
{
	Subscene * thisObj = (Subscene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	
	Vector2 position(0, 0);
	if (thisObj->mode() == Subscene::ModeDraggable)
		position = thisObj->dragPosition();		
	else
		position = thisObj->scrollPosition();

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(position.createScriptObject(s)));
	return JS_TRUE;
};

JSFunctionSpec SubsceneFuncs[] = {
	JS_FS("setScene", Subscene_setScene, 1, 0),
	JS_FS("getScene", Subscene_getScene, 0, 0),
	JS_FS("setTransparentBg", Subscene_setTransparentBg, 1, 0),
	JS_FS("setMode", Subscene_setMode, 1, 0),
	JS_FS("setWindowWidth", Subscene_setWindowWidth, 1, 0),
	JS_FS("setWindowHeight", Subscene_setWindowHeight, 1, 0),
	JS_FS("getWindowWidth", Subscene_getWindowWidth, 0, 0),
	JS_FS("getWindowHeight", Subscene_getWindowHeight, 0, 0),
	JS_FS("setPosition", Subscene_setPosition, 1, 0),
	JS_FS("getPosition", Subscene_getPosition, 0, 0),
    JS_FS_END
};


///////////////////////////////////////////////////////////////////////////////
JSObject * Subscene::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsSubsceneClass,
		0, 0, 0, SubsceneFuncs, 0, 0);

	return proto;
}

JSObject * Subscene::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsSubsceneClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}


void Subscene::create()
{
	scene_ = 0;	

	windowWidth_ = 320;
	windowHeight_ = 200;

	mode_ = ModeFixed;
	
	scenePos_ = Vector2(0, 0);
	dragScenePos_ = Vector2(0, 0);
	
	maxPlusDrag_ = 0;
	maxMinusDrag_ = 0;
	maxTopDrag_ = 0;
	maxBotDrag_ = 0;
	dragVertical_ = 1;

	transparentBg_ = 0;

	scrollTweenDuration_ = 0;

	locked_ = false;
	linkWithViewerLevelObjects_ = false;
	displayingAppObjects_ = false;

	isLoaded_ = false;

	dragSnap_ = false;
	justReleased_ = false;

	moveDirection_ = Undetermined;
	pageMovingId_ = -1;

	cameraMode_ = ParentScene;
	
	//SceneChanger zoom
	zoomFlag_ = false;
	isMultiTouched_ = false;
    isTouched_ = false;
    trans_ = Vector3(0,0,0);
    normalScaleX_ = 1;
	normalScaleY_ = 1;
    maxZoomMagnification_ = 2.0;
    multiTouchEndTime_ = 0;

	curMag_ = 1.0;
	
	pressMag_ = 1.0;

	dragZooming_ = false;
}

Subscene::Subscene()
{
	create();
	setId("Subscene");
}


Subscene::Subscene(const Subscene & rhs, ElementMapping * elementMapping) 
: 
SceneObject(rhs, elementMapping)
{
	create();
	visualAttrib_ = rhs.visualAttrib_;
	if (scene_) scene_->removeObjRef(this);
	scene_ = rhs.scene_;
	if (scene_) scene_->addObjRef(this);
	
	mode_ = rhs.mode_;
	windowWidth_ = rhs.windowWidth_;
	windowHeight_ = rhs.windowHeight_;

	maxPlusDrag_ = rhs.maxPlusDrag_;
	maxMinusDrag_ = rhs.maxMinusDrag_;
	maxTopDrag_ = rhs.maxTopDrag_;
	maxBotDrag_ = rhs.maxBotDrag_;
	dragVertical_ = rhs.dragVertical_;

	transparentBg_ = rhs.transparentBg_;
	linkWithViewerLevelObjects_ = rhs.linkWithViewerLevelObjects_;
	
}

void Subscene::remapReferences(const ElementMapping & elementMapping)
{
	SceneObject::remapReferences(elementMapping);

	if (scene_) scene_->removeObjRef(this);
	scene_ = elementMapping.mapScene(scene_);
	if (scene_) scene_->addObjRef(this);
}

Subscene::~Subscene()
{
	uninit();
	if (scene_) scene_->removeObjRef(this);
}

void Subscene::setWindowWidth(const float & width) 
{
	windowWidth_ = width;
}

void Subscene::setWindowHeight(const float & height) 
{
	windowHeight_ = height;	
}

void Subscene::setScene(Scene * const & scene)
{	
	setScene(this, scene);
}

void Subscene::setScene(SceneObject * containerObject, Scene * const & scene)
{	
	if (scene) scene->setCurContainerObject(containerObject);
	if (scene_ == scene) return;

	if (scene_) 
	{
		scene_->removeObjRef(this);
	}

	scene_ = scene;
	if (scene_) 
	{
		isLoaded_ = false;	
		scene_->addObjRef(this);
		
		//SceneChanger zoom
		zoomFlag_ = scene_->zoom();
		switch (scene_->zoomMag())
		{
		case 0:
			maxZoomMagnification_ = 2.0;
			break;
		case 1:
			maxZoomMagnification_ = 3.0;
			break;
		case 2:
			maxZoomMagnification_ = 4.0;
			break;
		default:
			maxZoomMagnification_ = 2.0;
			break;
		}
		curMag_ = Global::instance().magnification();
	}
	else 
		isLoaded_ = true;
}

void Subscene::changeScene(Scene * const & scene)
{
	setScene(scene);
}

void Subscene::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
	if (scene_) scene_->handleInitBasedOnRefs(firstTime);

	//isLoaded_ should be trivially true if scene_ is null
	isLoaded_ = scene_ == 0;
	//trigger initial update of isLoaded_ field
	asyncLoadUpdate();
}

void Subscene::uninit()
{		
	if (!gl_) return;
	
	isLoaded_ = false;

	SceneObject::uninit();
	if (scene_)scene_->handleInitBasedOnRefs();
}

float Subscene::subsceneWidth() const
{
	if (!scene_) return 0;
	if (mode_ == ModeScrollable)
		return windowWidth_;
	else
		return scene_->screenWidth();
}

float Subscene::subsceneHeight() const
{
	if (!scene_) return 0;
	if (mode_ == ModeScrollable)
		return windowHeight_;
	else
		return scene_->screenHeight();
}

void Subscene::drawBg(GfxRenderer * gl) const
{
	if (!isValid()) return;
	float sw = scene_->screenWidth();
	float sh = scene_->screenHeight();

	//draw scene background
	gl->useColorProgram();
	const Color & bgColor = scene_->bgColor();
	gl->setColorProgramColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
	gl->drawRect(0, 0, sw, sh);
}

void Subscene::drawObject(GfxRenderer * gl) const
{
	if (!isValid()) return;	

	bool draggable = isDraggable();
	/*
	not isScrollable(). isScrollable() will return 0 if the scroll window is 
	the same size or bigger than the scroll scene. However even in such 
	situations we want clipping to be applied.
	*/
	bool isScrollMode = mode_ == ModeScrollable; 
		
	gl->pushMatrix();

	if (draggable)	
		gl->multMatrix(Matrix::Translate(dragScenePos_.x, dragScenePos_.y, 0));	
	else if (isScrollMode)
		gl->multMatrix(Matrix::Translate(scenePos_.x, scenePos_.y, 0));	
			

	float sw = scene_->screenWidth();
	float sh = scene_->screenHeight();

	if (isScrollMode)
	{				
		gl->beginIncStencilDrawing();
		gl->useColorProgram();
		gl->setColorProgramColor(0,0,0,0);
		gl->drawRect(-scenePos_.x, -scenePos_.y, windowWidth_, windowHeight_);
		gl->endStencilDrawing();		
	}

	
	if (!transparentBg_) drawBg(gl);	

	//---------------------------------	

	Matrix camMatrix = gl->cameraMatrix();	
	Matrix fullMat = camMatrix * gl->modelMatrix();

	//Vector3 a = fullMat * Vector3(0.0f, 0.0f, 0.0f);
	Vector3 a(
		fullMat._14 / fullMat._44, 
		fullMat._24 / fullMat._44, 
		fullMat._34 / fullMat._44);
	Vector3 b = fullMat * Vector3(sw, 0.0f, 0.0f);
	Vector3 d = fullMat * Vector3(0, sh, 0.0f);

	const static float cEpsilon = 0.0001f;

	bool noNontrivialTransform = 
		fabs(a.y - b.y) < cEpsilon && fabs(d.x - a.x) < cEpsilon; 

	//-----------------------------------
	
	if (cameraMode_ == ChildScene)
	{
		if (noNontrivialTransform)
		{			
			
			Matrix prevMat = gl->modelMatrix();
			gl->loadMatrix(Matrix::Identity());

			//after the camera transform has been applied to the scene, use
			//camMod matrix to transform the camera output to the correct 
			//position and size
			Matrix camMod = gl->modelMatrix(); //identity
			float sx = (b.x - a.x) / 2.0f;
			float sy = (d.y - a.y) / -2.0f;
			camMod._11 = sx;
			camMod._22 = sy;		
			camMod._14 = sx + a.x;
			camMod._24 = -sy + a.y;

			const Matrix & sceneCamMatrix = scene_->camera()->GetMatrix();
			/*
			a.z = (sceneCamMatrix_.34 + sceneCamMatrix._44 * camMod._34) / sceneCamMatrix._44
			camMod._34 = ??
			camMod._34 = (sceneCamMatrix._44 * a.z - sceneCamMatrix._34) / sceneCamMatrix._44
			= a.z - sceneCamMatrix._34/sceneCamMatrix._44
			*/		
			//camMod._34 = (sceneCamMatrix._44 * a.z - sceneCamMatrix._34) / sceneCamMatrix._44;
			
			gl->setCameraMatrix(camMod * sceneCamMatrix);
			scene_->draw(gl);			
			gl->setCameraMatrix(camMatrix);	
			gl->loadMatrix(prevMat);
			
		}
		else
		{
			//Subscenes can only be drawn when they have no non trivial transform, 
			//ie. they can't have rotations etc.
			
			gl->useColorProgram();
			const Color & bgColor = scene_->bgColor();
			gl->useColorProgram();
			gl->setColorProgramColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
			gl->drawRect(0.0f, 0.0f, modeWidth(), modeHeight());
		}	
	}
	else
	{
		//just user parent camera settings as-is
		scene_->draw(gl);
	}

	
	if (isScrollMode)
	{	
		gl->beginDecStencilDrawing();
		gl->useColorProgram();
		gl->setColorProgramColor(0,0,0,0);
		gl->drawRect(-scenePos_.x, -scenePos_.y, windowWidth_, windowHeight_);
		gl->endStencilDrawing();
	}

	gl->popMatrix();
}


void Subscene::endScrollTween()
{
	scrollTweenDuration_ = 0.0f;
}

bool Subscene::isScrollTweenHappening() const 
{
	return scrollTweenDuration_ > 0;
}

void Subscene::startScrollTween(Vector2 * target,
	const Vector2 & startPos, const Vector2 & endPos)
{
	scrollTweenTargetPos_ = target;
	scrollTweenDuration_ = 0.3f;
	scrollTweenStartTime_ = Global::currentTime();	
	scrollTweenStartPos_ = startPos;		
	scrollTweenEndPos_ = endPos;	
	//enforce integer end pos
	scrollTweenEndPos_.x = roundFloat(scrollTweenEndPos_.x);
	scrollTweenEndPos_.y = roundFloat(scrollTweenEndPos_.y);
}



void Subscene::startDragTween(const DragSnapState & dragSnapState)
{
	Vector2 dragTweenEndPos;
	if (dragSnapState == SnapInitial)	
		dragTweenEndPos = Vector2(0.0f, 0.0f);
	else if (dragSnapState == SnapLeft)
		dragTweenEndPos = Vector2(-maxMinusDrag_, 0.0f);
	else if (dragSnapState == SnapRight)
		dragTweenEndPos = Vector2(maxPlusDrag_, 0.0f);
	else if (dragSnapState == SnapUp)
	{
		if (dragVertical_ != 2)
			dragTweenEndPos = Vector2(0.0f, -maxMinusDrag_);
		else
			dragTweenEndPos = Vector2(0.0f, -maxTopDrag_);
	}
	else if (dragSnapState == SnapDown)
	{
		if (dragVertical_ != 2)
			dragTweenEndPos = Vector2(0.0f, maxPlusDrag_);
		else
			dragTweenEndPos = Vector2(0.0f, maxBotDrag_);
	}


	dragTween_.start(
		Global::currentTime(), dragScenePos_, dragTweenEndPos);
}

bool Subscene::update(float sceneTime)
{
	bool needRedraw = SceneObject::update(sceneTime);
	if (scene_) needRedraw |= scene_->update(sceneTime);
	if (redrawTriggered_)
	{
		needRedraw = true;
		redrawTriggered_ = false;
	}

	bool notPressed = pageMovingId_ == -1;

	//Note, we need to keep scrollTween and dragTween exclusive hence the if/else

	if (dragTween_.isHappening())
	{
		dragScenePos_ = dragTween_.update(sceneTime);
		needRedraw = true;
	}
	else if (notPressed && (!dragVelHandler_.isZeroVelocity() || justReleased_))
	{			
		dragVelHandler_.update(sceneTime);
		
		if (isScrollable())
		{
			float minX = windowWidth_ - scene_->screenWidth();
			float minY = windowHeight_ - scene_->screenHeight();

			scenePos_ += dragVelHandler_.moveDelta();
			
			////NEW
			bool hasHitBoundaryX = false, hasHitBoundaryY = false;
			if (scenePos_.x < minX || scenePos_.x > 0)
				hasHitBoundaryX = true;
			if (scenePos_.y < minY || scenePos_.y > 0)
				hasHitBoundaryY = true;

			dragVelHandler_.hasHitBoundary(hasHitBoundaryX, hasHitBoundaryY);

			if (dragVelHandler_.isZeroVelocity())
			{
				Vector2 endPos = scenePos_;
				if (scenePos_.y > 0) endPos.y = 0;
				if (scenePos_.y < minY) endPos.y = minY;
				if (scenePos_.x > 0) endPos.x = 0;
				if (scenePos_.x < minX) endPos.x = minX;

				startScrollTween(&scenePos_, scenePos_, endPos);
			}
			
		}
		else if (isDraggable())
		{
			dragScenePos_ += dragVelHandler_.moveDelta();

			bool hasHitBoundaryX = false, hasHitBoundaryY = false;
			if (dragScenePos_.x < -maxMinusDrag_ || dragScenePos_.x > maxPlusDrag_)
				hasHitBoundaryX = true;
			if ((dragScenePos_.y < -maxMinusDrag_ || dragScenePos_.y > maxPlusDrag_) && dragVertical_ != 2)
				hasHitBoundaryY = true;
			else if (dragScenePos_.y < -maxTopDrag_ || dragScenePos_.y > maxBotDrag_)
				hasHitBoundaryY = true;

			dragVelHandler_.hasHitBoundary(hasHitBoundaryX, hasHitBoundaryY);

			if (dragVelHandler_.isZeroVelocity())
			{
				Vector2 endPos = dragScenePos_;
				if (dragVertical_ != 2)
				{
					if (dragScenePos_.y > maxPlusDrag_) endPos.y = maxPlusDrag_;
					if (dragScenePos_.y < -maxMinusDrag_) endPos.y = -maxMinusDrag_;
				}
				else
				{
					if (dragScenePos_.y > maxBotDrag_) endPos.y = maxBotDrag_;
					if (dragScenePos_.y < -maxTopDrag_) endPos.y = -maxTopDrag_;
				}
				if (dragScenePos_.x > maxPlusDrag_) endPos.x = maxPlusDrag_;
				if (dragScenePos_.x < -maxMinusDrag_) endPos.x = -maxMinusDrag_;

				startScrollTween(&dragScenePos_, dragScenePos_, endPos);
			}
		}	
		
		needRedraw = true;
	}


	if (isScrollTweenHappening())
	{
		float t = (sceneTime - scrollTweenStartTime_) / scrollTweenDuration_;		
		if (t > 1) t = 1;		
		t = 1 - (t - 1)*(t - 1);

		*scrollTweenTargetPos_ =
			scrollTweenStartPos_ + t * (scrollTweenEndPos_ - scrollTweenStartPos_);	

		if (t == 1) endScrollTween();

		needRedraw = true;
	}
	
	//SceneChanger zoom
	if (zoomTransTween_.isHappening())
	{
		Vector3 pos = zoomTransTween_.update(sceneTime);
		Transform t = transform();
		t.setTranslation(pos);
        
		setTransform(t);
		trans_ = t.translation();
        
		needRedraw = true;
	}
	if (zoomScaleTween_.isHappening())
	{
		Vector3 scale = zoomScaleTween_.update(sceneTime);
		Transform t = transform();
        
		t.setScaling(scale);
		checkOutside(t);

		setTransform(t);
        scene_->setCurrentScale(scale);
		trans_ = t.translation();
		float mag = scale.x;
		if (mag > scale.y)
			mag = scale.y;
		if (mag > normalScaleX_ * maxZoomMagnification_)
			mag = normalScaleX_ * maxZoomMagnification_;
		else if (mag > normalScaleY_ * maxZoomMagnification_)
			mag = normalScaleY_ * maxZoomMagnification_;
		curMag_ = mag * Global::instance().magnification();;
		Global::instance().setGlobalCurMagnification(mag / normalScaleX_);

		if (!zoomScaleTween_.isHappening() && (scale.x == normalScaleX_ || scale.y == normalScaleY_)) //&& viewState_ != Normal)
		{
            Matrix m1 = t.computeMatrix().inverse();
			t.setPivot(0,0,0);
            Matrix m2 = t.computeMatrix().inverse();
            m2._14 = m1._14;    m2._24 = m1._24;
            m2 = m2.inverse();
            t.setMatrix(m2, true);
			setTransform(t);
		}

		needRedraw = true;	
	}
	
	if(dragZooming_)
	{
		//Vector3 scale = Vector3(curMag_,curMag_,curMag_);
		float stemp = curMag_/Global::instance().magnification();
		Vector3 scale = Vector3(stemp,stemp,stemp);
		scene_->setCurrentScale(scale);
	}
	
	//doAppObjectOffset();

	justReleased_ = false;
	return needRedraw;
}

AppObjectOffset Subscene::appObjectOffset() const
{
	AppObjectOffset ret;
	

	if (linkWithViewerLevelObjects_)
	{
		ret.clip = true;		
		
		Matrix m = parentTransform(); 
		m *= visualAttrib_.transformMatrix();

		ret.clipX = m._14;
		ret.clipY = m._24;
		ret.clipWidth = m._11 * modeWidth();
		ret.clipHeight = m._22 * modeHeight();

		if (isScrollable())
		{
			ret.dx = m._11 * scenePos_.x + m._14;
			ret.dy = m._22 * scenePos_.y + m._24;
			ret.sx = m._11;
			ret.sy = m._22;
		}
		else if (isDraggable())
		{
			ret.dx = m._11 * dragScenePos_.x + m._14;
			ret.dy = m._22 * dragScenePos_.y + m._24;
			ret.sx = m._11;
			ret.sy = m._22;

			ret.clipX = m._11 * dragScenePos_.x + m._14;
			ret.clipY =  m._22 * dragScenePos_.y + m._24;
		}
		else
		{
			ret.dx = m._14;
			ret.dy = m._24;
			ret.sx = m._11;
			ret.sy = m._22;
		}

		
	}

	return ret;
}
/*
void Subscene::doAppObjectOffset()
{
	if (isScrollable() && linkWithViewerLevelObjects_)
	{
		Matrix m = parentTransform(); 
		m *= visualAttrib_.transformMatrix();

		DocumentTemplate * docTemp = 
			parentScene_->parentDocument()->documentTemplate();

		Vector3 t = visualAttrib_.transform().translation();
		docTemp->setAdditionalAppObjectOffset(
			scene_, m._14 + scenePos_.x, m._24 + scenePos_.y);
	}

	if (isDraggable() && linkWithViewerLevelObjects_)
	{
		Matrix m = parentTransform(); 
		m *= visualAttrib_.transformMatrix();

		DocumentTemplate * docTemp = 
			parentScene_->parentDocument()->documentTemplate();

		Vector3 t = visualAttrib_.transform().translation();
		docTemp->setAdditionalAppObjectOffset(
			scene_, m._14 + dragScenePos_.x, m._24 + dragScenePos_.y);
	}	
}
*/
BoundingBox Subscene::extents() const
{
	if (!scene_) return BoundingBox();
	
	return BoundingBox(
		Vector3(0, 0, 0), 
		Vector3(modeWidth(), modeHeight(), 0));
}

float Subscene::sceneWidth() const
{	
	if (scene_) return scene_->screenWidth();
	else return 0;
}

float Subscene::sceneHeight() const
{
	if (scene_) return scene_->screenHeight();
	else return 0;
}

SceneObject * Subscene::intersect(Vector3 * intPt, const Ray & ray)
{
	if (!visualAttrib_.isVisible()) return 0;
	if (intersectRect(intPt, ray)) return this;
	else return 0;	
}

float Subscene::modeWidth() const
{
	if (isScrollable())
		return windowWidth_;
	else if (scene_)
		return scene_->screenWidth();
	else return 0;
}

void Subscene::setScrollPosition(const Vector2 & pos)
{
	scenePos_.x = floor(pos.x);
	scenePos_.y = floor(pos.y);
	resetScrollTween();
	dragVelHandler_.clearMoveHistory();	
}

void Subscene::setDragPosition(const Vector2 & pos)
{
	dragScenePos_.x = floor(pos.x);
	dragScenePos_.y = floor(pos.y);
	resetScrollTween();
	dragVelHandler_.clearMoveHistory();
}

float Subscene::modeHeight() const
{
	if (isScrollable())
		return windowHeight_;
	else if (scene_)
		return scene_->screenHeight();
	else return 0;
}

bool Subscene::intersectRect(Vector3 * out, const Ray & ray)
{
	if (!scene_) return false;
	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();
	
	float width = modeWidth();
	float height = modeHeight();

	Vector3 a = trans * Vector3(
		dragScenePos_.x, dragScenePos_.y, 0);
	Vector3 b = trans * Vector3(
		dragScenePos_.x, dragScenePos_.y + height, 0);
	Vector3 c = trans * Vector3(
		dragScenePos_.x + width, dragScenePos_.y + height, 0);
	Vector3 d = trans * Vector3(
		dragScenePos_.x + width, dragScenePos_.y, 0);

	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return true;
	}
	else return false;
}

bool Subscene::isScrollable() const
{
	return scene_ && 
		(
		windowWidth_ < scene_->screenWidth() || 
		windowHeight_ < scene_->screenHeight()
		) &&
		mode_ == ModeScrollable
		;
}

bool Subscene::isDraggable() const
{
	return scene_ && mode_ == ModeDraggable;
}

Vector2 Subscene::convertToChildSceneCoords(const Vector2 & coords) const
{	
	return toLocalScenePos(processCoords(coords));
}

Vector2 Subscene::convertFromChildSceneCoords(const Vector2 & coords) const
{	
	Vector2 vec2 = fromLocalScenePos(coords);

	Vector3 vec3(vec2.x, vec2.y, 0.0f);
	return parentTransform() * visualAttrib_.transformMatrix() * vec3;	
}

bool Subscene::clickEvent(const Vector2 & mousePos, int pressId)
{
	if (!isLoaded_) return false;
	bool handled = SceneObject::clickEvent(mousePos, pressId);
	if (!isValid()) return handled;

	Vector2 localPos = processCoords(mousePos);
	Vector2 localScenePos = toLocalScenePos(localPos);
	
	handled |= scene_->clickEvent(localScenePos, pressId);
	return handled;
}

bool Subscene::doubleClickEvent(const Vector2 & mousePos, int pressId)
{
	if (!isLoaded_) return false;
	bool handled = SceneObject::doubleClickEvent(mousePos, pressId);
	if (!isValid()) return handled;

	Vector2 localPos = processCoords(mousePos);
	Vector2 localScenePos = toLocalScenePos(localPos);
	
	//SceneChanger zoom
	if (zoomFlag_)
	{
		Transform t;
		t = transform();
		Vector3 scale = t.scaling();
		pivot_ = Vector3(mousePos.x, mousePos.y, 0);
		Matrix mat = t.computeMatrix();
		mat = mat.inverse();
		pivot_ = mat * pivot_;
		t.setPivot(pivot_);
		mat = t.computeMatrix();
		mat = mat.inverse();
		setTransform(t);
		// 임
		float curscenemag = curMag_;
		float mag;
		if (fabs(scale.x - normalScaleX_) > 0.00001 &&
			fabs(scale.y - normalScaleY_) > 0.00001)
		{
			mag = normalScaleX_;
			zoomScaleTween_.reset();
			zoomScaleTween_.start(Global::currentTime(), scale, Vector3(normalScaleX_, normalScaleY_, 1));
			//if (viewState_ != Normal)
			//	checkTranslateLandscape(t);
		}
		else
		{
			mag = normalScaleX_ * maxZoomMagnification_;
			if (mag >= normalScaleX_ * maxZoomMagnification_)
			{
				mag = normalScaleX_ * maxZoomMagnification_;
				zoomScaleTween_.reset();
				zoomScaleTween_.start(Global::currentTime(), scale, Vector3(mag, mag, 1));
			}
			else if (mag >= normalScaleY_ * maxZoomMagnification_)
			{
				mag = normalScaleY_ * maxZoomMagnification_;
				zoomScaleTween_.reset();
				zoomScaleTween_.start(Global::currentTime(), scale, Vector3(mag, mag, 1));
			}
		}
		curMag_ = mag * Global::instance().magnification();
		if (curscenemag != mag)
		{
			float globalmag = Global::instance().curSceneMagnification();
			Global::instance().setCurSceneMagnification(curMag_);
			scene()->parentDocument()->documentTemplate()->reInitCurScene(gl_);
			Global::instance().setCurSceneMagnification(globalmag);
		}
	}
	
	handled |= scene_->doubleClickEvent(localScenePos, pressId);
	if (handled) redrawTriggered_ = true;

	return handled;
}

Vector2 Subscene::toLocalScenePos(const Vector2 & localPos) const
{
	if (mode_ == ModeScrollable) return localPos - scenePos_;
	else return localPos - dragScenePos_;
}

Vector2 Subscene::fromLocalScenePos(const Vector2 & localScenePos) const
{
	if (mode_ == ModeScrollable) return localScenePos + scenePos_;
	else return localScenePos + dragScenePos_;
}

bool Subscene::pressEvent(const Vector2 & startPos, int pressId)
{	
	PressEventData & pressEvent = pressEvents_[pressId];
	pressEvent.pressed = PressEventData::NotPressed;
	if (!isValid()) return false;

	bool handled = SceneObject::pressEvent(startPos, pressId);
	if (!scene_) return handled;	

	Vector2 localPos = processCoords(startPos);
	Vector2 localScenePos = toLocalScenePos(localPos);
	
	handled |= scene_->pressEvent(localScenePos, pressId);

	if (handled)
		pressEvent.pressed = PressEventData::PassedToChild;
	else
		pressEvent.pressed = PressEventData::Pressed;

	pressEvent.startedMoving = false;	
	pressEvent.pressPt = localPos;
	pressEvent.curPt = startPos;
	pressEvent.worldPressPt = startPos;
	pressScenePos_ = scenePos_;
	pressDragScenePos_ = dragScenePos_;	
	dragVelHandler_.clearMoveHistory();
	resetScrollTween();
	dragTween_.reset();
	moveDirection_ = Undetermined;
	
	//SceneChanger zoom
	if (locked_)
	{
		zoomFlag_ = scene_->zoom();
		if (zoomFlag_)
			locked_ = false;
		else
			locked_ = true;
	}
	
	
	if ((isScrollable() || isDraggable() || scene_->zoom()) && !handled && !locked_)
	{			
		handled = true;
	}
	
	//SceneChanger zoom
	Vector2 t1, t2;
    if (pressId == 0 || pressId == 1)
    {
		pressMag_ = curMag_;
        isTouched_ = true;
        trans_ = transform().translation();
        if (pressEvents_[0].pressed == PressEventData::Pressed &&
            pressEvents_[1].pressed == PressEventData::Pressed &&
            zoomFlag_)
        {
            t1 = pressEvents_[0].curPt;
            t2 = pressEvents_[1].curPt;
            isMultiTouched_ = true;
            Vector2 pivot = t1 + t2;
            pivot /= 2;
			pivot_ = Vector3(pivot.x, pivot.y, 0);
            Vector2 v = t1 - t2;
            defaultTouchLength_ = v.magnitude();
            scale_ = transform().scaling();
            Matrix mat = transform().computeMatrix();
            mat = mat.inverse();
            pivot_ = mat * pivot_;
            Transform t = transform();
            t.setPivot(pivot_);
            setTransform(t);

			dragZooming_ = true;
        }
    }
	
	pageMovingId_ = pressId;
	if (!handled) pressEvent.pressed = PressEventData::PassedToParent;
	return handled;
}

bool Subscene::moveEvent(const Vector2 & pos, int pressId)
{
	PressEventData & pressEvent = pressEvents_[pressId];
	if (!isLoaded_) return false;
	bool handled = SceneObject::moveEvent(pos, pressId);
	if (!isValid()) return handled;

	if (pressEvent.pressed == PressEventData::PassedToParent) return false;

	Vector2 localPos = processCoords(pos);
	Vector2 localScenePos = toLocalScenePos(localPos);

	handled |= scene_->moveEvent(localScenePos, pressId);

	//short circuit - don't do anything if the child scene processed the event
	//or if locked
	if (handled) return true;
	
	
	if (pressEvents_[0].pressed == PressEventData::Pressed &&
		pressEvents_[1].pressed == PressEventData::Pressed &&
		zoomFlag_)
	{
		isMultiTouched_ = true;
	}

	//SceneChanger zoom
	if (isMultiTouched_)
	{      
		if (pressId == 0 || pressId == 1)
		{
			Vector2 t1, t2;
            pressEvent.curPt = pos;
			t1 = pressEvents_[0].curPt;
			t2 = pressEvents_[1].curPt;
			Vector2 v = t1 - t2;
			float scale = v.magnitude() / defaultTouchLength_;
            Vector3 temp = scale * scale_;
			Transform t = transform();
			// normalScale_ 이상 축소 불가능 ---------------------------
			if (temp.x < normalScaleX_ || temp.y < normalScaleY_)
				temp = Vector3(normalScaleX_,normalScaleY_,1);
			else if (temp.x > normalScaleX_ * maxZoomMagnification_ || temp.y > normalScaleY_ * maxZoomMagnification_)
				temp = Vector3(normalScaleX_ * maxZoomMagnification_,normalScaleY_ * maxZoomMagnification_,1);
			//----------------------------------------------------------
            t.setScaling(temp);
			float mag = temp.x;
			if (mag > temp.y)
				mag = temp.y;
			curMag_ = mag * Global::instance().magnification();
			Global::instance().setGlobalCurMagnification(mag / normalScaleX_);
            //  빈공간 안보이게하기
            checkOutside(t);
			setTransform(t);
		}
        return true;
	}
    else if(transform().scaling().x > normalScaleX_ && transform().scaling().y > normalScaleY_ && zoomFlag_)
    {
        bool checkParentHandle;
        Vector2 dir = pos - pressEvent.curPt;
        pressEvent.curPt = pos;
        Transform t = transform();
        t.setTranslation(t.translation() + Vector3(dir.x, dir.y, 0));
		//checkParentHandle = checkParentHandled(t);

		checkOutside(t);
		setTransform(t);

        //if (checkParentHandle)
        //    return false;
        //else
            return true;
    }
	pressEvent.curPt = pos;
	
	/*
	Removing pressEvent_.pressed == PressEventData::NotPressed condition

	Because: Suppose for example there is a vertical scrollable subscene A
	contained in a horizontal subscene B. if the user clicks on A and drags
	horizontally, pressEvent_.pressed will be NotPressed within B. However, B
	still needs to process the moveEvent in order to carry out horizontal 
	scrolling

	Beware of possible regressions
	*/
	//if (locked_ || pressEvent_.pressed == PressEventData::NotPressed) return false;
	if (locked_) return false;
	if (pageMovingId_ == -1)
		pageMovingId_ = pressId;
	else if (pageMovingId_ != pressId) return false;
	
	float dragXDelta = localPos.x - pressEvent.pressPt.x;
	float dragYDelta = localPos.y - pressEvent.pressPt.y;
	if (moveDirection_ == Undetermined)
	{
		if (fabs(dragXDelta) > fabs(dragYDelta))
			moveDirection_ = Horz;
		else 
			moveDirection_ = Vert;
	}	

	if (!pressEvent.startedMoving)
	{
		bool supportsVDragging = false; 
		bool supportsHDragging = false; 		
		bool passOn = false;

		if (isScrollable())
		{			
			supportsVDragging = windowHeight_ < scene_->screenHeight();
			supportsHDragging = windowWidth_ < scene_->screenWidth();

			if ((moveDirection_ == Vert && !supportsVDragging) || 
			(moveDirection_ == Horz && !supportsHDragging))
			{
				//unrelated scrolling direction
				//must pass on event data to parents upstream
				passOn = true;			
			}

		}
		else if (isDraggable())
		{
			supportsVDragging = dragVertical_ == 1;
			supportsHDragging = dragVertical_ == 0;

			if (((moveDirection_ == Vert && !supportsVDragging) || 
			(moveDirection_ == Horz && !supportsHDragging)) && dragVertical_ != 2)
			{
				//unrelated scrolling direction
				//must pass on event data to parents upstream
				passOn = true;			
			}

			if (dragVertical_ == 2)
			{

			}
			else if (moveDirection_ == Vert)
			{
				if (!supportsVDragging) passOn = true;

				if (dragScenePos_.y == -maxMinusDrag_ && dragYDelta < 0) 
					passOn = true;

				if (dragScenePos_.y == maxPlusDrag_ && dragYDelta > 0) 
					passOn = true;
			}
			else if (moveDirection_ == Horz)
			{
				if (!supportsHDragging) passOn = true;
				
				if (dragScenePos_.x == -maxMinusDrag_ && dragXDelta < 0) 
					passOn = true;

				if (dragScenePos_.x == maxPlusDrag_ && dragXDelta > 0) 
					passOn = true;				
			}
		}

		

		if (passOn)
		{
			pressEvent.pressed = PressEventData::PassedToParent;
			return false;			
		}



		endScrollTween();
		pressEvent.startedMoving = true;
		pressEvent.startPt = localPos;
		pressEvent.startMovingTime = Global::currentTime();
		dragVelHandler_.clearMoveHistory();
	}
	
	//SceneChanger zoom
	if (isScrollable()  && !isMultiTouched_)
	
	//if (isScrollable())
	{		

		float minX = windowWidth_ - scene_->screenWidth();
		float minY = windowHeight_ - scene_->screenHeight();
		Vector2 scrollDelta = localPos - pressEvent.startPt;


		scenePos_ = pressScenePos_ + scrollDelta;		

		if (minX >= 0) scenePos_.x = pressScenePos_.x;
		if (minY >= 0) scenePos_.y = pressScenePos_.y;

		dragVelHandler_.addMoveHistory(scenePos_, Global::currentTime());
		handled = true;
		redrawTriggered_ = true;	
		
		//if at the end of scrollable region, pass move event to parent
		//to process potential page change	
		bool passOn = false;
		if (minX < 0)
		{
			if (scenePos_.x == 0 && dragXDelta > 0 && moveDirection_ == Horz) 
				{scenePos_.x = 0; passOn = true;}

			if (scenePos_.x == minX && dragXDelta < 0 && moveDirection_ == Horz) 
				{scenePos_.x = minX; passOn = true;}			
		}

		if (minY < 0)
		{
			if (scenePos_.y == 0 && dragYDelta > 0 && moveDirection_ == Vert) 
					{scenePos_.y = 0; passOn = true;}

			if (scenePos_.y == minY && dragYDelta < 0 && moveDirection_ == Vert) 
				{scenePos_.y = minY; passOn = true;}
		}

		if (passOn)
		{
			pressEvent.pressed = PressEventData::PassedToParent;
			return false;
		}	
		
		
	} 
	else if (isDraggable())
	{
		Vector2 scrollDelta = localPos - pressEvent.startPt;
		dragScenePos_ = pressDragScenePos_ + scrollDelta;
		dragScenePos_ = boundDragPos(dragScenePos_);	

		if (dragVertical_ == 1) dragScenePos_.x = pressDragScenePos_.x;
		else if (dragVertical_ == 0) dragScenePos_.y = pressDragScenePos_.y;
		//else if (dragVertical_ == 2)

		dragVelHandler_.addMoveHistory(dragScenePos_, Global::currentTime());
		handled = true;
		redrawTriggered_ = true;
	}

	return handled;
}

Vector2 Subscene::boundToWindow(const Vector2 & scenePos) const
{
	Vector2 ret = scenePos;
	float minX = windowWidth_ - scene_->screenWidth();
	float minY = windowHeight_ - scene_->screenHeight();	
	if (ret.x < minX) ret.x = minX;
	if (ret.y < minY) ret.y = minY;
	if (ret.x > 0) ret.x = 0;
	if (ret.y > 0) ret.y = 0;
	return ret;
}

Vector2 Subscene::boundDragPos(const Vector2 & dragPos) const
{
	Vector2 ret = dragPos;
	if (ret.x > maxPlusDrag_)
		ret.x = maxPlusDrag_;

	if (dragVertical_ != 2)
	{
		if (ret.y > maxPlusDrag_)
			ret.y = maxPlusDrag_;
	}
	else
	{
		if (ret.y > maxBotDrag_)
			ret.y = maxBotDrag_;
	}

	if (ret.x < -maxMinusDrag_)
		ret.x = -maxMinusDrag_;
	if (dragVertical_ != 2)
	{
		if (ret.y < -maxMinusDrag_)
			ret.y = -maxMinusDrag_;
	}
	else
	{
		if (ret.y < -maxTopDrag_)
			ret.y = -maxTopDrag_;
	}

	return ret;
}

Vector2 Subscene::processCoords(const Vector2 & screenCoords) const
{
	const Camera * camera = parentScene_->camera();
	Vector2 deviceCoords = 
		parentScene_->screenToDeviceCoords(screenCoords);

	Ray ray = camera->unproject(deviceCoords);
	Vector3 out;
	const_cast<Subscene *>(this)->intersect(&out, ray);
	
	Matrix invTrans = 
		(parentTransform() * visualAttrib_.transformMatrix()).inverse();

	Vector3 invClickPt = invTrans * out;
	return Vector2(invClickPt.x, invClickPt.y);
}

void Subscene::dragRelease(const PressEventData & pressEvent, const Vector2 & localPos)
{
	bool isQuickFlick = 
		Global::currentTime() - pressEvent.startMovingTime < 
		Global::flickTime();

	float xDelta = localPos.x - pressEvent.startPt.x;
	float yDelta = localPos.y - pressEvent.startPt.y;

	DragSnapState dragEndState;

	Vector2 dragPos = dragScenePos_;
	if (isQuickFlick) dragPos = pressDragScenePos_;

	if (moveDirection_ == Horz)
	{
		float initialDist = fabs(dragPos.x);
		float rightDist = fabs(maxPlusDrag_ - dragPos.x);
		float leftDist = fabs(-maxMinusDrag_ - dragPos.x);
		if (maxPlusDrag_ == 0) rightDist = FLT_MAX;
		if (maxMinusDrag_ == 0) leftDist = FLT_MAX;

		if (isQuickFlick)
		{
			if (leftDist < initialDist && leftDist < rightDist)
			{
				dragEndState = SnapLeft;
				if (xDelta > 0) dragEndState = SnapInitial;
			}
			else if (rightDist < initialDist && rightDist < leftDist)
			{
				dragEndState = SnapRight;
				if (xDelta < 0) dragEndState = SnapInitial;
			}
			else
			{
				dragEndState = SnapInitial;
				if (xDelta < 0) dragEndState = SnapLeft;
				else if (xDelta > 0) dragEndState = SnapRight;
			}
		}	
		else
		{
			if (leftDist < initialDist && leftDist < rightDist)			
				dragEndState = SnapLeft;
			else if (rightDist < initialDist && rightDist < leftDist)			
				dragEndState = SnapRight;			
			else			
				dragEndState = SnapInitial;
		}

		startDragTween(dragEndState);
	}
	else if (moveDirection_ == Vert)
	{
		float initialDist = fabs(dragScenePos_.y);
		float downDist = fabs(maxPlusDrag_ - dragScenePos_.y);
		float upDist = fabs(-maxMinusDrag_ - dragScenePos_.y);
		if (maxMinusDrag_ == 0) upDist = FLT_MAX;
		if (maxPlusDrag_ == 0) downDist = FLT_MAX;

		if (isQuickFlick)
		{
			if (upDist < initialDist && upDist < downDist)			
			{
				dragEndState = SnapUp;
				if (yDelta > 0) dragEndState = SnapInitial;
			}
			else if (downDist < initialDist && downDist < upDist)			
			{
				dragEndState = SnapDown;
				if (yDelta < 0) dragEndState = SnapInitial;
			}
			else			
			{
				dragEndState = SnapInitial;
				if (yDelta < 0) dragEndState = SnapUp;
				else if (yDelta > 0) dragEndState = SnapDown;
			}
		}
		else
		{
			if (upDist < initialDist && upDist < downDist)			
				dragEndState = SnapUp;
			else if (downDist < initialDist && downDist < upDist)			
				dragEndState = SnapDown;			
			else			
				dragEndState = SnapInitial;
			
		}

		startDragTween(dragEndState);
	}

	

	
}

bool Subscene::releaseEvent(const Vector2 & startPos, int pressId)
{
	PressEventData & pressEvent = pressEvents_[pressId];

	if (!isLoaded_) return false;
	bool handled = SceneObject::releaseEvent(startPos, pressId);
	if (!scene_) return handled;

	bool passedToParent = pressEvent.pressed == PressEventData::PassedToParent;
	justReleased_ = true;
	pressEvent.pressed = PressEventData::NotPressed;
	if (passedToParent) return false;
	
	Vector2 localPos = processCoords(startPos);
	Vector2 localScenePos = toLocalScenePos(localPos);

	handled |= scene_->releaseEvent(localScenePos, pressId);
	
	//SceneChanger zooom
	std::map<int, PressEventData>::iterator itr = pressEvents_.begin();
    int pesize = 0;
	for (; itr != pressEvents_.end() ; itr++)
    {
        if ((*itr).second.pressed == PressEventData::Pressed)
            pesize++;
    }
    
    if (Global::currentTime() - multiTouchEndTime_ < Global::flickTime())
    {
        pressEvent.pressed = PressEventData::NotPressed;
        return true;
    }
	
	if (pressEvents_[0].pressed == PressEventData::Pressed &&
        pressEvents_[1].pressed == PressEventData::Pressed &&
        zoomFlag_)
	{
        if (pressId == 0 || pressId == 1)
        {
            isMultiTouched_ = false;
            
            multiTouchEndTime_ = Global::currentTime();
            Transform t;
            t = transform();
            Vector3 scale = t.scaling();
            t.setPivot(0,0,0);
            //zoomTransTween_.reset();
            Vector3 startPos = t.translation();
            Vector3 endPos;
			float curscenemag = curMag_;
            if (scale.x <= normalScaleX_ && scale.y <= normalScaleY_)
            {
                setTransform(t);
                zoomScaleTween_.reset();
                zoomScaleTween_.start(Global::currentTime(), scale, Vector3(normalScaleX_, normalScaleY_, 1));                
                
                t.setScaling(normalScaleX_, normalScaleY_, 1);
                //t.setTranslation(0,0,0);
				curMag_ = normalScaleX_ * Global::instance().magnification();
            }
            else 
            {
				// 임
				float mag = t.scaling().x;
                if (mag > t.scaling().y)
                    mag = t.scaling().y;
                if (mag > normalScaleX_ * maxZoomMagnification_)
                {
                    mag = normalScaleX_ * maxZoomMagnification_;
                    zoomScaleTween_.reset();
                    zoomScaleTween_.start(Global::currentTime(), scale, Vector3(mag, mag, 1));
                }
                else if (mag > normalScaleY_ * maxZoomMagnification_)
                {
                    mag = normalScaleY_ * maxZoomMagnification_;
                    zoomScaleTween_.reset();
                    zoomScaleTween_.start(Global::currentTime(), scale, Vector3(mag, mag, 1));
                }
				curMag_ = mag * Global::instance().magnification();
                
                t.setScaling(mag, mag, mag);
            }
            endPos = t.translation();
            for (int i = 0 ; i < 2 ; i++) {
                pressEvents_[i].worldPressPt = pressEvents_[i].curPt;
            }
            //zoomTransTween_.start(Global::currentTime(), startPos, endPos);
            //checkOutside(t);
            //trans_ = t.translation();
            isTouched_ = false;
		//	if (curscenemag != curMag_)
		//	{
				float globalmag = Global::instance().curSceneMagnification();
				Global::instance().setCurSceneMagnification(curMag_);
				Global::instance().setGlobalCurMagnification(curMag_ / normalScaleX_);
				scene()->parentDocument()->documentTemplate()->reInitCurScene(gl_);
				Global::instance().setCurSceneMagnification(globalmag);
		//	}
        }
        handled = true;
	}
    else if(transform().scaling().x > normalScaleX_ && transform().scaling().y > normalScaleY_ && zoomFlag_)
    {
        bool checkChangeScene = false;
		Transform t = transform();
//        if (pressId == 0 || pressId == 1)
//        {
//            Matrix mat = t.computeMatrix();
////            trans_ = Vector3(0,0,0);
//            mat = t.computeMatrix();
//            mat = mat.inverse();
//            if (mat._14 * normalScaleX_ < 0 && hasLeft_)
//            {
//                checkChangeScene = true;
//            }
//            else if(mat._14 * normalScaleX_ > scene_->screenWidth() * normalScaleX_ - (scene_->screenWidth() * normalScaleX_ / t.scaling().x * normalScaleX_) && hasRight_)
//            {
//                checkChangeScene = true;
//            }
//            if(mat._24 * normalScaleY_ < 0 && hasTop_)
//            {
//                checkChangeScene = true;
//            }
//            else if(mat._24 * normalScaleY_ > scene_->screenHeight() * normalScaleY_ - (scene_->screenHeight() * normalScaleY_ / t.scaling().y * normalScaleY_) && hasBottom_)
//            {
//                checkChangeScene = true;
//            }
//        }
//        if (checkChangeScene)
//            handled = false;
//		else
//		{
			//Vector3 startPos = t.translation();
			checkOutside(t);
			//Vector3 endPos = t.translation();
			//zoomTransTween_.reset();
			//zoomTransTween_.start(Global::currentTime(), startPos, endPos);
            handled = true;
//		}
    }
	else 
	if (pressId == pageMovingId_)
	{
		float time = Global::currentTime();
		if (!locked_ && isDraggable())
		{
			if (dragSnap_)
			{
				dragRelease(pressEvent, localPos);
			}
			else
			{
				dragVelHandler_.calcVelocityFromHistory(dragScenePos_, time);
			}
		}
		else if (!locked_ && isScrollable())
		{
			dragVelHandler_.calcVelocityFromHistory(scenePos_, time);	
			
		}

		pageMovingId_ = -1;
	}
	
	//SceneChanger zoom
	pressEvent.pressed = PressEventData::NotPressed;
	
	if(isMultiTouched_)
	{
		if ((pressEvents_[0].pressed == PressEventData::Pressed ||
			pressEvents_[1].pressed == PressEventData::Pressed) &&
			isZoomable())
		{
			isMultiTouched_ = true;
		}else{
			isMultiTouched_ = false;
			pressEvents_.clear();
		}
	}

	dragZooming_ = false;
		
	return handled;
}

void Subscene::start(float docTime)
{
	start(docTime, true);
}

void Subscene::start(float docTime, bool triggerEvent)
{
	SceneObject::start(docTime);
	if (scene_)scene_->start(docTime, triggerEvent);

	//reset scene scroll / drag position
	//don't reset scroll position. 
	//allow the possibility for altering a page's contents before displaying it
	//to user. Resetting would simply undo those changes on start.
	//scenePos_ = Vector2(0, 0);
	//dragScenePos_ = Vector2(0, 0);
	
}

void Subscene::preStart(float docTime)
{
	SceneObject::preStart(docTime);
	if (scene_)scene_->preStart(docTime);

	//reset scene scroll / drag position
	//don't reset scroll position. 
	//allow the possibility for altering a page's contents before displaying it
	//to user. Resetting would simply undo those changes on start.
	//scenePos_ = Vector2(0, 0);
	//dragScenePos_ = Vector2(0, 0);
	
}

void Subscene::initAppObjects()
{
	if (linkWithViewerLevelObjects_ && !displayingAppObjects_ && 
		scene_ && scene_->isCurrentScene())
	{
		this->scene()->setCurContainerObject(this);
		scene_->initAppObjects();
		displayingAppObjects_ = true;
	}

	//doAppObjectOffset();
}

void Subscene::uninitAppObjects()
{
	if (linkWithViewerLevelObjects_ && displayingAppObjects_ &&
		scene_ && scene_->isCurrentScene())
	{
		scene_->uninitAppObjects();
		displayingAppObjects_ = false;
	}

}

void Subscene::stop()
{
	if (scene_)scene_->stop();
/*
	if (affectAppObjectScrolling_)
	{		
		if (displayingAppObjects_)
		{
			scene_->uninitAppObjects();
			displayingAppObjects_ = false;
		}
	}*/
}

bool Subscene::isLoaded() const
{
	return isLoaded_;	
}

bool Subscene::asyncLoadUpdate()
{
	bool everythingLoaded = SceneObject::asyncLoadUpdate();
	if (scene_) 
	{
		//Commenting out. Want to report as loaded, even if Subscene:: child 
		//scene isn't all loaded. This is because, the ImageChanger object 
		//itself will display a loading page, so the parent document template 
		//doesn't have to.
		//everythingLoaded &= scene_->asyncLoadUpdate();

		bool sceneAllLoaded = scene_->asyncLoadUpdate();

		//Once the subscene is loaded we want it to
		//always be reported as loaded until the child scene changes.
		if (!isLoaded_) isLoaded_ = scene_->isLoaded();
	}
		
	return everythingLoaded;
}


bool Subscene::dependsOn(Scene * scene) const
{
	if (!scene_) return false;
	if (scene_ == scene || scene_->dependsOn(scene))
		return true;
	
	return false;
}

void Subscene::dependsOnScenes(std::vector<Scene *> * dependsOnScenes, bool recursive) const
{
	if (scene_)
	{
		dependsOnScenes->push_back(scene_);
		if (recursive) scene_->dependsOnScenes(dependsOnScenes);
	}
}


bool Subscene::isShowing(Scene * scene) const
{
	if (!scene_) return false;
	if (scene_ == scene || scene_->isShowing(scene))
		return true;	
	return false;
}

void Subscene::showingScenes(std::set<Scene *> * showingScenes) const
{
	if (scene_)
	{
		showingScenes->insert(scene_);
		scene_->showingScenes(showingScenes);
	}
}

void Subscene::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");

	writer.write(scene_);

	writer.write(mode_);
	writer.write(windowWidth_);
	writer.write(windowHeight_);

	writer.write(dragVertical_);
	writer.write(maxPlusDrag_);
	writer.write(maxMinusDrag_);
	writer.write(dragSnap_);

	writer.write(transparentBg_);
	writer.write(linkWithViewerLevelObjects_);

	writer.write(cameraMode_);

	writer.write(maxTopDrag_);
	writer.write(maxBotDrag_);
}

void Subscene::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);
	if (scene_) scene_->removeObjRef(this);
	reader.read(scene_);
	if (scene_) 
	{
		scene_->addObjRef(this);
		scene_->setCurContainerObject(this);
	}

	if (version >= 2)
	{
		reader.read(mode_);
		if (mode_ > ModeDraggable) mode_ = ModeFixed;
	}
	else
		mode_ = ModeScrollable;

	if (version < 1)
	{
		windowWidth_ = scene_->screenWidth();
		windowHeight_ = scene_->screenHeight();
	}
	else
	{
		reader.read(windowWidth_);
		reader.read(windowHeight_);
	}

	if (version >= 3 && version < 6)
	{
		float maxUp, maxDown, maxLeft, maxRight;
		reader.read(maxUp);
		reader.read(maxDown);
		reader.read(maxLeft);
		reader.read(maxRight);

		if (maxUp != 0 || maxDown != 0)
		{
			dragVertical_ = 1;
			maxPlusDrag_ = maxDown;
			maxMinusDrag_ = maxUp;
		}
		else
		{
			dragVertical_ = 0;
			maxPlusDrag_ = maxRight;
			maxMinusDrag_ = maxLeft;
		}
	}
	else if (version >= 6)
	{
		if (version >= 8)
			reader.read(dragVertical_);
		else
		{
			bool temp;
			reader.read(temp);
			dragVertical_ = temp;
		}
		reader.read(maxPlusDrag_);
		reader.read(maxMinusDrag_);
		reader.read(dragSnap_);
	}


	if (version >= 4)
	{
		reader.read(transparentBg_);
	}

	if (version >= 5)
	{
		reader.read(linkWithViewerLevelObjects_);
	}

	if (version >= 7)
	{
		reader.read(cameraMode_);
	}
	else
	{
		cameraMode_ = ChildScene;
	}

	if (version >= 9)
	{
		reader.read(maxTopDrag_);
		reader.read(maxBotDrag_);
	}
}



void Subscene::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();
	
	w.writeTag("Scene", w.idMapping().getId(scene_));
	w.writeTag("Mode", (unsigned int)mode_);
	w.writeTag("Width", windowWidth_);
	w.writeTag("Height", windowHeight_);
	w.writeTag("DragVertical", dragVertical_);	
	w.writeTag("MaxPlusDrag", maxPlusDrag_);	
	w.writeTag("MaxMinusDrag", maxMinusDrag_);	
	w.writeTag("DragSnap", dragSnap_);	
	w.writeTag("TransparentBg", transparentBg_);	
	w.writeTag("LinkWithViewerLevelObjects", linkWithViewerLevelObjects_);	
	w.writeTag("CameraMode", (unsigned int)cameraMode_);	
}

void Subscene::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);

	unsigned int val;

	for (xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
		}
		else if (r.getNodeContentIfName(val, curNode, "Scene"))	scene_ = r.idMapping().getScene(val);
		else if (r.getNodeContentIfName(val, curNode, "Mode")) mode_ = (Mode)val;
		else if (r.getNodeContentIfName(windowWidth_, curNode, "Width"));
		else if (r.getNodeContentIfName(windowHeight_, curNode, "Height"));
		else if (r.getNodeContentIfName(dragVertical_, curNode, "DragVertical"));
		else if (r.getNodeContentIfName(maxPlusDrag_, curNode, "MaxPlusDrag"));
		else if (r.getNodeContentIfName(maxMinusDrag_, curNode, "MaxMinusDrag"));
		else if (r.getNodeContentIfName(dragSnap_, curNode, "DragSnap"));
		else if (r.getNodeContentIfName(transparentBg_, curNode, "TransparentBg"));
		else if (r.getNodeContentIfName(linkWithViewerLevelObjects_, curNode, "LinkWithViewerLevelObjects"));
		else if (r.getNodeContentIfName(val, curNode, "CameraMode")) cameraMode_ = (CameraMode)val;
	}

}

bool Subscene::isValid() const
{
	return scene_ && !parentScene_->zBuffer();
}

void Subscene::getSceneObjectsByID(
	const boost::uuids::uuid & type, std::vector<SceneObject *> * objs) const
{
	if (scene_) scene_->getSceneObjectsByID(type, objs);
}

//SceneChanger zoom
void Subscene::resizeText(GfxRenderer * gl)
{
	if (!scene_) return;
	scene_->resizeText(gl);
}
bool Subscene::isMultiTouchEnded() const
{
	return (Global::currentTime() - multiTouchEndTime_ > Global::flickTime());
}

bool Subscene::isTweenWorking() const
{
    return zoomScaleTween_.isHappening() || zoomTransTween_.isHappening();
}

const Vector3 Subscene::operateTranslation(int id) 
{
    PressEventData pressEvent = pressEvents_[id];
    float deltaX = pressEvent.curPt.x - pressEvent.worldPressPt.x;
    float deltaY = pressEvent.curPt.y - pressEvent.worldPressPt.y;
    Vector3 temp = Vector3(deltaX, deltaY, 0) + trans_;

    return temp;
}



void Subscene::checkOutside(Transform & t)
{
	Matrix mat = t.computeMatrix();
	mat = mat.inverse();
	if (mat._14 * normalScaleX_ < 0 )
	{
		Matrix m = t.computeMatrix();
		m = m.inverse();
		m._14 = 0;
		mat = m;
		m = m.inverse();
		t.setMatrix(m, true);
	}
	else if(mat._14 * normalScaleX_ > pageEndLine(t))
	{
		Matrix m = mat;
		m._14 = pageEndLine(t) / normalScaleX_;
		mat = m;
		m = m.inverse();
		t.setMatrix(m, true);
	}
	if(mat._24 * normalScaleY_ < 0 )
	{
		Matrix m = t.computeMatrix();
		m = m.inverse();
		m._24 = 0;
		m = m.inverse();
		t.setMatrix(m, true);
	}
	else if(mat._24 * normalScaleY_ > scene_->screenHeight() * normalScaleY_ - (scene_->screenHeight() * normalScaleY_ / t.scaling().y * normalScaleY_))
	{
		Matrix m = mat;
		m._24 = (scene_->screenHeight() * normalScaleY_ - (scene_->screenHeight() * normalScaleY_ / t.scaling().y * normalScaleY_)) / normalScaleY_;
		m = m.inverse();
		t.setMatrix(m, true);
	}
}

bool Subscene::checkParentHandled(Transform & t)
{
	bool checkHandle = false;
	Matrix mat = t.computeMatrix();
	mat = mat.inverse();

	if (mat._14 * normalScaleX_ < 0 && hasLeft_) // 외각이동 막음
	{
		checkHandle = true;
	}
	else if(mat._14 * normalScaleX_ > scene_->screenWidth() * normalScaleX_ - (scene_->screenWidth() * normalScaleX_ / t.scaling().x * normalScaleX_) && hasRight_)// 외각이동 막음
	{
		checkHandle = true;
	}
	if(mat._24 * normalScaleY_ < 0 && hasTop_) // 외각이동 막음
	{
		checkHandle = true;
	}
	else if(mat._24 * normalScaleY_ > scene_->screenHeight() * normalScaleY_ - (scene_->screenHeight() * normalScaleY_ / t.scaling().y * normalScaleY_) && hasBottom_) // 외각이동 막음
	{
		checkHandle = true;
	}
	return checkHandle;
}

float Subscene::pageEndLine(Transform & t)
{
	return scene_->screenWidth() * normalScaleX_ - (scene_->screenWidth() * normalScaleX_ / t.scaling().x * normalScaleX_);
}


bool Subscene::showPoint(Vector2 & outCoords, Scene * scene, const Vector2 & coords)
{
	if (!scene || scene != scene_) return false;
	
	Vector2 newPos = Vector2(windowWidth_/2.0f, windowHeight_/2.0f) - coords;
	if (mode_ == ModeScrollable) 
	{	
		float minX = windowWidth_ - scene_->screenWidth();
		float minY = windowHeight_ - scene_->screenHeight();
		
		if (newPos.x > 0) newPos.x = 0;		
		if (newPos.x < minX) newPos.x = minX;

		if (newPos.y > 0) newPos.y = 0;
		if (newPos.y < minY) newPos.y = minY;

		setScrollPosition(newPos);
	}
	else if (mode_ == ModeDraggable)
	{

		float minX = 0.0f, maxX = 0.0f;
		float minY = 0.0f, maxY = 0.0f;

		if (dragVertical_ == 1)
		{
			minY = -maxMinusDrag_;
			maxY = maxPlusDrag_;
		}
		else if (dragVertical_ == 0)
		{
			minX = -maxMinusDrag_;
			maxX = maxPlusDrag_;
		}

		if (newPos.x < minX) newPos.x = minX;
		if (newPos.x > maxX) newPos.x = maxX;
		if (newPos.y < minY) newPos.y = minY;
		if (newPos.y > maxY) newPos.y = maxY;

		setDragPosition(newPos);
	}


	Vector2 pos = fromLocalScenePos(coords);

	outCoords = parentTransform() * visualAttrib_.transformMatrix() * Vector3(pos.x, pos.y, 0.0f);
	return true;
}

void Subscene::reSize()
{
	curMag_ = Global::instance().magnification();
}

void Subscene::requestLocalSaveObject()
{
	scene_->requestLocalSaveObject();
}

AppObjectOffset Subscene::textFeaturesOffset() const
{
	AppObjectOffset ret;

	ret.clip = true;		

	Matrix m = parentTransform(); 
	m *= visualAttrib_.transformMatrix();

	ret.clipX = m._14;
	ret.clipY = m._24;
	ret.clipWidth = m._11 * modeWidth();
	ret.clipHeight = m._22 * modeHeight();

	if (isScrollable())
	{
		ret.dx = m._11 * scenePos_.x + m._14;
		ret.dy = m._22 * scenePos_.y + m._24;
		ret.sx = m._11;
		ret.sy = m._22;
	}
	else if (isDraggable())
	{
		ret.dx = m._11 * dragScenePos_.x + m._14;
		ret.dy = m._22 * dragScenePos_.y + m._24;
		ret.sx = m._11;
		ret.sy = m._22;

		ret.clipX = m._11 * dragScenePos_.x + m._14;
		ret.clipY =  m._22 * dragScenePos_.y + m._24;
	}
	else
	{
		ret.dx = m._14;
		ret.dy = m._24;
		ret.sx = m._11;
		ret.sy = m._22;
	}
	return ret;
}
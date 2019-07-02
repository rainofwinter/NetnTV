#include "stdafx.h"
#include "InternalSubscene.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "glUtils.h"
#include "Global.h"
#include "VideoPlayer.h"
#include "Document.h"
#include "DocumentTemplate.h"
#include "Exception.h"
#include "Network.h"
#include "VideoObject.h"

using namespace std;

const Transform & InternalSubscene::transform() const
{
	return visualAttrib_.transform();
}

void InternalSubscene::setTransform(const Transform & transform)
{
	visualAttrib_.setTransform(transform);	
    if (scene_)
        scene_->setCurrentScale(transform.scaling());
}

void InternalSubscene::create()
{
	gl_ = NULL;
	scene_ = 0;	

	windowWidth_ = 320;
	windowHeight_ = 200;
	
	scenePos_ = Vector2(0, 0);

	transparentBg_ = 0;

	scrollTweenDuration_ = 0;

	locked_ = false;
	displayingAppObjects_ = false;

	isLoaded_ = false;

	justReleased_ = false;

	moveDirection_ = Undetermined;
	pageMovingId_ = -1;

	zoomFlag_ = false;
	isMultiTouched_ = false;
    isTouched_ = false;
    trans_ = Vector3(0,0,0);
    
    hasLeft_ = false;
    hasRight_ = false;
    hasTop_ = false;
    hasBottom_ = false;
    maxZoomMagnification_ = 2.0;
    multiTouchEndTime_ = 0;

    networkRequest_ = new NetworkRequest;

	normalScaleX_ = 1;
	normalScaleY_ = 1;

	viewState_ = Normal;
}

InternalSubscene::InternalSubscene()
{
	create();
}


InternalSubscene::~InternalSubscene()
{
	uninit();
	if (scene_) scene_->removeObjRef(this);
	delete networkRequest_;
}

void InternalSubscene::setWindowWidth(const float & width) 
{
	windowWidth_ = width;
}

void InternalSubscene::setWindowHeight(const float & height) 
{
	windowHeight_ = height;	
}

void InternalSubscene::setScene(Scene * const & scene)
{	
	if (scene) scene->setCurContainerObject(NULL);
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
		Global::instance().setCurSceneMagnification(Global::instance().magnification());
		Global::instance().setGlobalCurMagnification(1);
	}
	else 
		isLoaded_ = true;

	if (isZoomable())
        locked_ = false;
	else
		locked_ = true;
}

void InternalSubscene::changeScene(Scene * const & scene)
{
	setScene(scene);
}

void InternalSubscene::init(GfxRenderer * gl, bool firstTime)
{
	if (gl_) uninit();
	gl_ = gl;
	if (scene_) scene_->handleInitBasedOnRefs(firstTime);

	//isLoaded_ should be trivially true if scene_ is null
	isLoaded_ = scene_ == 0;
	//trigger initial update of isLoaded_ field
	asyncLoadUpdate();
}

void InternalSubscene::uninit()
{		
	if (!gl_) return;
	
	isLoaded_ = false;

	gl_ = 0;
	if (scene_)scene_->handleInitBasedOnRefs();
}

float InternalSubscene::subsceneWidth() const
{
	if (!scene_) return 0;
	if (mode_ == ModeScrollable)
		return windowWidth_;
	else
		return scene_->screenWidth();
}

float InternalSubscene::subsceneHeight() const
{
	if (!scene_) return 0;
	if (mode_ == ModeScrollable)
		return windowHeight_;
	else
		return scene_->screenHeight();
}

void InternalSubscene::draw(GfxRenderer * gl) const
{		
	gl->pushMatrix();
	gl->multMatrix(visualAttrib_.transformMatrix());

	drawObject(gl);
	
	gl->popMatrix();
}


void InternalSubscene::drawObject(GfxRenderer * gl) const
{
	if (!scene_) return;
	

	/*
	not isScrollable(). isScrollable() will return 0 if the scroll window is 
	the same size or bigger than the scroll scene. However even in such 
	situations we want clipping to be applied.
	*/
	bool isScrollMode = mode_ == ModeScrollable; 
		
	gl->pushMatrix();

	if (isScrollMode)
		gl->multMatrix(Matrix::Translate(scenePos_.x, scenePos_.y, 0));	
			
	float sw = scene_->screenWidth();
	float sh = scene_->screenHeight();

	//---------------------------------	
	Matrix camMatrix = gl->cameraMatrix();	
	Matrix fullMat = camMatrix * gl->modelMatrix();

	//Assume there are no rotation transforms
	//Assume we are rendering to a rectangular region on screen

	//Vector3 a = fullMat * Vector3(0.0f, 0.0f, 0.0f);
	Vector3 a(
		fullMat._14 / fullMat._44, 
		fullMat._24 / fullMat._44, 
		fullMat._34 / fullMat._44);
	Vector3 b = fullMat * Vector3(sw, 0.0f, 0.0f);
	Vector3 d = fullMat * Vector3(0, sh, 0.0f);

	const static float cEpsilon = 0.0001f;

	//-----------------------------------

	if (!transparentBg_)
	{
		//draw scene background
#ifdef DRAW_SCENE_BACKGROUND
		gl->useColorProgram();
		const Color & bgColor = scene_->bgColor();
		gl->setColorProgramColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
		gl->drawRect(0, 0, sw, sh);
#endif
	}

	bool doStencilClipping = isScrollMode;
	if (doStencilClipping)
	{				
		gl->beginIncStencilDrawing();
		gl->useColorProgram();
		gl->setColorProgramColor(0,0,0,0);
		gl->drawRect(-scenePos_.x, -scenePos_.y, windowWidth_, windowHeight_);
		gl->endStencilDrawing();		
	}


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
		
	if (doStencilClipping)
	{	
		gl->beginDecStencilDrawing();
		gl->useColorProgram();
		gl->setColorProgramColor(0,0,0,0);
		gl->drawRect(-scenePos_.x, -scenePos_.y, windowWidth_, windowHeight_);
		gl->endStencilDrawing();
	}

	gl->popMatrix();
}


void InternalSubscene::endScrollTween()
{
	scrollTweenDuration_ = 0.0f;
}

bool InternalSubscene::isScrollTweenHappening() const 
{
	return scrollTweenDuration_ > 0;
}

void InternalSubscene::startScrollTween(Vector2 * target,
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

bool InternalSubscene::update(float sceneTime)
{
	bool needRedraw = false;
	if (scene_) needRedraw |= scene_->update(sceneTime);
	if (redrawTriggered_)
	{
		needRedraw = true;
		redrawTriggered_ = false;
	}

	bool notPressed = pageMovingId_ == -1;

	if (notPressed && (!dragVelHandler_.isZeroVelocity() || justReleased_))
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
	if (zoomTransTween_.isHappening())
	{
		Vector3 pos = zoomTransTween_.update(sceneTime);
		Transform t = transform();
		t.setTranslation(pos);
		//checkOutside(t);
        
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
		Global::instance().setCurSceneMagnification(mag);
		Global::instance().setGlobalCurMagnification(mag / normalScaleX_);
//		if (viewState_ != Normal)
//		{
//			float windowheight = windowHeight() * t.scaling().y / normalScaleY_;
//			if (scale.x != maxZoomMagnification_*normalScaleX_ && 
//				!yTransTween_.isHappening() && !isScaleGrowing)
//			{
//
//				float landy = (scene_->parentDocument()->height() - windowHeight())/2;
//				yTransTween_.reset();
//				yTransTween_.start(Global::currentTime(), t.computeMatrix()._24, landy);
//			}
//		}

		if (!zoomScaleTween_.isHappening() && (scale.x == normalScaleX_ || scale.y == normalScaleY_)) //&& viewState_ != Normal)
		{
//            Transform t1;
//            t1.setScaling(normalScaleX_, normalScaleY_, 1);
            Matrix m1 = t.computeMatrix().inverse();
//            cout << "m1.14: " << m1._14 << endl;
//            cout << "m1.24: " << m1._24 << endl;
			t.setPivot(0,0,0);
            Matrix m2 = t.computeMatrix().inverse();
//            cout << "m2.14: " << m2._14 << endl;
//            cout << "m2.24: " << m2._24 << endl;
            m2._14 = m1._14;    m2._24 = m1._24;
            m2 = m2.inverse();
            t.setMatrix(m2, true);
//			t.setTranslation(0,0,0);
			setTransform(t);
		}

		needRedraw = true;	
	}
//	if (zoomTransTween_.isHappening())
//	{
//		Vector3 pos = zoomTransTween_.update(sceneTime);
//		Transform t = transform();
//		Vector3 pivot = t.pivot();
//		Vector3 translation = t.translation();
//		//cout << "pivot.x: " << pivot.x << "\tpitovt.y: " << pivot.y << endl;
//		//cout << "translation.x: " << translation.x << "\ttranslation.y: " << translation.y << endl;
//		//cout << "pos.x: " << pos.x << "\tpos.y: " << pos.y << endl;
//		t.setTranslation(pos);
//		checkOutside(t);
//
//		setTransform(t);
//		trans_ = t.translation();
//
//		//        if (!zoomTransTween_.isHappening() && t.scaling().x == normalScale_)
//		//        {
//		//            t.setTranslation(0,0,0);
//		//            setTransform(t);
//		//        }
//
//		needRedraw = true;
//	}
	if (yTransTween_.isHappening())
	{
		float m24 = yTransTween_.update(sceneTime);
		Transform t = transform();
		Matrix m = t.computeMatrix();
		m._24 = m24;
		t.setMatrix(m, true);
		setTransform(t);
	
		if (!yTransTween_.isHappening())
		{
			t.setPivot(0,0,0);
			float posx;
			if (viewState_ == LandscapeLeft)
				posx = 0;
			else if (viewState_ == LandscapeRight)
				posx = windowWidth();
			else if (viewState_ == LandscapeCenter)
				posx = windowWidth()/2;
			else
				posx = 0;
			t.setTranslation(posx, (scene_->parentDocument()->height() - windowHeight())/2,0);
			setTransform(t);
		}
	}
//    else if(/*transform().scaling().x > normalScale_ &&*/ !isTouched_ )//&& isZoomable())
//    {
//        bool checkTrans = false;
//        
//        Transform t = transform();
//        Matrix mat = t.computeMatrix();
//        mat = mat.inverse();
//        if (mat._14 * normalScale_ < 0)
//        {
//            Matrix m = t.computeMatrix();
//            m._14 = 0;
//            mat = m.inverse();
//            t.setMatrix(m, true);
//            checkTrans = true;
//        }
//        else if(mat._14 * normalScale_ > scene_->screenWidth() * normalScale_ - (scene_->screenWidth() * normalScale_ / t.scaling().x * normalScale_))
//        {
//            Matrix m = mat;
//            m._14 = (scene_->screenWidth() * normalScale_ - (scene_->screenWidth() * normalScale_ / t.scaling().x * normalScale_)) / normalScale_;
//            mat = m;
//            m = m.inverse();
//            t.setMatrix(m, true);
//            checkTrans = true;
//        }
//        if(mat._24 * normalScale_ < 0)
//        {
//            Matrix m = t.computeMatrix();
//            m._24 = 0;
//            t.setMatrix(m, true);
//            checkTrans = true;
//        }
//        else if(mat._24 * normalScale_ > scene_->screenHeight() * normalScale_ - (scene_->screenHeight() * normalScale_ / t.scaling().y * normalScale_))
//        {
//            Matrix m = mat;
//            m._24 = (scene_->screenHeight() * normalScale_ - (scene_->screenHeight() * normalScale_ / t.scaling().y * normalScale_)) / normalScale_;
//            m = m.inverse();
//            t.setMatrix(m, true);
//            checkTrans = true;
//        }
//        if (checkTrans)
//        {
//            zoomTransTween_.reset();
//            zoomTransTween_.start(Global::currentTime(), transform().translation(), t.translation());
//			needRedraw = true;
//        }
//    }

	
	
	//doAppObjectOffset();

	justReleased_ = false;
	return needRedraw;
}

AppObjectOffset InternalSubscene::appObjectOffset() const
{
	AppObjectOffset ret;	
	ret.clip = true;		 
	Matrix m = visualAttrib_.transformMatrix();

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
	else
	{
		ret.dx = m._14;
		ret.dy = m._24;
		ret.sx = m._11;
		ret.sy = m._22;
	}


	return ret;
}
/*
void InternalSubscene::doAppObjectOffset()
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


float InternalSubscene::sceneWidth() const
{	
	if (scene_) return scene_->screenWidth();
	else return 0;
}

float InternalSubscene::sceneHeight() const
{
	if (scene_) return scene_->screenHeight();
	else return 0;
}

float InternalSubscene::modeWidth() const
{
	if (isScrollable())
		return windowWidth_;
	else if (scene_)
		return scene_->screenWidth();
	else return 0;
}

void InternalSubscene::setScrollPosition(const Vector2 & pos)
{
	scenePos_.x = floor(pos.x);
	scenePos_.y = floor(pos.y);
	resetScrollTween();
	dragVelHandler_.clearMoveHistory();	
}

float InternalSubscene::modeHeight() const
{
	if (isScrollable())
		return windowHeight_;
	else if (scene_)
		return scene_->screenHeight();
	else return 0;
}


bool InternalSubscene::isScrollable() const
{
	return scene_ && 
		(
		windowWidth_ < scene_->screenWidth() || 
		windowHeight_ < scene_->screenHeight()
		) &&
		mode_ == ModeScrollable
		;
}

bool InternalSubscene::clickEvent(const Vector2 & mousePos, int pressId)
{
	if (!isLoaded_) return false;
	bool handled = false;
	if (!scene_) return handled;

	Vector2 localPos = processCoords(mousePos);
	Vector2 localScenePos = toLocalScenePos(localPos);
	
	handled |= scene_->clickEvent(localScenePos, pressId);
	//Transform t;
	//t = transform();
	//t.setScaling(2,2,2);
	//setTransform(t);
	return handled;
}

bool InternalSubscene::doubleClickEvent(const Vector2 & mousePos, int pressId)
{
	if (!isLoaded_) return false;
	bool handled = false;
	if (!scene_) return handled;

	Vector2 localPos = processCoords(mousePos);
	Vector2 localScenePos = toLocalScenePos(localPos);

	if (isZoomable())
	{
		Transform t;
		t = transform();
		Vector3 scale = t.scaling();
		//cout << "mousePos.x: " << mousePos.x << endl;
		//cout << "windowwidth: " << windowWidth_ << endl;
		if (viewState_ == LandscapeCenter)
			pivot_ = Vector3(windowWidth_, mousePos.y, 0); ////수정사항
		else
			pivot_ = Vector3(mousePos.x, mousePos.y, 0);
		Matrix mat = t.computeMatrix();
		mat = mat.inverse();
		pivot_ = mat * pivot_;
		t.setPivot(pivot_);
		mat = t.computeMatrix();
		mat = mat.inverse();
		setTransform(t);
		// 임
		float curscenemag = Global::instance().curSceneMagnification();
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

			//loadZoomImageRequest();
		}
		Global::instance().setCurSceneMagnification(mag);
		Global::instance().setGlobalCurMagnification(mag / normalScaleX_);
		if (curscenemag != mag)
		{
			scene()->parentDocument()->documentTemplate()->reInitCurScene(gl_);
		}
	}

	handled |= scene_->doubleClickEvent(localScenePos, pressId);
	if (handled) redrawTriggered_ = true;

	return handled;
}

Vector2 InternalSubscene::toLocalScenePos(const Vector2 & localPos) const
{
	if (mode_ == ModeScrollable) return localPos - scenePos_;
	else return localPos;
}

bool InternalSubscene::keyPressEvent(int keyCode)
{
	bool handled = false;
	if (scene_) 
		handled = scene_->keyPressEvent(keyCode);
	return handled;
}

bool InternalSubscene::keyReleaseEvent(int keyCode)
{
	bool handled = false;
	if (scene_)
		handled = scene_->keyReleaseEvent(keyCode);
	return handled;
}

bool InternalSubscene::pressEvent(const Vector2 & startPos, int pressId)
{	
	PressEventData & pressEvent = pressEvents_[pressId];
	pressEvent.pressed = PressEventData::NotPressed;
	if (!isLoaded_) return false;

	bool handled = false;
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
	dragVelHandler_.clearMoveHistory();
	resetScrollTween();
	moveDirection_ = Undetermined;

	if (locked_)
	{
		zoomFlag_ = scene_->zoom();
		if (isZoomable())
			locked_ = false;
		else
			locked_ = true;
	}

	if ((isScrollable() || isZoomable()) && !handled && !locked_)
	{			
		handled = true;
	}
	Vector2 t1, t2;
    if (pressId == 0 || pressId == 1)
    {
        isTouched_ = true;
        trans_ = transform().translation();
        if (pressEvents_[0].pressed == PressEventData::Pressed &&
            pressEvents_[1].pressed == PressEventData::Pressed &&
            isZoomable())
        {
            t1 = pressEvents_[0].curPt;
            t2 = pressEvents_[1].curPt;
            isMultiTouched_ = true;
            Vector2 pivot = t1 + t2;
            pivot /= 2;
            if (viewState_  == LandscapeCenter)
                pivot_ = Vector3(windowWidth_, pivot.y, 0);
            else
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
        }
    }
    
    


	pageMovingId_ = pressId;
	if (!handled) pressEvent.pressed = PressEventData::PassedToParent;
	return handled;
}

bool InternalSubscene::moveEvent(const Vector2 & pos, int pressId)
{
	PressEventData & pressEvent = pressEvents_[pressId];
	if (!isLoaded_) return false;
	bool handled = false;
	if (!scene_) return handled;

	if (pressEvent.pressed == PressEventData::PassedToParent) return false;
    
	Vector2 localPos = processCoords(pos);
	Vector2 localScenePos = toLocalScenePos(localPos);
    

	handled |= scene_->moveEvent(localScenePos, pressId);

	//short circuit - don't do anything if the child scene processed the event
	//or if locked
	if (handled) return true;
    //if (isMultiTouched_)
    if (pressEvents_[0].pressed == PressEventData::Pressed &&
        pressEvents_[1].pressed == PressEventData::Pressed &&
        isZoomable())
	{   
		isMultiTouched_ = true;
	}
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
			Global::instance().setCurSceneMagnification(mag);
			Global::instance().setGlobalCurMagnification(mag / normalScaleX_);
            //  빈공간 안보이게하기
            checkOutside(t);
			setTransform(t);
		}
        return true;
	}
    else if(transform().scaling().x > normalScaleX_ && transform().scaling().y > normalScaleY_ && isZoomable())
    {
        bool checkParentHandle;
        Vector2 dir = pos - pressEvent.curPt;
        pressEvent.curPt = pos;
        Transform t = transform();
        t.setTranslation(t.translation() + Vector3(dir.x, dir.y, 0));
		checkParentHandle = checkParentHandled(t);

		//checkOutside(t);
		setTransform(t);

        /*if (checkParentHandle)
            return false;
        else*/
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


	if (isScrollable() && !isMultiTouched_)
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

	return handled;
}

Vector2 InternalSubscene::boundToWindow(const Vector2 & scenePos) const
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

Vector2 InternalSubscene::processCoords(const Vector2 & screenCoords)
{
	//TODO make sure this is correct
	Matrix invTrans = visualAttrib_.transformMatrix().inverse();

	Vector3 out(screenCoords.x, screenCoords.y, 0.0f);

	Vector3 invClickPt = invTrans * out;
	return Vector2(invClickPt.x, invClickPt.y);
}

bool InternalSubscene::releaseEvent(const Vector2 & startPos, int pressId)
{
	PressEventData & pressEvent = pressEvents_[pressId];

	if (!isLoaded_) return false;
	bool handled = false;
	if (!scene_) return handled;

	bool passedToParent = pressEvent.pressed == PressEventData::PassedToParent;
	justReleased_ = true;
	
	if (passedToParent) return false;
	
	Vector2 localPos = processCoords(startPos);
	Vector2 localScenePos = toLocalScenePos(localPos);

	handled |= scene_->releaseEvent(localScenePos, pressId);
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
		isMultiTouched_ = false;
		pressEvents_.clear();
        return true;
    }
	
//	if (isMultiTouched_ && !handled)
    if (pressEvents_[0].pressed == PressEventData::Pressed &&
        pressEvents_[1].pressed == PressEventData::Pressed &&
        isZoomable())
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
			float curscenemag = Global::instance().curSceneMagnification();
            if (scale.x <= normalScaleX_ && scale.y <= normalScaleY_)
            {
                setTransform(t);
                zoomScaleTween_.reset();
                zoomScaleTween_.start(Global::currentTime(), scale, Vector3(normalScaleX_, normalScaleY_, 1));                
                
                t.setScaling(normalScaleX_, normalScaleY_, 1);
                //t.setTranslation(0,0,0);
				Global::instance().setCurSceneMagnification(normalScaleX_);
				Global::instance().setGlobalCurMagnification(1);
            }
            else 
            {
                //loadZoomImageRequest();
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
				Global::instance().setCurSceneMagnification(mag);
                Global::instance().setGlobalCurMagnification(mag / normalScaleX_);

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
		//	if (curscenemag != Global::instance().curSceneMagnification())
		//	{
				scene()->parentDocument()->documentTemplate()->reInitCurScene(gl_);
		//	}
        }
        handled = true;
	}
    else if(transform().scaling().x > normalScaleX_ && transform().scaling().y > normalScaleY_ && isZoomable())
    {
        bool checkChangeScene = false;
		 Transform t = transform();
        if (pressId == 0 || pressId == 1)
        {
            Matrix mat = t.computeMatrix();
//            trans_ = Vector3(0,0,0);
            mat = t.computeMatrix();
            mat = mat.inverse();
            if (mat._14 * normalScaleX_ < 0 && hasLeft_)
            {
                checkChangeScene = true;
            }
            else if(mat._14 * normalScaleX_ > scene_->screenWidth() * normalScaleX_ - (scene_->screenWidth() * normalScaleX_ / t.scaling().x * normalScaleX_) && hasRight_)
            {
                checkChangeScene = true;
            }
            if(mat._24 * normalScaleY_ < 0 && hasTop_)
            {
                checkChangeScene = true;
            }
            else if(mat._24 * normalScaleY_ > scene_->screenHeight() * normalScaleY_ - (scene_->screenHeight() * normalScaleY_ / t.scaling().y * normalScaleY_) && hasBottom_)
            {
                checkChangeScene = true;
            }
        }
        if (checkChangeScene)
            handled = false;
		else
		{
			Vector3 startPos = t.translation();
			checkOutside(t);
			Vector3 endPos = t.translation();
			zoomTransTween_.reset();
			zoomTransTween_.start(Global::currentTime(), startPos, endPos);
            handled = true;
		}
    }
	else if (pressId == pageMovingId_)
	{
		float time = Global::currentTime();
		if (!locked_ && isScrollable())
		{
			dragVelHandler_.calcVelocityFromHistory(scenePos_, time);	
			
		}
		pageMovingId_ = -1;
	}
    
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

	return handled;
}

void InternalSubscene::start(float docTime)
{
	start(docTime, true);
}

void InternalSubscene::start(float docTime, bool triggerEvent)
{
	startTime_ = docTime;

	if (scene_)
	{
		scene_->start(docTime, triggerEvent);
		if (!scene_->trackingUrl().empty())
		{
			try {
				networkRequest_->start(scene_->trackingUrl());
			} catch (...) {}			
		}
	}

	//reset scene scroll / drag position
	//don't reset scroll position. 
	//allow the possibility for altering a page's contents before displaying it
	//to user. Resetting would simply undo those changes on start.
	//scenePos_ = Vector2(0, 0);
	//dragScenePos_ = Vector2(0, 0);
	
}

void InternalSubscene::preStart(float docTime)
{
	if (scene_)
	{
		scene_->preStart(docTime);
	}
}

void InternalSubscene::initAppObjects()
{
	if (!displayingAppObjects_ && 
		scene_ && scene_->isCurrentScene())
	{
		scene_->initAppObjects();
		displayingAppObjects_ = true;
	}

	//doAppObjectOffset();
}

void InternalSubscene::uninitAppObjects()
{
	if (displayingAppObjects_ &&
		scene_ && scene_->isCurrentScene())
	{
		scene_->uninitAppObjects();
		displayingAppObjects_ = false;
	}

}

void InternalSubscene::stop()
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

bool InternalSubscene::isLoaded() const
{
	return isLoaded_;	
}

bool InternalSubscene::asyncLoadUpdate()
{
//    if (normalScale_ != Global::instance().magnification())
//        normalScale_ = Global::instance().magnification();
    /*if (scene())
    {
        normalScaleX_ = scene()->parentDocument()->width() / scene()->screenWidth();
        normalScaleY_ = scene()->parentDocument()->height() / scene()->screenHeight();
    }
    else
    {
        normalScaleX_ = Global::instance().magnification();
        normalScaleY_ = Global::instance().magnification();
    }*/
	//bool everythingLoaded = true;
	bool sceneAllLoaded = false;;
	if (scene_) 
	{
		//Commenting out. Want to report as loaded, even if child 
		//scene isn't all loaded. This is because, the ImageChanger object 
		//itself will display a loading page, so the parent document template 
		//doesn't have to.
		//everythingLoaded &= scene_->asyncLoadUpdate();

		sceneAllLoaded = scene_->asyncLoadUpdate();

		//Once the subscene is loaded we want it to
		//always be reported as loaded until the child scene changes.
		if (!isLoaded_) isLoaded_ = scene_->isLoaded();
	}
		
	return sceneAllLoaded;
}


bool InternalSubscene::dependsOn(Scene * scene) const
{
	if (!scene_) return false;
	if (scene_ == scene || scene_->dependsOn(scene))
		return true;
	
	return false;
}

void InternalSubscene::dependsOnScenes(std::vector<Scene *> * dependsOnScenes, bool recursive) const
{
	if (scene_)
	{
		dependsOnScenes->push_back(scene_);
		if (recursive) scene_->dependsOnScenes(dependsOnScenes);
	}
}


bool InternalSubscene::isShowing(Scene * scene) const
{
	if (!scene_) return false;
	if (scene_ == scene || scene_->isShowing(scene))
		return true;	
	return false;
}

void InternalSubscene::showingScenes(std::set<Scene *> * showingScenes) const
{
	if (scene_)
	{
		showingScenes->insert(scene_);
		scene_->showingScenes(showingScenes);
	}
}

void InternalSubscene::resizeText(GfxRenderer * gl)
{
	if (!scene_) return;
	scene_->resizeText(gl);
}
bool InternalSubscene::isMultiTouchEnded() const
{
	return (Global::currentTime() - multiTouchEndTime_ > Global::flickTime());
}

bool InternalSubscene::isTweenWorking() const
{
    return zoomScaleTween_.isHappening() || zoomTransTween_.isHappening();
}
const Vector3 InternalSubscene::operateTranslation(int id) 
{
    PressEventData pressEvent = pressEvents_[id];
    float deltaX = pressEvent.curPt.x - pressEvent.worldPressPt.x;
    float deltaY = pressEvent.curPt.y - pressEvent.worldPressPt.y;
    Vector3 temp = Vector3(deltaX, deltaY, 0) + trans_;

    return temp;
}

void InternalSubscene::checkOutside(Transform & t)
{
	Matrix mat = t.computeMatrix();
	mat = mat.inverse();
	if (viewState_ == LandscapeCenter)
	{
		float windowwidth = windowWidth() * t.scaling().x / normalScaleX_;
		float landx = (scene_->parentDocument()->width() - windowwidth)/2;
        float documentHeight = scene_->parentDocument()->height();
		
		//cout << "height: " << windowheight << endl;
		//cout << "landy: " << landy << endl;

//		cout << "mat._24: " << mat._24 << endl;
//		cout << "sceneHeight() - (sceneWidth() / t.scaling().y): " <<  sceneHeight() - (sceneWidth() / t.scaling().y) << endl;
//		cout << "sceneHeight(): " << sceneHeight() << endl;
//		cout << "windowHeight_: " << windowHeight_ << endl;
//		cout << "normalScaleY_: " << normalScaleY_ << endl;
//		cout << "t.scaling().y: " <<  t.scaling().y << endl;
//        cout << "sceneHeight() * t.scaling().y : " <<  sceneHeight() * t.scaling().y  << endl;
//        cout << "documentHeight: " << documentHeight << endl;
//        cout << "(documentHeight - windowHeight_)/2/normalScaleY_: " << (documentHeight - windowHeight_)/2/normalScaleY_ << endl;

        if (documentHeight - windowHeight_ == 0)
        {
            if (sceneHeight() * t.scaling().y > documentHeight)
            {
                if (mat._24 < 0)
                    mat._24 = 0;
                else if (mat._24 > scene_->screenHeight() - scene_->screenHeight() / t.scaling().y * normalScaleY_ )
                    mat._24 = scene_->screenHeight() - scene_->screenHeight() / t.scaling().y * normalScaleY_ ;
            }
        }
        else if (sceneHeight() * t.scaling().y > documentHeight)
        {
            if (mat._24 < 0)
                mat._24 = 0;
            else if (mat._24 > sceneHeight() - (sceneWidth() / t.scaling().y))
                mat._24 = sceneHeight() - (sceneWidth() / t.scaling().y);
        }
        else
            mat._24 = -(documentHeight - windowHeight_)/2/normalScaleY_;

		mat = mat.inverse();
		mat._14 = landx;
		
		t.setMatrix(mat, true);
	}
	else
	{
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
	    if (viewState_ == Normal)
        {
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
        else
        {
            float documentHeight = scene_->parentDocument()->height();
            Matrix m = t.computeMatrix().inverse();
            if (documentHeight - windowHeight_ == 0)
            {
                if (sceneHeight() * t.scaling().y > documentHeight)
                {
                    if (m._24 < 0)
                        m._24 = 0;
                    else if (m._24 > scene_->screenHeight() - scene_->screenHeight() / t.scaling().y * normalScaleY_ )
                        m._24 = scene_->screenHeight() - scene_->screenHeight() / t.scaling().y * normalScaleY_ ;
                }
            }
            else if (sceneHeight() * t.scaling().y > documentHeight)
            {
                if (m._24 < 0)
                    m._24 = 0;
                else if (m._24 > sceneHeight() - (sceneWidth() / t.scaling().y))
                    m._24 = sceneHeight() - (sceneWidth() / t.scaling().y);
            }
            else
                m._24 = -(documentHeight - windowHeight_)/2/normalScaleY_;
            m = m.inverse();
            t.setMatrix(m, true);
        }
	}
}

//void InternalSubscene::checkTranslateLandscape(Transform & t)
//{
//	Vector3 tr = t.translation();
//	Matrix m = t.computeMatrix().inverse();
//	float landy = (scene_->parentDocument()->height() - windowHeight())/2;
//
//	//if (m._24 > -landy)
//		m._24 = -landy;
//	m = m.inverse();
//	t.setMatrix(m, true);
//	zoomTransTween_.reset();
//	zoomTransTween_.start(Global::currentTime(), tr, t.translation());
//}

void InternalSubscene::loadZoomImageRequest()
{
    std::vector<SceneObject *> images;
    static boost::uuids::uuid type = Image().type();
    scene_->getSceneObjectsByID(type, &images);
    BOOST_FOREACH(SceneObject * obj, images)
    {
        Image* img = (Image*)obj;
        img->requestZoom();
    }
}

bool InternalSubscene::checkParentHandled(Transform & t)
{
	bool checkHandle = false;
	Matrix mat = t.computeMatrix();
	mat = mat.inverse();
	if (viewState_ == Normal)
	{
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
	}
	else if (viewState_ == LandscapeLeft)
	{
		if (mat._14 != 0)
			checkHandle = true;
		//if (mat._14 < 0 && hasLeft_) // 외각이동 막음
		//{
		//	checkHandle = true;
		//}
		//else if(mat._14 - scene_->screenWidth() > scene_->screenWidth() - (scene_->screenWidth()  / t.scaling().x * normalScaleX_) && hasRight_)// 외각이동 막음
		//{
		//	checkHandle = true;
		//}
		if(mat._24 * normalScaleY_ < 0 && hasTop_) // 외각이동 막음
		{
			checkHandle = true;
		}
		else if(mat._24 * normalScaleY_ > scene_->screenHeight() * normalScaleY_ - (scene_->screenHeight() * normalScaleY_ / t.scaling().y * normalScaleY_) && hasBottom_) // 외각이동 막음
		{
			checkHandle = true;
		}
	}
	else if (viewState_ == LandscapeRight)
	{
		//if (mat._14 < 0 && hasLeft_) // 외각이동 막음
		//{
		//	checkHandle = true;
		//}
		//else if(mat._14 > scene_->screenWidth() - (scene_->screenWidth()  / t.scaling().x *normalScaleX_) && hasRight_)// 외각이동 막음
		//{
		//	checkHandle = true;
		//}
		if (mat._14 != 0)
			checkHandle = true;
		if(mat._24 * normalScaleY_ < 0 && hasTop_) // 외각이동 막음
		{
			checkHandle = true;
		}
		else if(mat._24 * normalScaleY_ > scene_->screenHeight() * normalScaleY_ - (scene_->screenHeight() * normalScaleY_ / t.scaling().y * normalScaleY_) && hasBottom_) // 외각이동 막음
		{
			checkHandle = true;
		}
	}
	else if (viewState_ == LandscapeCenter)
	{
		if (mat._14 != 0)
			checkHandle = true;
		//cout << "invm14: " << mat._14 << "\t";
		//cout << "invm24: " << mat._24 << "\n";
		//Matrix m = mat.inverse();
		//cout << "m14: " << m._14 << "\t";
		//cout << "m24: " << m._24 << "\n";
		//if (mat._14 < 0 && hasLeft_) // 외각이동 막음
		//{
		//	checkHandle = true;
		//}
		//else if(mat._14 > scene_->screenWidth() - (scene_->screenWidth()  / t.scaling().x *normalScaleX_) && hasRight_)// 외각이동 막음
		//{
		//	checkHandle = true;
		//}
		if(mat._24 * normalScaleY_ < 0 && hasTop_) // 외각이동 막음
		{
			checkHandle = true;
		}
		else if(mat._24 * normalScaleY_ > scene_->screenHeight() * normalScaleY_ - (scene_->screenHeight() * normalScaleY_ / t.scaling().y * normalScaleY_) && hasBottom_) // 외각이동 막음
		{
			checkHandle = true;
		}
	}
	return checkHandle;
}

float InternalSubscene::pageEndLine(Transform & t)
{
	return scene_->screenWidth() * normalScaleX_ - (scene_->screenWidth() * normalScaleX_ / t.scaling().x * normalScaleX_);
}

AppObjectOffset InternalSubscene::textFeaturesOffset() const
{
	AppObjectOffset ret;	
	ret.clip = true;		 
	Matrix m = visualAttrib_.transformMatrix();

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
	else
	{
		ret.dx = m._14;
		ret.dy = m._24;
		ret.sx = m._11;
		ret.sy = m._22;
	}


	return ret;
}
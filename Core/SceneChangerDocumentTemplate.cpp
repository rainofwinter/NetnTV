#include "stdafx.h"
#include "InternalSubscene.h"
#include "SceneChangerDocumentTemplate.h"
#include "Mesh.h"
#include "Image.h"
#include "Texture.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Xml.h"
#include "Scene.h"
#include "Reader.h"
#include "Camera.h"
#include "GLUtils.h"
#include "ElementMapping.h"
#include "FileUtils.h"
#include "Document.h"
#include "PageChangedEvent.h"
#include "Global.h"
#include "Root.h"
#include "PageChangingEvent.h"
#include "LoadingPage.h"
#include "ScriptProcessor.h"

#include "VideoPlayer.h"
#include "AudioPlayer.h"

using namespace std;

JSClass jsSceneChangerTemplateClass = InitClass(
	"SceneChangerTemplate", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, 0, 0);


JSBool SceneChangerTemplate_changeScene(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneChangerDocumentTemplate * thisObj = 
		(SceneChangerDocumentTemplate *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsScene;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsScene)) return JS_FALSE;
	
	if (JS_GetPrototype(cx, jsScene) == s->sceneProto())
	{
		Scene * scene = (Scene *)JS_GetPrivate(cx, jsScene);
		thisObj->sceneChangeTo(scene);
	}

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool SceneChangerTemplate_changeSceneIndex(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneChangerDocumentTemplate * thisObj = 
		(SceneChangerDocumentTemplate *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	uint32 jsIndex;
	if (!JS_ConvertArguments(cx, argc, argv, "u", &jsIndex)) return JS_FALSE;
	thisObj->sceneChangeTo((int)jsIndex);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool SceneChangerTemplate_addEventListener(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneChangerDocumentTemplate * d = 
		(SceneChangerDocumentTemplate *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsHandler;
	jschar * jsEvent;
	if (!JS_ConvertArguments(cx, argc, argv, "Wo", &jsEvent, &jsHandler)) return JS_FALSE;

	if (!JS_ObjectIsFunction(cx, jsHandler))
	{
		JS_ReportError(cx, "SceneChangerTemplate::addEventListener - Parameter must be a function");
		return JS_FALSE;
	}

	std::string event = jsCharStrToStdString(jsEvent);

	bool ret = d->addEventListener(event, jsHandler);
	if (!ret)
	{
		JS_ReportError(cx, "SceneChangerTemplate::addEventListener - Invalid event type");
		return JS_FALSE;
	}

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSFunctionSpec SceneChangerTemplateFuncs[] = {
	JS_FS("changeScene", SceneChangerTemplate_changeScene, 1, 0),
	JS_FS("changeSceneIndex", SceneChangerTemplate_changeSceneIndex, 1, 0),
	JS_FS("addEventListener", SceneChangerTemplate_addEventListener, 2, 0),
    JS_FS_END
};

///////////////////////////////////////////////////////////////////////////////
JSObject * SceneChangerDocumentTemplate::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseDocumentTemplateProto(), &jsSceneChangerTemplateClass,
		0, 0, 0, SceneChangerTemplateFuncs, 0, 0);

	return proto;
}

JSObject * SceneChangerDocumentTemplate::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsSceneChangerTemplateClass, s->documentTemplateProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);
	}

	return scriptObject_;
}


void SceneChangerDocumentTemplate::create()
{
	init_ = false;
	width_ = 768;
	height_ = 1024;	
	redrawTriggered_ = false;

	subscene_.reset(new InternalSubscene);
	subsceneLeft_.reset(new InternalSubscene);
	subsceneRight_.reset(new InternalSubscene);
	
	pageTweenEndPos_ = Vector3(0, 0, 0);
	pageTweenStartTime_ = pageTweenDuration_ = 0;

	startPagePos_ = Vector3(0, 0, 0);
	allowDrag_ = true;


	for (int i = 0; i < 3; ++i)
		subSceneBackup_.push_back(new InternalSubscene);
	
	resetPageTween();

	pageChangingMsg_ = "pageChanging";

	pageIndex_ = -1;
	displayingAppObjects_ = false;

	//loading page
	loadingPage_.reset(new LoadingScreen);
	curPageStarted_ = false;
	pageMovingId_ = -1;
	sceneCheck_ = false;

	resetComplete_ = false;
	searchQ_.clear();
}

void SceneChangerDocumentTemplate::doCurPageStart()
{
	curPageStarted_ = false; //schedule a start when the page finishes loading	
	//force refresh of isLoaded flag
	//this helps prevents glitches on next draw call
	if (subscene_->isInit()) subscene_->asyncLoadUpdate(); 	
}

void SceneChangerDocumentTemplate::setDocument(Document * document)
{
	DocumentTemplate::setDocument(document);
}

void SceneChangerDocumentTemplate::setSize(int width, int height, bool resizeGl)
{
	width_ = (float)width;
	height_ = (float)height;

	camera_.SetTo2DArea(width_/2, height_/2, width_, height_, 60.0f, width_/height_);

	setSubsceneScaling();
	setCurPagePos(subscene_->transform().translation());

	reInitCurScene(document_->renderer());

	if (resizeGl)
	{
		loadingPage_->init(document_->renderer(), width, height);
		static boost::uuids::uuid type = SceneChanger().type();
		for(int i = 0 ; i < document_->scenes().size() ; i++)
		{
			std::vector<SceneObject *> scenechanger;
			document_->scenes()[i]->getSceneObjectsByID(type, &scenechanger);
			BOOST_FOREACH(SceneObject * obj, scenechanger)
			{
				SceneChanger* sc = (SceneChanger*)obj;
				sc->reSize();
			}
		}
	}
}

Scene * SceneChangerDocumentTemplate::curScene() const
{
	return subscene_->scene();
}

Scene * SceneChangerDocumentTemplate::prevScene() const
{
	if (pageIndex_ <= 0) return NULL;
	return scenes_[pageIndex_ - 1];
}

Scene * SceneChangerDocumentTemplate::nextScene() const
{
	if (pageIndex_ >= scenes_.size() - 1) return NULL;
	return scenes_[pageIndex_ + 1];
}

void SceneChangerDocumentTemplate::getScenes(std::vector<Scene *> & scenes) const
{
	scenes.clear();
	scenes.insert(scenes.end(), scenes_.begin(), scenes_.end());	
}

SceneChangerDocumentTemplate::SceneChangerDocumentTemplate()
{
	create();
}

SceneChangerDocumentTemplate::~SceneChangerDocumentTemplate()
{
	uninit();	
	BOOST_FOREACH(InternalSubscene * subscene, subSceneBackup_) delete subscene;
}


void SceneChangerDocumentTemplate::init(GfxRenderer * gl)
{
	if (init_) uninit();

	vector<InternalSubscene *> subscenes;
	subscenes.push_back(subscene_.get());
	subscenes.push_back(subsceneLeft_.get());
	subscenes.push_back(subsceneRight_.get());

	BOOST_FOREACH(InternalSubscene * subscene, subscenes)
	{
		subscene->setMode(InternalSubscene::ModeFixed);
		subscene->setLocked(true);		
		subscene->init(gl);
	}


	activePageImg_.reset(new Texture);
	inactivePageImg_.reset(new Texture);

	//activePageImg_->init("c:\\active.png");
	//inactivePageImg_->init("c:\\inactive.png");

	activePageImg_->init(gl, activeMarkerFileName_, Texture::UncompressedRgba32);
	inactivePageImg_->init(gl, inactiveMarkerFileName_, Texture::UncompressedRgba32);

	BOOST_FOREACH(InternalSubscene * subscene, subSceneBackup_) 
	{
		subscene->init(gl);
	}	

	loadingPage_->init(gl, (int)width_, (int)height_);

#ifdef WATERMARK
	sceneChangerCheck(scenes_[0]);
	sceneCheck_ = true;
	sceneChangerCheck(scenes_[scenes_.size()-1]);
#endif
	
	init_ = true;
}

void SceneChangerDocumentTemplate::sceneChangerCheck(Scene * scene)
{
	std::vector<SceneObject *> sceneObject;
	static boost::uuids::uuid type = SceneChanger().type();
	scene->getSceneObjectsByID(type, &sceneObject);
	if(!sceneObject.empty())
	{
		BOOST_FOREACH(SceneObject * obj, sceneObject)
		{		
			if(obj->parent() == scene->root())
			{
				SceneChanger* sceneChanger = (SceneChanger*)obj;
				if(!sceneChanger->scenes().empty())
					if(!sceneCheck_)
						sceneChangerCheck(sceneChanger->scenes()[0]);
					else if(sceneCheck_)
						sceneChangerCheck(sceneChanger->scenes().back());
				else 
					imageChangerCheck(scene);
			}
		}
	}else{
		scene->waterMark();
	}
}

void SceneChangerDocumentTemplate::imageChangerCheck(Scene * scene)
{
	std::vector<SceneObject *> sceneObject;
	static boost::uuids::uuid type = ImageChanger().type();
	scene->getSceneObjectsByID(type, &sceneObject);
	if(!sceneObject.empty())
	{
		BOOST_FOREACH(SceneObject * obj, sceneObject)
		{
			if(obj->parent() == scene->root())
			{
				ImageChanger* imageChanger = (ImageChanger *)obj;
				if(!imageChanger->fileNames().empty())
				{
					for (int j = 0 ; j <imageChanger->pageIndex() ; j++)
					{
						if(!sceneCheck_)
						{
							if (j == 0)
								scene->waterMark();
						} else if(sceneCheck_){
							if(j == imageChanger->pageIndex())
								scene->waterMark();
						}
					}
				}
				else
					scene->waterMark();
			}
		}
	}else{
		scene->waterMark();
	}
}

void SceneChangerDocumentTemplate::uninit()
{
	if (!init_) return;
	subscene_->uninit();
	subsceneLeft_->uninit();
	subsceneRight_->uninit();

	BOOST_FOREACH(InternalSubscene * s, subSceneBackup_)
	{
		s->uninit();
	}
	loadingPage_->uninit();
	init_ = false;
}

Scene * SceneChangerDocumentTemplate::pageLeft() const
{
	if (pageIndex_ == 0) return 0;
	return scenes_[pageIndex_ - 1];
}

Scene * SceneChangerDocumentTemplate::pageRight() const
{
	if (pageIndex_ == (int)scenes_.size() - 1) return 0;
	return scenes_[pageIndex_ + 1];	
}


bool SceneChangerDocumentTemplate::isCurrentScene(Scene * scene) const
{
	Scene * curScene = 0;

	if (0 <= pageIndex_ && pageIndex_ < (int)scenes_.size())
		curScene = scenes_[pageIndex_];	
	return curScene && (scene == curScene || curScene->isShowing(scene));
}

void SceneChangerDocumentTemplate::setCurPage(int pageIndex)
{
	/*
	Example: SceneChange action -> audio stopped action -> scene set action ....
	scene set action must override the first SceneChange action...
	so have to call resetPageTween()
	*/
	resetPageTween();
	Global::instance().showAppLayer(true);
	bool sceneChanged = false;	
	if (pageIndex_ != pageIndex) sceneChanged = true;
	if (!sceneChanged) return;

	if (pageIndex >= (int)scenes_.size()) return;

	curSceneAppObjectStop();
	subscene_->stop();
	pageIndex_ = pageIndex;

	if (isPreDownload_)
		resetSaveLocalRequest();

	//stop any videos or audio that were playing on the previous page
	Global::instance().videoPlayer()->deactivate();
	Global::instance().audioPlayer()->stopAll();
	Global::instance().cameraOff();
	
	//temporaryily store previous scenes to prevent needless unloading of scenes
	
	if (subscene_) subSceneBackup_[0]->setScene(subscene_->scene());
	if (subsceneLeft_) subSceneBackup_[1]->setScene(subsceneLeft_->scene());
	if (subsceneRight_) subSceneBackup_[2]->setScene(subsceneRight_->scene());

	subscene_->setScene(scenes_[pageIndex]);
    subscene_->setHasLeft(pageLeft() ? true : false);
    subscene_->setHasRight(pageRight() ? true : false);
    subscene_->setHasTop(false);
    subscene_->setHasBottom(false);
	
	setCachedScene(subsceneLeft_.get(), pageLeft());
	setCachedScene(subsceneRight_.get(), pageRight());
	setSubsceneScaling();

	//use this function to start only after loading screen disappears
	doCurPageStart();
	float curTime = Global::currentTime();
	//subsceneLeft_->start(curTime);
	//subsceneRight_->start(curTime);

	for (int i = 0; i < (int)subSceneBackup_.size(); ++i)
		subSceneBackup_[i]->setScene(0);

	setCurPagePos(Vector3(0, 0, 0));	

	doPageChangedEvent(pageIndex);
	
	Global::instance().callbackPageChanged(scenes_[pageIndex]->name());
}


bool SceneChangerDocumentTemplate::doPageChangedEvent(int index)
{
	JSContext * cx = document_->scriptProcessor()->jsContext();
	JSObject * evtObject = JS_NewObject(cx, 0, 0, 0);

	jsval val;
	val = INT_TO_JSVAL(index);
	JS_SetProperty(cx, evtObject, "index", &val);
	
	return fireEvent(EventSceneChanged, evtObject);

}

void SceneChangerDocumentTemplate::setSubsceneScaling()
{
	InternalSubscene * subscenes[3];

	subscenes[0] = subscene_.get();
	subscenes[1] = subsceneLeft_.get();
	subscenes[2] = subsceneRight_.get();

	for (int i = 0; i < 3; ++i)
	{
		InternalSubscene * subscene = subscenes[i];
		Scene * scene = subscene->scene();	
		if (scene)
		{
			Transform transform = subscene->transform();
			float sx = width_ / scene->screenWidth();
			float sy = height_ / scene->screenHeight();
			transform.setScaling(sx, sy, (sx + sy)/2);
			subscene->setTransform(transform);			
			subscene->setWindowWidth(scene->screenWidth());
			subscene->setWindowHeight(scene->screenHeight());
			subscene->setNormalScaleX(sx);
			subscene->setNormalScaleY(sy);
			
		}
	}
	
	Vector3 curSceneScale = subscene_->transform().scaling();
	document_->setCurSceneScale(curSceneScale.x, curSceneScale.y);
}

void SceneChangerDocumentTemplate::curSceneAppObjectStart() const
{
	Scene * curScene = subscene_->scene();
	//init app objects
	if (curScene)
	{
		if (!displayingAppObjects_)
		{
			curScene->initAppObjects();
			displayingAppObjects_ = true;
		}		
		
	}
}

void SceneChangerDocumentTemplate::curSceneAppObjectStop() const
{
	Scene * curScene = subscene_->scene();
	//uninit app objects
	if (curScene)
	{
		if (displayingAppObjects_)
		{
			curScene->uninitAppObjects();
			displayingAppObjects_ = false;
		}		
		
	}
}

bool SceneChangerDocumentTemplate::hasPageBeenMoved() const
{
	if (!subscene_) return false;
	const Vector3 & trans = subscene_->transform().translation();
	if (trans.x == 0.0f && trans.y == 0.0f) return false;
	return true;
}

void SceneChangerDocumentTemplate::setCachedScene(InternalSubscene * subsceneCached, Scene * scene)
{
	subsceneCached->setScene(scene);
	subsceneCached->preStart(Global::currentTime());
}

void SceneChangerDocumentTemplate::doPageSnapBack(const PressEventData & pressEvent, const Vector2 & pos)
{
	Vector2 startPt = pressEvent.startPt;
	resetPageTween();
	pageTweenStartTime_ = Global::currentTime();
	pageTweenDuration_ = 0.3f;

	Transform transform = subscene_->transform();
	transform.setPivot(0,0,0);
	subscene_->setTransform(transform);

	pageTweenStartPos_ = subscene_->transform().translation();
	pageTweenEndPos_ = Vector3(0, 0, 0);
	targetPageIndex_ = pageIndex_;
    float normalScale = Global::instance().magnification();
	float currentScale = subscene_->transform().scaling().x;

	bool isQuickFlick = 
		pageTweenStartTime_ - pressEvent.startMovingTime < 
		Global::flickTime();
    
    bool quickFlickLeft = isQuickFlick && 
    pos.x > startPt.x && pageLeft();
    bool slowLeft = !isQuickFlick && 
    pageTweenStartPos_.x > width_/2 && pageLeft();
    
    bool quickFlickRight = isQuickFlick &&
    pos.x < startPt.x && pageRight();
    bool slowRight = !isQuickFlick &&
    pageTweenStartPos_.x < -width_/2 && pageRight();
    
	if (currentScale > normalScale)
    {
        float temp = 0;
        Transform t = subscene_->transform();
        Matrix mat = t.computeMatrix();
        mat = mat.inverse();

		float dx = width_ / currentScale * normalScale / 2;
		slowLeft = !isQuickFlick && 
		mat._14 < -dx && pageLeft();

		slowRight = !isQuickFlick &&
		mat._14 > width_ - dx && pageRight();
        if (quickFlickLeft || slowLeft)
		{
            temp = ((width_ / t.scaling().x * normalScale) + mat._14 * normalScale ) / normalScale;
            mat._14 -= temp;
            mat = mat.inverse();
            t.setMatrix(mat, true);
            pageTweenEndPos_ = t.translation();
            targetPageIndex_ = pageIndex_ - 1;
        }
        else if (quickFlickRight || slowRight)
		{
            temp = ((width_ / t.scaling().x * normalScale) - (mat._14 * normalScale  - (width_ - (width_ / t.scaling().x * normalScale )))) / normalScale;
            mat._14 += temp;
            mat = mat.inverse();
            t.setMatrix(mat, true);
            pageTweenEndPos_ = t.translation();		
            targetPageIndex_ = pageIndex_ + 1;
        }
        else 
        {
            if(mat._14 < 0)
            {
                Matrix m = t.computeMatrix();;
                m._14 = 0;
                mat = m.inverse();
                t.setMatrix(m, true);
            }
            else if(mat._14 * normalScale > width_ - (width_ / t.scaling().x * normalScale))
            {
                Matrix m = mat;
                m._14 = (width_ - (width_ / t.scaling().x * normalScale)) / normalScale;
                mat = m;
                m = m.inverse();
                t.setMatrix(m, true);
            }
            if(mat._24 < 0)
            {
                Matrix m = t.computeMatrix();;
                m._24 = 0;
                t.setMatrix(m, true);
            }
            else if(mat._24 * normalScale > height_ - (height_ / t.scaling().y * normalScale))
            {
                Matrix m = mat;
                m._24 = (height_ - (height_ / t.scaling().y * normalScale)) / normalScale;
                m = m.inverse();
                t.setMatrix(m, true);
            }
            pageTweenEndPos_ = t.translation();
        }
    }
    else
    {
        if (quickFlickLeft || slowLeft)
		{
            pageTweenEndPos_ = Vector3(width_, 0, 0);
            targetPageIndex_ = pageIndex_ - 1;
        }
        else if (quickFlickRight || slowRight)
		{
            pageTweenEndPos_ = Vector3(-width_, 0, 0);		
            targetPageIndex_ = pageIndex_ + 1;
        }
    }
}

void SceneChangerDocumentTemplate::setCurPagePos(const Vector3 & pos)
{
	setBaseAppObjectOffset(
		pos.x, pos.y,		
		document_->curSceneScaleX(), document_->curSceneScaleY(),
		pos.x, pos.y, width_, height_);

	Vector3 pagePos = pos;
	Transform transform;
	
	transform = subscene_->transform();
	transform.setTranslation(pagePos.x, pagePos.y, 0);
	subscene_->setTransform(transform);	
    
    float normalScale = Global::instance().magnification();
	float currentScale = Global::instance().curSceneMagnification();
    if (currentScale > normalScale && subscene_->isZoomable())
    {
        float tempW = width_ * transform.scaling().x / normalScale;
        Matrix m = transform.computeMatrix();
        
        pagePos = Vector3(-width_ , 0 , 0) + Vector3(m._14, 0, 0);
        transform = subsceneLeft_->transform();
        transform.setTranslation(pagePos.x, pagePos.y, 0);
        subsceneLeft_->setTransform(transform);	
        
        pagePos = Vector3(tempW , 0 , 0) + Vector3(m._14, 0, 0);
        transform = subsceneRight_->transform();
        transform.setTranslation(pagePos.x, pagePos.y, 0);
        subsceneRight_->setTransform(transform);
    }
    else
    {
        pagePos = pos + Vector3(-width_, 0, 0);
        transform = subsceneLeft_->transform();
        transform.setTranslation(pagePos.x, pagePos.y, 0);
        subsceneLeft_->setTransform(transform);	

        pagePos = pos + Vector3(width_, 0, 0);
        transform = subsceneRight_->transform();
        transform.setTranslation(pagePos.x, pagePos.y, 0);
        subsceneRight_->setTransform(transform);		
    }
}


bool SceneChangerDocumentTemplate::pressEvent(const Vector2 & startPos, int pressId, bool propagate)
{
	PressEventData & pressEvent = pressEvents_[pressId];
	pressEvent.propagate = propagate;
	if (isPageTweenHappening() || subscene_->isTweenWorking()) 
	{
		pressEvent.pressed = PressEventData::StrayPress;
		return true;
	}

	startPagePos_ = subscene_->transform().translation();		

	pressEvent.pressed = PressEventData::Pressed;	
	pressEvent.startedMoving = false;	
	pressEvent.pressPt = Vector2(startPos);

	bool handled = false;

	if (propagate)
		handled = subscene_->pressEvent(startPos, pressId);

	if (handled) 
	{
		pressEvent.pressed = PressEventData::PassedToChild;
		return true;
	}

	if (!allowDrag_) 
	{
		pressEvent.pressed = PressEventData::NotPressed;
		return false;
	}

	if (pageMovingId_ == -1 && !handled) pageMovingId_ = pressId;
	return true;
}


bool SceneChangerDocumentTemplate::moveEvent(const Vector2 & pos, int pressId)
{	
	PressEventData & pressEvent = pressEvents_[pressId];
	if (pressEvent.pressed == PressEventData::PassedToChild)
	{
		bool handled = subscene_->moveEvent(pos, pressId);
		if (handled) return true;
	}
	else
	{
		bool handled = subscene_->moveEvent(pos, pressId);	
		if (handled)
		{
			pressEvent.pressed = PressEventData::PassedToChild;
			return true;
		}	
	}
	
	if (pressEvent.pressed == PressEventData::StrayPress) return true;
	if (pressEvent.pressed == PressEventData::NotPressed) return false;
	if (pressEvent.pressed == PressEventData::PassedToParent) return false;
	if (!allowDrag_) return false;

	if (pageMovingId_ == -1)
		pageMovingId_ = pressId;
	else if (pageMovingId_ != pressId) return false;

	pageMovingPos_ = pos;

	if (!pressEvent.startedMoving) 
	{
		if (fabs(pos.x - pressEvent.pressPt.x) <
			fabs(pos.y - pressEvent.pressPt.y) && 
            !(subscene_->transform().scaling().x > Global::instance().magnification()))
		{
			//vertical scrolling
			//must pass on event data to parents upstream
			pressEvent.pressed = PressEventData::PassedToParent;
			return false;			
		}
		
		pressEvent.startMovingTime = Global::currentTime();
		pressEvent.startPt = pos;
		pressEvent.startedMoving = true;
		Global::instance().showAppLayer(false);
		notifyPageChanging();

		
	}
		
	const Vector2 & pressPt = pressEvent.pressPt;
	//float scaleX = subscene_->transform().scaling().x;
	if (Global::instance().curSceneMagnification() > Global::instance().magnification() && subscene_->isZoomable())
        setCurPagePos(startPagePos_ + Vector3(pos.x - pressPt.x, pos.y - pressPt.y, 0));	
    else
        setCurPagePos(startPagePos_ + Vector3(pos.x - pressPt.x, 0, 0));	
		
	redrawTriggered_ = true;
	return true;
}

bool SceneChangerDocumentTemplate::releaseEvent(const Vector2 & pos, int pressId)
{	
	PressEventData & pressEvent = pressEvents_[pressId];
	
	if (pressEvent.pressed == PressEventData::StrayPress) return true;	
	if (pressEvent.pressed == PressEventData::NotPressed) return false;    
    
    
    
	if (pressId == pageMovingId_) 
	{
		pageMovingId_ = -1;
		if (hasPageBeenMoved()) doPageSnapBack(pressEvent, pos);
	}
    
	if (pressEvent.pressed == PressEventData::PassedToChild)
		subscene_->releaseEvent(pos, pressId);
	
	redrawTriggered_ = true;	
	return true;
}

bool SceneChangerDocumentTemplate::keyPressEvent(int keyCode)
{
	return subscene_->keyPressEvent(keyCode);
}

bool SceneChangerDocumentTemplate::keyReleaseEvent(int keyCode)
{
	return subscene_->keyReleaseEvent(keyCode);
}


bool SceneChangerDocumentTemplate::clickEvent(const Vector2 & pos, int pressId)
{
	//setTempSubSceneTransform();
	bool handled = false;	
	if (pressEvents_[pressId].propagate)
		handled = subscene_->clickEvent(pos, pressId);
	//resetSubSceneTransform();
	return handled;
}

bool SceneChangerDocumentTemplate::doubleClickEvent(const Vector2 & pos, int pressId)
{
	//setTempSubSceneTransform();
	bool handled = false;	
	if (pressEvents_[pressId].propagate)
		handled = subscene_->doubleClickEvent(pos, pressId);
	//resetSubSceneTransform();
	setCurPagePos(Vector3(0,0,0));
	return handled;
}


bool SceneChangerDocumentTemplate::update(float sceneTime)
{
	bool needRedraw = false;

	if (!subscene_->isLoaded())
		needRedraw = true;
	else
		needRedraw |= subscene_->update(sceneTime);

	if (redrawTriggered_)
	{
		redrawTriggered_ = false;
		needRedraw = true;
	}
	
	if (isPageTweenHappening())
	{			
		float t = (sceneTime - pageTweenStartTime_) / pageTweenDuration_;		
		if (t > 1) t = 1;		
		t = 1 - (t - 1)*(t - 1);

		Vector3 pos = 
			pageTweenStartPos_ + t*(pageTweenEndPos_ - pageTweenStartPos_);

		setCurPagePos(pos);		

		if (t == 1)
		{
			Scene * prevScene = subscene_->scene();
			resetPageTween();
			setCurPage(targetPageIndex_);		
			

		}

		needRedraw = true;	
	}
	
	if (subscene_->isLoaded())
	{
		Matrix m = subscene_->transform().computeMatrix();
		setBaseAppObjectOffset(
			m._14, m._24, 
			m._11 , m._22,
			m._14, m._24, width_ * m._11 / subscene_->normalScaleX() , height_ * m._22 / subscene_->normalScaleY());
	}

	return needRedraw;
}

bool SceneChangerDocumentTemplate::asyncLoadUpdate() 
{
	bool everythingLoaded = true;
	everythingLoaded &= subscene_->asyncLoadUpdate();
	if (subsceneLeft_)everythingLoaded &= subsceneLeft_->asyncLoadUpdate();
	if (subsceneRight_)everythingLoaded &= subsceneRight_->asyncLoadUpdate();

	everythingLoaded &= activePageImg_->asyncLoadUpdate();
	everythingLoaded &= inactivePageImg_->asyncLoadUpdate();

	if(!searchQ_.empty() && resetComplete_)
	{
		Scene * scene = searchQ_.front();
		searchQ_.pop_front();
		scene->requestLocalSaveObject();
	}

	return everythingLoaded;
}

void SceneChangerDocumentTemplate::drawMarkers(GfxRenderer * gl) const
{
	int numScenes = (int)scenes_.size();	
	
	float maxWidth = (float)activePageImg_->width();
	if (activePageImg_->width() > maxWidth)
		maxWidth = (float)activePageImg_->width();
	float maxHeight = (float)activePageImg_->height();
	if (inactivePageImg_->height() > maxHeight) 
		maxHeight = (float)inactivePageImg_->height();

	float curX = 0.5f*width_ - 0.5f*((numScenes - 1) * maxWidth);
	
	float curY;
	
	int pageIndex = pageIndex_;
	if (pageIndex < 0) pageIndex = 0;
	for (int i = 0; i < numScenes; ++i)
	{
		if (i == pageIndex) 
		{
			curY = height_ - maxHeight/2 - activePageImg_->height()/2;
			gl->use(activePageImg_.get());
			gl->drawRect(
				curX - activePageImg_->width()/2, curY,
				activePageImg_->width(), activePageImg_->height());
			curX += maxWidth;
		}
		else 
		{
			curY = height_ - maxHeight/2 - inactivePageImg_->height()/2;
			gl->use(inactivePageImg_.get());
			gl->drawRect(
				curX - inactivePageImg_->width()/2, curY, 
				(float)inactivePageImg_->width(), (float)inactivePageImg_->height());
			curX += maxWidth;
		}
		
	}
}

void SceneChangerDocumentTemplate::sceneSet(Scene * scene)
{
	bool sceneFound = false;
	for (int i = 0; i < (int)scenes_.size(); ++i)
	{
		if (scenes_[i] == scene)
		{
			targetPageIndex_ = i;
			sceneFound = true;
		}
	}

	if (!sceneFound) return;

	notifyPageChanging();
	setCurPage(targetPageIndex_);
}

void SceneChangerDocumentTemplate::sceneChangeTo(int index)
{
	//trigger release if user has grabbed and dragged the page but not released
	if (pageMovingId_ >= 0) 
	{
		PressEventData & pressEvent = pressEvents_[pageMovingId_];			
		releaseEvent(pageMovingPos_, pageMovingId_);
		pressEvent.pressed = PressEventData::StrayPress;			
	}
	//if in the middle of a page tween, just reset it (if it was to another page)
	else if (isPageTweenHappening())
	{
		if (targetPageIndex_ != index)
		{
			resetPageTween();
			return sceneChangeTo(index);
		}
		else return;
	}

	if (index < 0 || index >= (int)scenes_.size()) return;
	if (index == pageIndex_) return;

	int prevTargetPageIndex = targetPageIndex_;
	targetPageIndex_ = index;


	if (targetPageIndex_ < pageIndex_)
	{
		pageTweenEndPos_ = Vector3(width_, 0, 0);
		setCachedScene(subsceneLeft_.get(), scenes_[targetPageIndex_]);
	}
	else
	{
		pageTweenEndPos_ = Vector3(-width_, 0, 0);
		setCachedScene(subsceneRight_.get(), scenes_[targetPageIndex_]);
	}

	pageTweenStartTime_ = Global::currentTime();
	pageTweenDuration_ = 0.3f;
	pageTweenStartPos_ = subscene_->transform().translation();

	notifyPageChanging();
	redrawTriggered_ = true;

}

void SceneChangerDocumentTemplate::sceneChangeTo(Scene * scene)
{
	int index = -1;
	for (int i = 0; i < (int)scenes_.size(); ++i)
	{
		if (scenes_[i] == scene) 
		{
			index = i;			
			break;
		}
	}

	sceneChangeTo(index);	
}

void SceneChangerDocumentTemplate::notifyPageChanging()
{	
	Scene * curPageScene = subscene_->scene();
	document_->broadcastMessage(
		curPageScene, pageChangingMsg_);	
	
	static PageChangingEvent event;
	curPageScene->root()->handleEvent(&event, Global::currentTime());

}


void SceneChangerDocumentTemplate::drawSubscene(GfxRenderer * gl, InternalSubscene * subscene) const
{
	if (!subscene->isLoaded())
	{
		const Vector3 & trans = subscene->transform().translation();
		loadingPage_->draw(gl, trans.x, trans.y);
		document_->triggerRedraw();
	}
	else
	{		
		bool isCurPage = subscene == subscene_.get();
		if (isCurPage && !curPageStarted_)
		{
			subscene->start(Global::currentTime());
			curSceneAppObjectStart();
			curPageStarted_ = true;
		}		

		subscene->draw(gl);
	}
}

void SceneChangerDocumentTemplate::draw(GfxRenderer * gl) const
{		
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | Global::extraGlClearOptions());
	gl->setCameraMatrix(camera_.GetMatrix());
	gl->loadMatrix(Matrix::Identity());

	bool drawLeft = subsceneLeft_->transform().translation().x > -width_;
	bool drawRight = subsceneRight_->transform().translation().x < width_;
	
	if (drawLeft) drawSubscene(gl, subsceneLeft_.get());
	else if (drawRight) drawSubscene(gl, subsceneRight_.get());	

	if (drawLeft || drawRight) subscene_->setMode(InternalSubscene::ModeScrollable);
	else subscene_->setMode(InternalSubscene::ModeFixed);	
	drawSubscene(gl, subscene_.get());

	bool showMarkers = 
		activePageImg_->isLoaded() && inactivePageImg_->isLoaded();

	if (showMarkers) drawMarkers(gl);
	
}

void SceneChangerDocumentTemplate::start(float docTime)
{	
	setCurPage(0);
}

bool SceneChangerDocumentTemplate::addEventListener(const std::string & eventStr, JSObject * handler)
{
	Event event;
	if (eventStr == "Scene Changed")
		event = EventSceneChanged;
	else
		return false;

	eventListeners_.insert(make_pair(event, handler));
	return true;
}

bool SceneChangerDocumentTemplate::fireEvent(SceneChangerDocumentTemplate::Event event, JSObject * evtObject)
{
	bool handled = false;
	typedef multimap<Event, JSObject *>::iterator iterator;
	pair<iterator, iterator> range = eventListeners_.equal_range(event);
	jsval evtVal;
	if (evtObject) evtVal = OBJECT_TO_JSVAL(evtObject);
	for (iterator iter = range.first; iter != range.second; ++iter)
	{
		handled = true;
		JSObject * handler = (*iter).second;
		jsval rval;
		jsval funcVal = OBJECT_TO_JSVAL(handler);
		ScriptProcessor * s = document_->scriptProcessor();
		if (evtObject)
			JS_CallFunctionValue(s->jsContext(), s->jsGlobal(), funcVal, 1, 
				&evtVal, &rval);
		else
			JS_CallFunctionValue(s->jsContext(), s->jsGlobal(), funcVal, 0, 
				0, &rval);
	}

	return handled;
}


Scene * SceneChangerDocumentTemplate::dependsOn(Scene * scene) const
{
	BOOST_FOREACH(Scene * childScene, scenes_)
	{
		if (childScene == scene || childScene->dependsOn(scene))
			return childScene;
	}

	return 0;
}

void SceneChangerDocumentTemplate::pageScenes(std::vector<Scene *> * scenes) const
{
	BOOST_FOREACH(Scene * childScene, scenes_) scenes->push_back(childScene);
}

void SceneChangerDocumentTemplate::referencedFiles(vector<string> * refFiles) const
{
}

int SceneChangerDocumentTemplate::setReferencedFiles(
	const std::string & baseDirectory,
	const vector<string> & refFiles, int startIndex)
{
	return startIndex;
}

void SceneChangerDocumentTemplate::write(Writer & writer) const
{
	writer.writeParent<DocumentTemplate>(this);
	writer.write(visualAttrib_, "visualAttrib");	
	writer.write(width_, "width");
	writer.write(height_, "height");
	writer.write(scenes_, "scenes");	

	writer.write(activeMarkerFileName_, "activeMarker");
	writer.write(inactiveMarkerFileName_, "inactiveMarker");

	writer.write(allowDrag_, "allowDrag");
	writer.write(pageChangingMsg_, "pageChangingMsg");

	writer.write(isPreDownload_, "preDownload");
}

void SceneChangerDocumentTemplate::writeXml(XmlWriter & w) const
{
	w.startTag("Scenes");
	for (int i = 0; i < (int)scenes_.size(); ++i)
	{
		w.writeTag("Scene", w.idMapping().getId(scenes_[i]));		
	}
	w.endTag(); //scenes

	w.writeTag("AllowDrag", allowDrag_);
}

void SceneChangerDocumentTemplate::read(Reader & reader, unsigned char version)
{
	reader.readParent<DocumentTemplate>(this);
	reader.read(visualAttrib_);
	reader.read(width_);
	reader.read(height_);
	reader.read(scenes_);
		
	reader.read(activeMarkerFileName_);
	reader.read(inactiveMarkerFileName_);

	reader.read(allowDrag_);
	
	reader.read(pageChangingMsg_);


	if (version >= 1)
		reader.read(isPreDownload_);
}

void SceneChangerDocumentTemplate::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.isNodeName(curNode, "Scenes"))
		{
			unsigned int sceneId = 0;
			for(xmlNode * sceneNode = curNode->children; sceneNode; sceneNode = sceneNode->next)
			{		
				if (sceneNode->type != XML_ELEMENT_NODE) continue;				
				if (r.getNodeContentIfName(sceneId, sceneNode, "Scene"))
				{
					scenes_.push_back(r.idMapping().getScene(sceneId));
				}
			}			
		}
	}	
}

void SceneChangerDocumentTemplate::reInitCurScene(GfxRenderer * gl)
{
	if (!subscene_->isInit()) return;
	subscene_->resizeText(gl);
	if (!subsceneLeft_->isInit()) return;
	subsceneLeft_->resizeText(gl);
	if (!subsceneRight_->isInit()) return;
	subsceneRight_->resizeText(gl);
}

void SceneChangerDocumentTemplate::resetSaveLocalRequest()
{
    Global & g = Global::instance();
    g.resetSaveLocalRequest();
	resetComplete_ = false;
	searchQ_.clear();
	int searchDepth;
	if(scenes_.size()/2 > pageIndex_)
		searchDepth = scenes_.size() - pageIndex_;
	else
		searchDepth = pageIndex_;

	for(int i = 0 ; i < searchDepth ; i++)
	{
		if (pageIndex_+i < scenes_.size() && i!=0)
			searchQ_.push_back(scenes_[pageIndex_+i]);
		if (pageIndex_-i >= 0 && i!=0)
			searchQ_.push_back(scenes_[pageIndex_-i]);
	}
	resetComplete_ = true;
}
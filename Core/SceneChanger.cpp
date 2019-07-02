#include "stdafx.h"
#include "Subscene.h"
#include "SceneChanger.h"
#include "Mesh.h"
#include "Image.h"
#include "Texture.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Scene.h"
#include "Reader.h"
#include "Camera.h"
#include "GLUtils.h"
#include "ElementMapping.h"
#include "FileUtils.h"
#include "Document.h"
#include "PageChangedEvent.h"
#include "PageChangingEvent.h"
#include "Global.h"
#include "Document.h"
#include "DocumentTemplate.h"
#include "VideoPlayer.h"
#include "AudioPlayer.h"
#include "TextureRenderer.h"
#include "LoadingPage.h"
#include "ScriptProcessor.h"
#include "PageFlip.h"
#include "PageFlipOrig.h"
#include "Xml.h"
using namespace std;

JSClass jsSceneChangerClass = InitClass(
	"SceneChanger", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);


JSBool SceneChanger_changeScene(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneChanger * thisObj = (SceneChanger *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
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

JSBool SceneChanger_setScene(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneChanger * thisObj = (SceneChanger *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsScene;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsScene)) return JS_FALSE;
	
	if (JS_GetPrototype(cx, jsScene) == s->sceneProto())
	{
		Scene * scene = (Scene *)JS_GetPrivate(cx, jsScene);
		thisObj->sceneSet(scene);
	}

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool SceneChanger_changeSceneIndex(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneChanger * thisObj = (SceneChanger *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	uint32 jsIndex;
	if (!JS_ConvertArguments(cx, argc, argv, "u", &jsIndex)) return JS_FALSE;
	thisObj->sceneChangeTo((int)jsIndex);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool SceneChanger_setSceneIndex(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneChanger * thisObj = (SceneChanger *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	uint32 jsIndex;
	if (!JS_ConvertArguments(cx, argc, argv, "u", &jsIndex)) return JS_FALSE;
	thisObj->sceneSet((int)jsIndex);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool SceneChanger_getSceneIndex(JSContext *cx, uintN argc, jsval *vp)
{
	SceneChanger * thisObj = (SceneChanger *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));		
	int32 index = thisObj->pageIndex();
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(index));
	return JS_TRUE;
};

JSBool SceneChanger_getNumScenes(JSContext *cx, uintN argc, jsval *vp)
{
	SceneChanger * thisObj = (SceneChanger *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));		
	int32 numScenes = (int32)thisObj->scenes().size();
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(numScenes));
	return JS_TRUE;
};

JSBool SceneChanger_getScenes(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneChanger * thisObj = (SceneChanger *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	const vector<Scene *> & scenes = thisObj->scenes();
	
	JSObject * jsScenes = JS_NewArrayObject(cx, (jsint)scenes.size(), 0);

	for (int i = 0; i < (int)scenes.size(); ++i)
	{
		jsval val;
		val = OBJECT_TO_JSVAL(scenes[i]->getScriptObject(s));
		JS_SetElement(cx, jsScenes, (jsint)i, &val);
	}

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsScenes));	

	return JS_TRUE;
};

JSFunctionSpec SceneChangerFuncs[] = {
	JS_FS("changeScene", SceneChanger_changeScene, 1, 0),
	JS_FS("setScene", SceneChanger_setScene, 1, 0),
	JS_FS("changeSceneIndex", SceneChanger_changeSceneIndex, 1, 0),
	JS_FS("setSceneIndex", SceneChanger_setSceneIndex, 1, 0),
	JS_FS("getSceneIndex", SceneChanger_getSceneIndex, 0, 0),
	JS_FS("getNumScenes", SceneChanger_getNumScenes, 0, 0),
	JS_FS("getScenes", SceneChanger_getScenes, 0, 0),
    JS_FS_END
};

///////////////////////////////////////////////////////////////////////////////

JSObject * SceneChanger::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsSceneChangerClass,
		0, 0, 0, SceneChangerFuncs, 0, 0);

	return proto;
}

JSObject * SceneChanger::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsSceneChangerClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}


void SceneChanger::create()
{
	setId("SceneChanger");
	width_ = 768;
	height_ = 1024;	
	redrawTriggered_ = false;

	subscene_.reset(new Subscene);
	subscenePrev_.reset(new Subscene);
	subsceneNext_.reset(new Subscene);

	setCameraMode(Subscene::ChildScene);
	
	pageTweenEndPos_ = Vector3(0, 0, 0);
	pageTweenStartTime_ = pageTweenDuration_ = 0;

	startPagePos_ = Vector3(0, 0, 0);
	allowDrag_ = true;


	for (int i = 0; i < 3; ++i)
		subSceneBackup_.push_back(new Subscene);
	
	resetPageTween();

	pageChangingMsg_ = "pageChanging";

	pageIndex_ = -1;

	linkWithViewerLevelObjects_ = false;
	displayingAppObjects_ = false;

	texRenderer_ = texRenderer2_ = 0;
	renderTex_ = renderTex2_ = 0;

	showMarkers_ = false;
	draggingOnMarkers_ = false;

	//loading page
	loadingPage_.reset(new LoadingScreen);
	curPageStarted_ = false;

	scrollDirection_ = Horizontal;
	markerLocation_ = Bottom;

	markerOffsetX_ = 0;
	markerOffsetY_ = 0;

	preloadAdjacent_ = true;
	pageMovingId_ = -1;

	transitionMode_ = SlidingMode;
	transDir_ = prevTransDir_ = TransitionDirectionNone;
	pageFlip_ = NULL;
	
	//SceneChanger
	normalScaleX = 1;
	normalScaleY = 1;
}

SceneChanger::SceneChanger()
{
	create();
}

SceneChanger::SceneChanger(
	const SceneChanger & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();
	visualAttrib_ = rhs.visualAttrib_;
	width_ = rhs.width_;
	height_ = rhs.height_;

	BOOST_FOREACH(Scene * scene, rhs.scenes_)
		scenes_.push_back(scene);	


	activeMarkerFileName_ = rhs.activeMarkerFileName_;
	inactiveMarkerFileName_ = rhs.inactiveMarkerFileName_;
	allowDrag_ = rhs.allowDrag_;
	pageChangingMsg_ = rhs.pageChangingMsg_;

	setTransparentBg(rhs.transparentBg());
	linkWithViewerLevelObjects_ = rhs.linkWithViewerLevelObjects_;

	showMarkers_ = rhs.showMarkers_;
	scrollDirection_ = rhs.scrollDirection_;
	markerLocation_ = rhs.markerLocation_;
	markerOffsetX_ = rhs.markerOffsetX_;
	markerOffsetY_ = rhs.markerOffsetY_;
}

SceneChanger::~SceneChanger()
{
	uninit();
	BOOST_FOREACH(Subscene * subscene, subSceneBackup_) delete subscene;
    delete pageFlip_;
	pageFlip_ = NULL;
}


void SceneChanger::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);

	vector<Subscene *> subscenes;
	subscenes.push_back(subscene_.get());
	subscenes.push_back(subscenePrev_.get());
	subscenes.push_back(subsceneNext_.get());

	BOOST_FOREACH(Subscene * subscene, subscenes)
	{
		subscene->setMode(Subscene::ModeFixed);
		subscene->setLocked(true);
		subscene->setWindowWidth(width_);
		subscene->setWindowHeight(height_);	
		subscene->init(gl);
		setAsSubObject(subscene);
	}


	activePageImg_.reset(new Texture);
	inactivePageImg_.reset(new Texture);

	//activePageImg_->init("c:\\active.png");
	//inactivePageImg_->init("c:\\inactive.png");

	if (showMarkers_)
	{
	activePageImg_->init(gl, activeMarkerFileName_, Texture::UncompressedRgba32);
	inactivePageImg_->init(gl, inactiveMarkerFileName_, Texture::UncompressedRgba32);
	}

	BOOST_FOREACH(Subscene * subscene, subSceneBackup_) 
	{
		subscene->init(gl);
		setAsSubObject(subscene);
	}	

	loadingPage_->init(gl, width_, height_);

	
	if (pageFlip_) 
	{
		pageFlip_->init(gl);
		pageFlip_->setPageDims(width_, height_);
		initTextureRendering(gl);
	}
	prevTransDir_ = TransitionDirectionNone;
}

void SceneChanger::uninit()
{
	if (!gl_) return;
	subscene_->uninit();
	subscenePrev_->uninit();
	subsceneNext_->uninit();

	BOOST_FOREACH(Subscene * s, subSceneBackup_)
	{
		s->uninit();
	}
	
	activePageImg_.reset();
	inactivePageImg_.reset();
	
	uninitTextureRendering();
	loadingPage_->uninit();
	SceneObject::uninit();

}



void SceneChanger::initTextureRendering(GfxRenderer * gl)
{
	uninitTextureRendering();
	texRenderer_ = new TextureRenderer;
	renderTex_ = new Texture;

	renderTex_->init(gl, (int)width_, (int)height_, Texture::UncompressedRgb16, 0);
	texRenderer_->init(gl, (int)width_, (int)height_, true);
	texRenderer_->setTexture(renderTex_);

	texRenderer2_ = new TextureRenderer;
	renderTex2_ = new Texture;

	renderTex2_->init(gl, (int)width_, (int)height_, Texture::UncompressedRgb16, 0);
	texRenderer2_->init(gl, (int)width_, (int)height_, true);
	texRenderer2_->setTexture(renderTex2_);
}

void SceneChanger::uninitTextureRendering()
{
	delete texRenderer_;
	texRenderer_ = 0;
	delete renderTex_;
	renderTex_ = 0;

	delete texRenderer2_;
	texRenderer2_ = 0;
	delete renderTex2_;
	renderTex2_ = 0;
}



Scene * SceneChanger::pagePrev() const
{
	if (pageIndex_ <= 0) return 0;
	return scenes_[pageIndex_ - 1];
}

Scene * SceneChanger::pageNext() const
{
	if (pageIndex_ >= (int)scenes_.size() - 1) return 0;
	return scenes_[pageIndex_ + 1];	
}


void SceneChanger::initAppObjects()
{	
	Scene * curScene = subscene_->scene();
	if (
		linkWithViewerLevelObjects_ && !displayingAppObjects_ && 
		curScene && curScene->isCurrentScene())
	{
		curScene->initAppObjects();
		displayingAppObjects_ = true;		
	}
}

void SceneChanger::uninitAppObjects()
{
	Scene * curScene = subscene_->scene();
	if (linkWithViewerLevelObjects_ && displayingAppObjects_ &&
		curScene && curScene->isCurrentScene())
	{			
		curScene->uninitAppObjects();		
		displayingAppObjects_ = false;		
	}	
}


void SceneChanger::setScenes(const std::vector<Scene *> & scenes)
{
	scenes_ = scenes;
	pageIndex_ = -1;
	subscene_->setScene(this, 0);
	for (int i = 0; i < (int)scenes_.size(); ++i)
		scenes_[i]->setCurContainerObject(this);

	setCurPage(0);
}

void SceneChanger::doCurPageStart()
{
	curPageStarted_ = false; //schedule a start when the page finishes loading	
	//force refresh of isLoaded flag
	//this helps prevents glitches on next draw call
	if (subscene_->isInit()) subscene_->asyncLoadUpdate(); 	
}

void SceneChanger::setInitPage()
{	
	if (pageIndex_ >= 0) return; 
	if (scenes_.empty()) return;
	
	subscene_->setScene(this, scenes_[0]);
	doCurPageStart();
}
int counter=0;
void SceneChanger::setCurPage(int pageIndex)
{
	/*
	Example: SceneChange action -> audio stopped action -> scene set action ....
	scene set action must override the first SceneChange action...
	so have to call resetPageTween()
	*/
	if (transitionMode_ == PageFlipMode || transitionMode_ == PageFlipOrigMode)
	{
		pageFlip_->reset();
	}
	else
	{
		resetPageTween();
	}

	bool sceneChanged = false;	
	if (pageIndex_ != pageIndex) sceneChanged = true;
	if (!sceneChanged) return;

	if (pageIndex >= (int)scenes_.size()) return;

	if (linkWithViewerLevelObjects_)
	{
		//stop any videos or audio that were playing on the previous page
		ContainerObjects containerObjects;
		Scene * curScene = subscene_->scene();
		Scene * scene = parentScene();
		Document * doc = scene->parentDocument();

		if (curScene)
		{
			curScene->getContainerObjects(&containerObjects);	
			if (doc->isCurrentVideoScene(containerObjects))
			Global::instance().videoPlayer()->deactivate();
		}
		
		//TODO Need to check if audio was initiated in the current scene before
		//stopping the audio		
		Global::instance().audioPlayer()->stopAll();
	}

	uninitAppObjects();
	subscene_->stop();
	
	pageIndex_ = pageIndex;

	//temporarily store previous scenes to prevent needless unloading of scenes
	
	if (subscene_) subSceneBackup_[0]->setScene(this, subscene_->scene());
	if (subscenePrev_) subSceneBackup_[1]->setScene(this, subscenePrev_->scene());
	if (subsceneNext_) subSceneBackup_[2]->setScene(this, subsceneNext_->scene());
	
	subscene_->setScene(this, scenes_[pageIndex]);	
	
	//SceneChanger zoom
	if (scrollDirection_ == Horizontal)
	{
		subscene_->setHasLeft(pagePrev() ? true : false);
		subscene_->setHasRight(pageNext() ? true : false);
		subscene_->setHasTop(false);
		subscene_->setHasBottom(false);
	}
	else
	{
		subscene_->setHasLeft(false);
		subscene_->setHasRight(false);
		subscene_->setHasTop(pagePrev() ? true : false);
		subscene_->setHasBottom(pageNext() ? true : false);
	}
	
	if (preloadAdjacent_)
	{
		subscenePrev_->setScene(this, pagePrev());
		subsceneNext_->setScene(this, pageNext());
	}
	else
	{
		subscenePrev_->setScene(this, NULL);
		subsceneNext_->setScene(this, NULL);
	}

	setSubsceneScaling();

	doCurPageStart();
	float sceneTime = Global::currentTime();

	if (preloadAdjacent_)
	{
		subscenePrev_->preStart(sceneTime);
		subsceneNext_->preStart(sceneTime);	
	}

	for (int i = 0; i < (int)subSceneBackup_.size(); ++i)
		subSceneBackup_[i]->setScene(this, 0);

	Transform t = subscene_->transform();
	t.setPivot(0,0,0);
	t.setTranslation(0,0,0);
	subscene_->setTransform(t);
	setCurPagePos(Vector3(0, 0, 0));		

	initAppObjects();

	doPageChangedEvent(pageIndex);
}

void SceneChanger::setSubsceneScaling()
{
	Subscene * subscenes[3];

	subscenes[0] = subscene_.get();
	subscenes[1] = subscenePrev_.get();
	subscenes[2] = subsceneNext_.get();

	static bool checkFirst = false;
	if (!checkFirst)
	{
		Transform transform = subscene_->transform();
		normalScaleX = transform.scaling().x;
		normalScaleY = transform.scaling().y;
		checkFirst = true;
	}

	for (int i = 0; i < 3; ++i)
	{
		Subscene * subscene = subscenes[i];
		Scene * scene = subscene->scene();	
		if (scene)
		{
			//Transform transform = subscene_->transform();
			//transform.setScaling(normalScaleX, normalScaleY, (normalScaleX + normalScaleY)/2);
 		//	subscene->setTransform(transform);
			//subscene->setNormalScaleX(normalScaleX);
			//subscene->setNormalScaleY(normalScaleY);
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
}

bool SceneChanger::hasPageBeenMoved() const
{
	if (!subscene_) return false;
	const Vector3 & trans = subscene_->transform().translation();
	if (trans.x == 0.0f && trans.y == 0.0f) return false;
	return true;
}

void SceneChanger::doPageSnapBack(const PressEventData & pressEvent, const Vector2 & pt)
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
	float currentScale = subscene_->curSceneMag();
	float width = width_ * normalScale;
	float height = height_ * normalScale;

	bool isQuickFlick = 
		Global::currentTime() - pressEvent.startMovingTime < 
		Global::flickTime();

	float maxX = 0;
	float maxY = 0;
	float halfPos;
	float startPos;
	float curPos;
	float scenePos;
	float curLeng;

	if (scrollDirection_ == Horizontal)
	{
		maxX = width;
		startPos = startPt.x;
		curPos = pt.x;	
		scenePos = pageTweenStartPos_.x;
		halfPos = width/2;
		if (currentScale > normalScale)
		{
			halfPos = width / currentScale * normalScale / 2;
			scenePos = subscene_->transform().computeMatrix().inverse()._14;
			curLeng = width;
		}
	}
	else
	{
		maxY = height;
		startPos = startPt.y;
		curPos = pt.y;		
		scenePos = pageTweenStartPos_.y;
		halfPos = height/2;
		if (currentScale > normalScale)
		{
			halfPos = height / currentScale * normalScale / 2;
			scenePos = subscene_->transform().computeMatrix().inverse()._24;
			curLeng = height;
		}
	}

	bool quickFlickLeft = isQuickFlick && 
		curPos > startPos && pagePrev();
	bool slowLeft = !isQuickFlick && 
		scenePos > halfPos && pagePrev();

	bool quickFlickRight = isQuickFlick &&
		curPos < startPos && pageNext();
	bool slowRight = !isQuickFlick &&
		scenePos < -halfPos && pageNext();
	if (currentScale > normalScale)
	{
		slowLeft = !isQuickFlick && 
		scenePos < -halfPos && pagePrev();

		slowRight = !isQuickFlick &&
		scenePos > curLeng - halfPos && pageNext();
	}

	//if (subscene_->transform().scaling().x > normalScale)
	if (currentScale > normalScale)
	{
		float temp = 0;
		Transform t = subscene_->transform();
		//Vector3 scaling = t.scaling();
		//Matrix parentm = subscene_->parentTransform();
		Matrix mat = t.computeMatrix();
		mat = mat.inverse();
		
		if (quickFlickLeft || slowLeft)
		{
			if (scrollDirection_ == Horizontal)
			{
				//temp = ((width_ / t.scaling().x * normalScale) + mat._14 * normalScale ) / normalScale;
				temp = ((width / currentScale) + mat._14);
				mat._14 -= temp;
			}
			else
			{
				temp = ((height / currentScale) + mat._24);
				mat._24 -= temp;
			}
			mat = mat.inverse();
			t.setMatrix(mat, true);
			pageTweenEndPos_ = t.translation();
			targetPageIndex_ = pageIndex_ - 1;
			Global::instance().setCurSceneMagnification(Global::instance().magnification());
			Global::instance().setGlobalCurMagnification(1);
		}
		else if (quickFlickRight || slowRight)
		{
			if (scrollDirection_ == Horizontal)
			{
				//temp = ((width_ / t.scaling().x * normalScale) - (mat._14 * normalScale  - (width_ - (width_ / t.scaling().x * normalScale )))) / normalScale;
				temp = ((width / currentScale * normalScale) - (mat._14 * normalScale - (width - (width / currentScale * normalScale)))) / normalScale;
				mat._14 += temp;
			}
			else
			{
				temp = ((height / currentScale * normalScale) - (mat._24 * normalScale - (height - (height / currentScale * normalScale)))) / normalScale;
				mat._24 += temp;
			}
			mat = mat.inverse();
			t.setMatrix(mat, true);
			pageTweenEndPos_ = t.translation();		
			targetPageIndex_ = pageIndex_ + 1;
			Global::instance().setCurSceneMagnification(Global::instance().magnification());
			Global::instance().setGlobalCurMagnification(1);
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
			//else if(mat._14 * normalScale > width_ - (width_ / t.scaling().x * normalScale))
			else if(mat._14 * normalScale > width - (width / currentScale * normalScale))
			{
				Matrix m = mat;
				//m._14 = (width_ - (width_ / t.scaling().x * normalScale)) / normalScale;
				m._14 = (width - (width / currentScale * normalScale)) / normalScale;
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
			//else if(mat._24 * normalScale > height_ - (height_ / t.scaling().y * normalScale))
			else if(mat._24 * normalScale > height - (height / currentScale * normalScale))
			{
				Matrix m = mat;
				//m._24 = (height_ - (height_ / t.scaling().y * normalScale)) / normalScale;
				m._24 = (height - (height / currentScale * normalScale)) / normalScale;
				m = m.inverse();
				t.setMatrix(m, true);
			}
			pageTweenEndPos_ = t.translation();
		}
	}
	else
	{
		if (scrollDirection_ == Horizontal)
			maxX = width_;
		else
			maxY = height_;
		if (quickFlickLeft || slowLeft)
		{
			pageTweenEndPos_ = Vector3(maxX, maxY, 0);
			targetPageIndex_ = pageIndex_ - 1;
		}
		else if (quickFlickRight || slowRight)
		{
			pageTweenEndPos_ = Vector3(-maxX, -maxY, 0);		
			targetPageIndex_ = pageIndex_ + 1;
		}
	}

	/*if (isQuickFlick)
	{
		if (curPos > startPos && pagePrev())
		{
			pageTweenEndPos_ = Vector3(maxX, maxY, 0);
			targetPageIndex_ = pageIndex_ - 1;
		}
		else if (curPos < startPos && pageNext())
		{
			pageTweenEndPos_ = Vector3(-maxX, -maxY, 0);
			targetPageIndex_ = pageIndex_ + 1;
		}
	}
	else
	{
		if (scenePos > halfPos && pagePrev())
		{
			pageTweenEndPos_ = Vector3(maxX, maxY, 0);
			targetPageIndex_ = pageIndex_ - 1;
		}	
		else if (scenePos < -halfPos && pageNext())		
		{
			pageTweenEndPos_ = Vector3(-maxX, -maxY, 0);
			targetPageIndex_ = pageIndex_ + 1;
		}
	}*/

	if (pageTweenStartPos_.x == pageTweenEndPos_.x &&
		pageTweenStartPos_.y == pageTweenEndPos_.y)
		pageTweenDuration_ = 0;

}

void SceneChanger::setCurPagePos(const Vector3 & pos)
{
	Vector3 pagePos = pos;
	Transform transform;

	float maxX = 0, maxY = 0;
	if (scrollDirection_ == Horizontal)
		maxX = width_;
	else
		maxY = height_;	

	transform = subscene_->transform();
	transform.setTranslation(pagePos.x, pagePos.y, 0);
	subscene_->setTransform(transform);
	float normalScale = Global::instance().magnification();
	float currentScale = subscene_->curSceneMag();
	//if (transform.scaling().x > normalScale && subscene_->isZoomable())
	if (currentScale > normalScale && subscene_->isZoomable())
	{
		//float tempW = maxX * transform.scaling().x / normalScale;
		//float tempH = maxY * transform.scaling().y / normalScale;
		float tempW = maxX * currentScale / normalScale;
		float tempH = maxY * currentScale / normalScale;
		Matrix m = transform.computeMatrix();
		float m14 = 0, m24 = 0;
		if (scrollDirection_ == Horizontal)
			m14 = m._14;
		else
			m24 = m._24;	

		pagePos = Vector3(-maxX , -maxY , 0) + Vector3(m14, m24, 0);
		transform = subscenePrev_->transform();
		transform.setTranslation(pagePos.x, pagePos.y, 0);
		subscenePrev_->setTransform(transform);	

		pagePos = Vector3(tempW , tempH , 0) + Vector3(m14, m24, 0);
		transform = subsceneNext_->transform();
		transform.setTranslation(pagePos.x, pagePos.y, 0);
		subsceneNext_->setTransform(transform);
	}
	else
	{
		pagePos = pos + Vector3(-maxX, -maxY, 0);
		transform = subscenePrev_->transform();
		transform.setTranslation(pagePos.x, pagePos.y, 0);
		subscenePrev_->setTransform(transform);	

		pagePos = pos + Vector3(maxX, maxY, 0);
		transform = subsceneNext_->transform();
		transform.setTranslation(pagePos.x, pagePos.y, 0);
		subsceneNext_->setTransform(transform);		
	}
}

bool SceneChanger::doPageChangedEvent(int index)
{
	static SceneChangerPageChangedEvent event;

	event.setIndex(index);

	return handleEvent(&event, Global::currentTime());

}

bool SceneChanger::shouldAcceptInput() const
{
	bool pageTransition;

	if (transitionMode_ == PageFlipMode || transitionMode_ == PageFlipOrigMode)
	{
		pageTransition = pageFlip_->curTransitionDirection() != TransitionDirectionNone;
	}
	else
	{
		pageTransition = isPageTweenHappening() || subscene_->isTweenWorking();
	}

	return !pageTransition;
}

Vector2 SceneChanger::convertToChildSceneCoords(const Vector2 & coords) const
{	
	return subscene_->convertToChildSceneCoords(coords);
}

Vector2 SceneChanger::convertFromChildSceneCoords(const Vector2 & coords) const
{	
	return subscene_->convertFromChildSceneCoords(coords);
}


bool SceneChanger::pressEvent(const Vector2 & screenCoords, int pressId)
{
	PressEventData & pressEvent = pressEvents_[pressId];
	if (!shouldAcceptInput()) 
	{
		pressEvent.pressed = PressEventData::StrayPress;
		return true;
	}
	
	Ray mouseRay = parentScene_->camera()->unproject(
		parentScene_->screenToDeviceCoords(screenCoords));
	Vector3 startPt;
	intersectRect(&startPt, mouseRay);
	transMatrix_ = parentTransform() * visualAttrib_.transformMatrix();
	invTransMatrix_ = transMatrix_.inverse();
	startPt = invTransMatrix_ * startPt;

	startPagePos_ = subscene_->transform().translation();

	pressEvent.pressed = PressEventData::Pressed;
	pressEvent.startedMoving = false;
	pressEvent.pressPt = Vector2(startPt);

	bool handled = subscene_->pressEvent(screenCoords, pressId);
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

	draggingOnMarkers_ = false;
	if (showMarkers_ && cursorOnWhichMarker(startPt) >= 0) draggingOnMarkers_ = true;

	if (pageMovingId_ == -1 && !handled) pageMovingId_ = pressId;
	return true;
}

void SceneChanger::setRangePos(float pos)
{
	if (pos < 0) pos = 0;
	if (pos > 1) pos = 1;
	int numPages = (int)scenes_.size();

	int pageIndex;
	float pixelPos;
	float pagePos;
	float maxPos;
	float pageX = 0, pageY = 0;

	if (scrollDirection_ == Horizontal)
		maxPos = width_;
	else
		maxPos = height_;

	pixelPos = maxPos * (numPages - 1) * pos;			
	pageIndex = (int)(pixelPos / maxPos);
	pagePos = pixelPos - pageIndex*maxPos;
		
	if (pagePos > maxPos / 2)
	{
		pageIndex++;
		pagePos -= maxPos;
	}	
		
	int prevPageIndex = pageIndex_;
	setCurPage(pageIndex);
	if (prevPageIndex != pageIndex_);
	notifyPageChanging();

	if (scrollDirection_ == Horizontal)
	{
	setCurPagePos(Vector3(-pagePos, 0, 0));
	}
	else
	{
		setCurPagePos(Vector3(0, -pagePos, 0));
	}

}

float SceneChanger::getCursorRangePos(const Vector3 & pt)
{
	int numScenes = (int)scenes_.size();	

	if (markerLocation_ == Top || markerLocation_ == Bottom)
	{
	float maxWidth = (float)activePageImg_->width();
		if (inactivePageImg_->width() > maxWidth)
			maxWidth = (float)inactivePageImg_->width();

	float minX = 0.5f*width_ - 0.5f*(numScenes * maxWidth) + maxWidth/2;	
	float maxX = minX + (numScenes - 1)*maxWidth;

	float pos = (pt.x - minX)/(maxX - minX);
	if (pos < 0) pos = 0;
	if (pos > 1) pos = 1;
	return pos;	
}
	else
	{
		float maxHeight = (float)activePageImg_->height();
		if (inactivePageImg_->height() > maxHeight)
			maxHeight = (float)inactivePageImg_->height();

		float minY = 0.5f*height_ - 0.5f*(numScenes * maxHeight) + maxHeight/2;	
		float maxY = minY + (numScenes - 1)*maxHeight;

		float pos = (pt.y - minY)/(maxY - minY);
		if (pos < 0) pos = 0;
		if (pos > 1) pos = 1;
		return pos;	
	}
}

int SceneChanger::cursorOnWhichMarker(const Vector3 & pt)
{
	int numScenes = (int)scenes_.size();	

	float maxWidth = (float)activePageImg_->width();
	if (activePageImg_->width() > maxWidth)
		maxWidth = (float)activePageImg_->width();
	float maxHeight = (float)activePageImg_->height();
	if (inactivePageImg_->height() > maxHeight) 
		maxHeight = (float)inactivePageImg_->height();

	float dX = 0;
	float dY = 0;
	float curX, curY;
	
	if (markerLocation_ == Top || markerLocation_ == Bottom)		
		dX = maxWidth;	
	else	
		dY = maxHeight;	
	
	switch (markerLocation_)
		{
	case Top: 
		curX = 0.5f*width_ - 0.5f*(numScenes * maxWidth);	
		curY = 0;	
		break;
	case Bottom: 
		curX = 0.5f*width_ - 0.5f*(numScenes * maxWidth);	
			curY = height_ - maxHeight;		
		break;
	case Left: 
		curX = 0;	
		curY = 0.5f*height_ - 0.5f*(numScenes * maxHeight);	
		break;
	case Right: 
		curX = width_ - maxWidth;
		curY = 0.5f*height_ - 0.5f*(numScenes * maxHeight);
		break;
			}	

	int pageIndex = pageIndex_;
	if (pageIndex < 0) pageIndex = 0;
	for (int i = 0; i < numScenes; ++i)
		{
		float x = curX;
			float w = maxWidth;
			float y = curY;
			float h = maxHeight;

			if (x < pt.x && pt.x < x + w && y < pt.y && pt.y < y + h)
			{
				return i;
			}	

		curX += dX;
		curY += dY;
	}

	return -1;
}

Vector2 SceneChanger::toUnitCoords(const Vector2 & coords) const
{
	float sx = (float)1.0f / width();
	float sy = (float)1.0f / height();
	return Vector2(sx * coords.x, sy * coords.y);
}


bool SceneChanger::moveEvent(const Vector2 & screenCoords, int pressId)
{
	PressEventData & pressEvent = pressEvents_[pressId];
	if (pressEvent.pressed == PressEventData::PassedToChild)
	{
		bool handled = subscene_->moveEvent(screenCoords, pressId);
		if (handled) return true;
		//return handled;
	}
	else
	{
		bool handled = subscene_->moveEvent(screenCoords, pressId);	
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

	pageMovingPos_ = screenCoords;

	Vector3 curPt;
	Ray mouseRay = parentScene_->camera()->unproject(
		parentScene_->screenToDeviceCoords(screenCoords));
	intersectRect(&curPt, mouseRay);
	curPt = invTransMatrix_ * curPt;

	float dragXDelta = curPt.x - pressEvent.pressPt.x;
	float dragYDelta = curPt.y - pressEvent.pressPt.y;
	bool verticalDragged = fabs(dragXDelta) < fabs(dragYDelta);		
	bool horzDragged = !verticalDragged;

	bool wrongDirection;
	if (draggingOnMarkers_)
	{
		wrongDirection =
			(verticalDragged && (markerLocation_ == Top || markerLocation_ == Bottom)) 
			||
			(horzDragged && (markerLocation_ == Left || markerLocation_ == Right));
	}
	else
	{
		wrongDirection = 
			(verticalDragged && scrollDirection_ == Horizontal) 
			||
			(horzDragged && scrollDirection_ == Vertical);
	}		
	
	bool justStartedMoving = false;
	
	if (!pressEvent.startedMoving) 
	{
		if (wrongDirection &&
			(fabs(screenCoords.x - pressEvent.pressPt.x) <
			fabs(screenCoords.y - pressEvent.pressPt.y) && 
			!(subscene_->transform().scaling().x > Global::instance().magnification())))
		{
			//vertical scrolling
			//must pass on event data to parents upstream
			pressEvent.pressed = PressEventData::PassedToParent;
			return false;			
		}

		justStartedMoving = true;
		
		pressEvent.startMovingTime = Global::currentTime();
		pressEvent.startPt = curPt;
		pressEvent.startedMoving = true;
		notifyPageChanging();		
	}

	if (!draggingOnMarkers_)
	{
		if (transitionMode_ == PageFlipMode || transitionMode_ == PageFlipOrigMode)
		{
			if (justStartedMoving)
			{
				Transform transform = subscene_->transform();
				float scalex = subscene_->normalScaleX();
				float scaley = subscene_->normalScaleY();
				if(fabs(scalex - transform.scaling().x) > 0.0001 ||
				   fabs(scaley - transform.scaling().y) > 0.0001)
				{
					transform.setScaling(scalex, scaley, (scalex + scaley)/2);
					transform.setPivot(0.0f, 0.0f, 0.0f);
					transform.setTranslation(0.0f, 0.0f, 0.0f);
					subscene_->setTransform(transform);
				}

				Vector2 unitCoords = toUnitCoords(curPt);
				TransitionDirection pageFlipDir = TransitionDirectionNone;
				if (scrollDirection_ == Horizontal)
				{
					if (pressEvent.pressPt.x > curPt.x && pageIndex_ < scenes_.size() - 1) pageFlipDir = TransitionDirectionRight;
					else if (pressEvent.pressPt.x <= curPt.x  && pageIndex_ > 0) pageFlipDir = TransitionDirectionLeft;
					
					//if (unitCoords.x > 0.5f && pageIndex_ < scenes_.size() - 1) pageFlipDir = TransitionDirectionRight;
					//else if (unitCoords.x <= 0.5f && pageIndex_ > 0) pageFlipDir = TransitionDirectionLeft;
				}
				else
				{
					if (pressEvent.pressPt.y > curPt.y && pageIndex_ < scenes_.size() - 1) pageFlipDir = TransitionDirectionDown;
					else if (pressEvent.pressPt.y <= curPt.y  && pageIndex_ > 0) pageFlipDir = TransitionDirectionUp;
					
				//	if (unitCoords.y > 0.5f && pageIndex_ < scenes_.size() - 1) pageFlipDir = TransitionDirectionDown;
				//	else if (unitCoords.y <= 0.5f && pageIndex_ > 0) pageFlipDir = TransitionDirectionUp;				
				}

				if (pageFlipDir != TransitionDirectionNone)
				{					
				//	pageFlip_->pressEvent(unitCoords, pageFlipDir);
					pageFlip_->pressEvent(toUnitCoords(pressEvent.pressPt), pageFlipDir);

					Transform transform = subsceneNext_->transform();
					transform.setTranslation(0.0f, 0.0f, 0.0f);
					subsceneNext_->setTransform(transform);

					transform = subscenePrev_->transform();
					transform.setTranslation(0.0f, 0.0f, 0.0f);
					subscenePrev_->setTransform(transform);

					Global::instance().showAppLayer(false);
				}
			}

			
			pageFlip_->moveEvent(toUnitCoords(curPt));
		}
		else 
		{
			float curPagePos, dragDelta;
			float dx = 0, dy = 0;
			bool isZoomed = false;

			//if ((subscene_->transform().scaling().x > Global::instance().magnification()))
			if ((subscene_->curSceneMag() > Global::instance().magnification()))
			{
				//dx = curPt.x - pressEvent.startPt.x;
				//dy = curPt.y - pressEvent.startPt.y;
				setCurPagePos(subscene_->operateTranslation(pressId));
				isZoomed = true;
			}
			else 
			{
				if (scrollDirection_ == Horizontal)
				{
					curPagePos = subscene_->transform().translation().x;
					dragDelta = dragXDelta;
					dx = curPt.x - pressEvent.startPt.x;
					if (pageIndex_ == 0 && dx > 0) dx = 0;
					else if (pageIndex_ == scenes_.size() - 1 && dx < 0) dx = 0;
				}
				else
				{
					curPagePos = subscene_->transform().translation().y;
					dragDelta = dragYDelta;
					dy = curPt.y - pressEvent.startPt.y;
				}

				if (!isZoomed)
				{
					//if at the end of scrollable region, pass move event to parent
					//to process potential page change
					bool passOn = false;

					if (pageIndex_ == 0 && curPagePos == 0 && /*dragDelta > 0 &&*/ !wrongDirection && pressEvent.startPt.x < curPt.x)
						passOn = true;

					if (pageIndex_ == (int)scenes_.size() - 1 && /*dragDelta < 0 &&*/ !wrongDirection && pressEvent.startPt.x > curPt.x)
						passOn = true;		

					if (passOn)
					{
						pressEvent.pressed = PressEventData::PassedToParent;
						setCurPagePos(Vector3(0, 0, 0));
						return false;
					}
				}


				//change page position according to drag		
				setCurPagePos(startPagePos_ + Vector3(dx, dy, 0));
			}
		}
	}
	else
	{
		float pos = getCursorRangePos(curPt);
		setRangePos(pos);
	}
	redrawTriggered_ = true;
	return true;
}

bool SceneChanger::releaseEvent(const Vector2 & screenCoords, int pressId)
{
	PressEventData & pressEvent = pressEvents_[pressId];

	if (pressEvent.pressed == PressEventData::PassedToParent) return false;	
	if (pressEvent.pressed == PressEventData::StrayPress) return true;
	if (pressEvent.pressed == PressEventData::NotPressed) return false;

	if (pressId == pageMovingId_)
	{
		Ray mouseRay = parentScene_->camera()->unproject(
			parentScene_->screenToDeviceCoords(screenCoords));
		Vector3 pt;
		intersectRect(&pt, mouseRay);
		invTransMatrix_ = 
			(parentTransform() * visualAttrib_.transformMatrix()).inverse();
		pt = invTransMatrix_ * pt;	

		pageMovingId_ = -1;

		if (transitionMode_ == PageFlipMode || transitionMode_ == PageFlipOrigMode)
		{
			pageFlip_->releaseEvent(toUnitCoords(pt));
				
			switch (pageFlip_->curTransitionDirection())
			{
			case TransitionDirectionLeft:
			case TransitionDirectionUp:
				targetPageIndex_ = pageIndex_ - 1;
				break;
			case TransitionDirectionRight:
			case TransitionDirectionDown:
				targetPageIndex_ = pageIndex_ + 1;				
				break;
			}
		}
	//	else
	//	{
			if (hasPageBeenMoved()) doPageSnapBack(pressEvent, pt);
	//	}
	}

	bool handled = subscene_->releaseEvent(screenCoords, pressId);
	redrawTriggered_ = true;	

	if (!handled) handled = SceneObject::releaseEvent(screenCoords, pressId);

	return true;
}

void SceneChanger::pageModeClickEvent(const Vector3 & pt, const Ray & mouseRay)
{
	
}

bool SceneChanger::clickEvent(const Vector2 & pos, int pressId)
{
	if (!shouldAcceptInput())
	{
		return false;
	}

	bool handled = subscene_->clickEvent(pos, pressId);
	
	if (handled) return true;


	Ray mouseRay = parentScene_->camera()->unproject(
		parentScene_->screenToDeviceCoords(pos));
	Vector3 pt;
	intersectRect(&pt, mouseRay);
	transMatrix_ = parentTransform() * visualAttrib_.transformMatrix();
	invTransMatrix_ = transMatrix_.inverse();
	pt = invTransMatrix_ * pt;

	if (showMarkers_ && allowDrag_)
	{
		int index = cursorOnWhichMarker(pt);
		if (index >= 0)
		{				
			sceneChangeTo(scenes_[index]);					
			return true;
		}
	}
	
	return SceneObject::clickEvent(pos, pressId);
}

bool SceneChanger::doubleClickEvent(const Vector2 & pos, int pressId)
{
	if (!shouldAcceptInput())
	{
		return false;
	}

	//setTempSubSceneTransform();
	bool handled = subscene_->doubleClickEvent(pos, pressId);
	//resetSubSceneTransform();
	return handled;
}


bool SceneChanger::update(float sceneTime)
{
	bool needRedraw = false;
	needRedraw |= SceneObject::update(sceneTime);
	
	if (!subscene_->isLoaded())
		needRedraw = true;
	else
		needRedraw |= subscene_->update(sceneTime);

	if (redrawTriggered_)
	{
		redrawTriggered_ = false;
		needRedraw = true;
	}
	
	if (transitionMode_ == PageFlipMode || transitionMode_ == PageFlipOrigMode)
	{
		transDir_ = pageFlip_->curTransitionDirection();
		
		if (transDir_ == TransitionDirectionNone && prevTransDir_ != transDir_) 
		{			
			if (pageFlip_->doPageChange()) setCurPage(targetPageIndex_);
			Global::instance().showAppLayer(true);		
		}		

		prevTransDir_ = transDir_;

		needRedraw |= pageFlip_->update();
	}
//	else 
//	{
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
//	}


	return needRedraw;
}

bool SceneChanger::asyncLoadUpdate() 
{
	bool everythingLoaded = SceneObject::asyncLoadUpdate();
	
	//Commenting out. Want to report as loaded, even if SceneChanger subscenes
	//aren't all loaded. This is because, the scene changer object itself will 
	//display a loading page, so the parent document template doesn't have to.
	/*
	everythingLoaded &= subscene_->asyncLoadUpdate();
	if (subscenePrev_)everythingLoaded &= subscenePrev_->asyncLoadUpdate();
	if (subsceneNext_)everythingLoaded &= subsceneNext_->asyncLoadUpdate();
	*/

	subscene_->asyncLoadUpdate();
	if (subscenePrev_)subscenePrev_->asyncLoadUpdate();
	if (subsceneNext_)subsceneNext_->asyncLoadUpdate();

	if (activePageImg_)
	{
	everythingLoaded &= activePageImg_->asyncLoadUpdate();
	everythingLoaded &= inactivePageImg_->asyncLoadUpdate();	
	}
	
	return everythingLoaded;
}

bool SceneChanger::isLoaded() const
{
	//bug fix, want to allow running the scene (including querying loaded
	//status) without having called init
	if (!gl_) return false;

	return 
		subscene_->isLoaded() && 
		subscenePrev_->isLoaded() && 
		subsceneNext_->isLoaded() &&
		activePageImg_->isLoaded() &&
		inactivePageImg_->isLoaded();
}

void SceneChanger::drawMarkers(GfxRenderer * gl) const
{
	gl->useTextureProgram();
	gl->setTextureProgramOpacity(1);

	int numScenes = (int)scenes_.size();	
	
	float maxWidth = (float)activePageImg_->width();
	if (activePageImg_->width() > maxWidth)
		maxWidth = (float)activePageImg_->width();
	float maxHeight = (float)activePageImg_->height();
	if (inactivePageImg_->height() > maxHeight) 
		maxHeight = (float)inactivePageImg_->height();

	float dX = 0;
	float dY = 0;
	float curX, curY;
	
	if (markerLocation_ == Top || markerLocation_ == Bottom)		
		dX = maxWidth;	
	else	
		dY = maxHeight;	
	
	switch (markerLocation_)
	{
	case Top: 
		curX = 0.5f*width_ - 0.5f*(numScenes * maxWidth);	
		curY = 0;	
		break;
	case Bottom: 
		curX = 0.5f*width_ - 0.5f*(numScenes * maxWidth);	
		curY = height_ - maxHeight;	
		break;
	case Left: 
		curX = 0;	
		curY = 0.5f*height_ - 0.5f*(numScenes * maxHeight);	
		break;
	case Right: 
		curX = width_ - maxWidth;
		curY = 0.5f*height_ - 0.5f*(numScenes * maxHeight);
		break;
	}
	
	gl->setTextureProgramOpacity(totalOpacity());
	int pageIndex = pageIndex_;
	if (pageIndex < 0) pageIndex = 0;
	for (int i = 0; i < numScenes; ++i)
	{
		if (i == pageIndex) 
		{
			gl->use(activePageImg_.get());
			gl->drawRect(curX, curY,
				activePageImg_->width(), activePageImg_->height());
		}
		else 
		{
			gl->use(inactivePageImg_.get());
			gl->drawRect(curX, curY,
				(float)inactivePageImg_->width(), (float)inactivePageImg_->height());
		}
		curX += dX;
		curY += dY;		
	}
}

void SceneChanger::sceneChangeTo(int index)
{
	if (index < 0 || index >= (int)scenes_.size()) return;
	if (index == pageIndex_) return;
	
	if ((transitionMode_ == PageFlipMode || transitionMode_ == PageFlipOrigMode) && pageFlip_)
	{
		//trigger release if user has grabbed and dragged the page but not released
		if (pageMovingId_ >= 0) 
		{
			PressEventData & pressEvent = pressEvents_[pageMovingId_];			
			releaseEvent(pageMovingPos_, pageMovingId_);
			pressEvent.pressed = PressEventData::StrayPress;			
		}

		//if in the middle of a page transition, just reset it (if it was to another page)
		else if (pageFlip_->curTransitionDirection() != TransitionDirectionNone)
		{
			if (targetPageIndex_ != index)
			{
				pageFlip_->reset();
				return sceneChangeTo(index);
			}
			else return;
		}

		int prevTargetPageIndex = targetPageIndex_;	
		targetPageIndex_ = index;		
			
		if (targetPageIndex_ < pageIndex_)
		{
			subscenePrev_->setScene(this, scenes_[targetPageIndex_]);
			subscenePrev_->preStart(Global::currentTime());		
			Transform transform = subscenePrev_->transform();
			transform.setTranslation(0.0f, 0.0f, 0.0f);
			subscenePrev_->setTransform(transform);

			if (scrollDirection_ == Horizontal)
				pageFlip_->animate(TransitionDirectionLeft);
			else
				pageFlip_->animate(TransitionDirectionUp);
		}
		else
		{
			subsceneNext_->setScene(this, scenes_[targetPageIndex_]);
			subsceneNext_->preStart(Global::currentTime());	
			Transform transform = subsceneNext_->transform();
			transform.setTranslation(0.0f, 0.0f, 0.0f);
			subsceneNext_->setTransform(transform);
			if (scrollDirection_ == Horizontal)
				pageFlip_->animate(TransitionDirectionRight);
			else
				pageFlip_->animate(TransitionDirectionDown);
		}

		
	}
	else
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

		int prevTargetPageIndex = targetPageIndex_;	
		targetPageIndex_ = index;
		
		float maxX = 0, maxY = 0;
		if (scrollDirection_ == Horizontal) maxX = width_;
		else maxY = height_;
		
		if (targetPageIndex_ < pageIndex_)
		{
			pageTweenEndPos_ = Vector3(maxX, maxY, 0);		
			if (preloadAdjacent_)
			{
				subscenePrev_->setScene(this, scenes_[targetPageIndex_]);
				subscenePrev_->preStart(Global::currentTime());
			}
		}
		else
		{
			pageTweenEndPos_ = Vector3(-maxX, -maxY, 0);
			if (preloadAdjacent_)
			{
				subsceneNext_->setScene(this, scenes_[targetPageIndex_]);
				subsceneNext_->preStart(Global::currentTime());
			}
		}

		pageTweenStartTime_ = Global::currentTime();
		pageTweenDuration_ = 0.3f;
		pageTweenStartPos_ = subscene_->transform().translation();
	}
	

	notifyPageChanging();
	redrawTriggered_ = true;

}

void SceneChanger::sceneChangeTo(Scene * scene)
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

Scene * SceneChanger::prevScene() const
{
	if (pageIndex_ <= 0) return NULL;
	return scenes_[pageIndex_ - 1];
}

Scene * SceneChanger::nextScene() const
{
	if (pageIndex_ >= (int)scenes_.size() - 1) return NULL;
	return scenes_[pageIndex_ + 1];
}

void SceneChanger::sceneSet(Scene * scene)
{
	bool sceneFound = false;
	int index = -1;
	for (int i = 0; i < (int)scenes_.size(); ++i)
	{
		if (scenes_[i] == scene)
		{
			index = i;
			sceneFound = true;
		}
	}

	if (!sceneFound) return;

	sceneSet(index);
}

void SceneChanger::sceneSet(int index)
{
	notifyPageChanging();
	setCurPage(index);
}

void SceneChanger::notifyPageChanging()
{
	Scene * curPageScene = subscene_->scene();
	
	if (curPageScene)
	{
		parentScene_->parentDocument()->broadcastMessage(
			curPageScene, pageChangingMsg_);	
		Global::instance().cameraOff();
		static PageChangingEvent event;
		curPageScene->root()->handleEvent(&event, Global::currentTime());
	}

}

const bool & SceneChanger::transparentBg() const 
{
	return subscene_->transparentBg();
}

void SceneChanger::setTransparentBg(const bool & transparentBg) 
{
	subscene_->setTransparentBg(transparentBg);
	subscenePrev_->setTransparentBg(transparentBg);
	subsceneNext_->setTransparentBg(transparentBg);
}

void SceneChanger::drawSubscene(GfxRenderer * gl, Subscene * subscene) const
{	
	if (!subscene->isLoaded())
	{
		if (!subscene->transparentBg()) subscene->drawBg(gl);
		const Vector3 & trans = subscene->transform().translation();
		loadingPage_->draw(gl, trans.x, trans.y);
		redrawTriggered_ = true;
	}
	else
	{
		bool isCurPage = subscene == subscene_.get() && parentScene_->isCurrentScene();

		if (isCurPage && !curPageStarted_)
		{
			subscene->start(Global::currentTime());
			curPageStarted_ = true;
		}		

		subscene->draw(gl);
	}
}

void SceneChanger::takeScreenshot(GfxRenderer * gl, Subscene * subscene) const
{
	texRenderer_->beginRenderToTexture(gl);
	drawSubscene(gl, subscene);
	texRenderer_->endRenderToTexture(gl);
}

void SceneChanger::setTransitionMode(const TransitionMode & transitionMode) 
{
	transitionMode_ = transitionMode;	

	delete pageFlip_;
	pageFlip_ = NULL;
	if (transitionMode_ == PageFlipMode) 
	{
		pageFlip_ = new PageFlip;
	}
	else if (transitionMode_ == PageFlipOrigMode)
	{
		pageFlip_ = new PageFlipOrig;
	}

	if (gl_)
	{
		if (transitionMode_ != SlidingMode)
		{
			pageFlip_->init(gl_);
			initTextureRendering(gl_);
		}
		else uninitTextureRendering();
	}

}
void gfxLog(const std::string & str);
void SceneChanger::drawPageFlip(GfxRenderer * gl, Subscene * subscene) const
{	
	Document * doc = parentScene()->parentDocument();	

	if (this->transform().isIdentity() && width_ == doc->origWidth() && height_ == doc->origHeight())
	{
		drawSubscene(gl, subscene);
		pageFlip_->draw(gl);
	}
	else
	{	
		texRenderer2_->beginRenderToTexture(gl);		
		glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		gl->pushMatrix();
		gl->multMatrix(Matrix::Scale(1.0f, -1.0f, 1.0f) * Matrix::Translate(0.0f, -height_, 0.0f));
		drawSubscene(gl, subscene);
		gl->popMatrix();
		pageFlip_->draw(gl);				
		texRenderer2_->endRenderToTexture(gl);

		gl->useTextureProgram();
		gl->setTextureProgramOpacity(1.0f);				
		gl->use(renderTex2_);				
		gl->drawRect(0.0f, height_, width_, -height_);
	}
}

void SceneChanger::drawObject(GfxRenderer * gl) const
{	
	int prevDstRgb, prevDstA, prevSrcRgb, prevSrcA;
	
	
	gl->beginIncStencilDrawing();
	gl->useColorProgram();
	gl->setColorProgramColor(0,0,0,0);
	gl->drawRect(0, 0, width_, height_);
	gl->endStencilDrawing();
	

	if (transitionMode_ == PageFlipMode || transitionMode_ == PageFlipOrigMode)
	{
		if (transDir_ != TransitionDirectionNone)
		{
			if (transDir_ == TransitionDirectionLeft || transDir_ == TransitionDirectionUp)
			{
				//for when preloadAdjacent_ = false
				if (!subscenePrev_->scene()) subscenePrev_->setScene(const_cast<SceneChanger *>(this), pagePrev());

				subscenePrev_->preStart(Global::currentTime());

				if (pageFlip_->texture() == NULL && subscenePrev_->isLoaded()) 
				{										
					takeScreenshot(gl, subscenePrev_.get());
					pageFlip_->setTexture(renderTex_);
				}	

				drawPageFlip(gl, subscene_.get());				
			}
			else
			{
				if (pageFlip_->texture() == NULL) 
				{										
					takeScreenshot(gl, subscene_.get());
					pageFlip_->setTexture(renderTex_);
				}

				//for when preloadAdjacent_ = false
				if (!subsceneNext_->scene())
				{
					subsceneNext_->setScene(const_cast<SceneChanger *>(this), pageNext());
					subsceneNext_->preStart(Global::currentTime());
				}

				drawPageFlip(gl, subsceneNext_.get());				
			}
		}
		else
		{
			if (pageFlip_->texture()) pageFlip_->setTexture(NULL);
			subscene_->setMode(Subscene::ModeFixed);
			drawSubscene(gl, subscene_.get());
		}
	}
	else
	{
		
		float posCoord;
		float maxPosCoord;
		float maxPosX = 0;
		float maxPosY = 0;
		if (scrollDirection_ == Horizontal)
		{
			posCoord = subscenePrev_->transform().translation().x;
			maxPosCoord = width_;
			maxPosX = width_;
		}
		else
		{
			posCoord = subscenePrev_->transform().translation().y;
			maxPosCoord = height_;
			maxPosY = height_;
		}

		bool drawPrev = posCoord > -maxPosCoord;
		bool drawNext = subsceneNext_.get();

		if (drawPrev) drawSubscene(gl, subscenePrev_.get());
		else if (drawNext) drawSubscene(gl, subsceneNext_.get());	

		if (drawPrev || drawNext) subscene_->setMode(Subscene::ModeScrollable);
		else subscene_->setMode(Subscene::ModeFixed);	
		drawSubscene(gl, subscene_.get());
	
	}


	bool showMarkers = showMarkers_ &&
		activePageImg_->isLoaded() && inactivePageImg_->isLoaded();

	if (showMarkers) drawMarkers(gl);


	gl->beginDecStencilDrawing();
	gl->useColorProgram();
	gl->setColorProgramColor(0,0,0,0);
	gl->drawRect(0, 0, width_, height_);
	gl->endStencilDrawing();
	
}

void SceneChanger::start(float docTime)
{
	SceneObject::start(docTime);
	//Commenting out prevAffect stuff: watch out for regression bugs resulting from this
	//bool prevAffect = linkWithViewerLevelObjects_;
	//linkWithViewerLevelObjects_ = false;
	
	//setCurPage(0);		
	doCurPageStart();

	//linkWithViewerLevelObjects_ = prevAffect;
	
	if (pageIndex_ == -1 && !isPageTweenHappening()) setCurPage(0);
}

void SceneChanger::preStart(float docTime)
{
	SceneObject::preStart(docTime);
	if (subscene_)subscene_->preStart(docTime);
	if (pageIndex_ == -1) setCurPage(0);
}

void SceneChanger::stop()
{
	if (subscene_)subscene_->stop();
}


BoundingBox SceneChanger::extents() const
{
	return BoundingBox(Vector3(0, 0, 0), Vector3(width_, height_, 0));
}


SceneObject * SceneChanger::intersect(Vector3 * intPt, const Ray & ray)
{
	if (!visualAttrib_.isVisible()) return 0;
	if (intersectRect(intPt, ray)) return this;
	else return 0;
}

bool SceneChanger::intersectRect(Vector3 * out, const Ray & ray)
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

void SceneChanger::remapReferences(const ElementMapping & elementMapping)
{
	SceneObject::remapReferences(elementMapping);


	vector<Scene *> tempScenes = scenes_;
	scenes_.clear();

	BOOST_FOREACH(Scene * scene, tempScenes)
	{
		Scene * mapped = elementMapping.mapScene(scene);
		if (mapped)
			scenes_.push_back(mapped);
	}


}


void SceneChanger::referencedFiles(std::vector<std::string> * refFiles) const
{	
	SceneObject::referencedFiles(refFiles);
	if (showMarkers_)
	{
		refFiles->push_back(activeMarkerFileName_);
		refFiles->push_back(inactiveMarkerFileName_);
	}
}

int SceneChanger::setReferencedFiles(
	const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);
	if (showMarkers_)
	{
		activeMarkerFileName_ = refFiles[index++];
		inactiveMarkerFileName_ = refFiles[index++];
	}
	return index;
}

bool SceneChanger::dependsOn(Scene * scene) const
{
	BOOST_FOREACH(Scene * childScene, scenes_)
	{
		if (childScene == scene || childScene->dependsOn(scene))
			return true;
	}

	return false;
}

void SceneChanger::dependsOnScenes(std::vector<Scene *> * dependsOnScenes, bool recursive) const
{
	BOOST_FOREACH(Scene * childScene, scenes_)
	{
		dependsOnScenes->push_back(childScene);
		if (recursive) childScene->dependsOnScenes(dependsOnScenes);
	}
}

AppObjectOffset SceneChanger::appObjectOffset() const
{
	AppObjectOffset ret;	
	

	if (linkWithViewerLevelObjects_)
	{
		const Vector3 & trans = subscene_->transform().translation();

		Matrix m = parentTransform(); 
		m *= visualAttrib_.transformMatrix();

		ret.dx = m._11 * trans.x + m._14;
		ret.dy = m._22 * trans.y + m._24;
		ret.sx = m._11;
		ret.sy = m._22;

		ret.clip = true;


		float localWidth = width_ - fabs(trans.x);
		float localHeight = height_ - fabs(trans.y);

		ret.clipX = m._11 * max(trans.x, 0.0f) + m._14;
		ret.clipY = m._22 * max(trans.y, 0.0f) + m._24;
		ret.clipWidth = m._11 * localWidth;
		ret.clipHeight = m._22 * localHeight;	
	}

	return ret;
}

bool SceneChanger::isShowing(Scene * scene) const
{	
	if (scenes_.empty()) return false;	
	Scene * childScene = subscene_->scene();
	if (!childScene) return false;

	if (childScene == scene || childScene->isShowing(scene))
		return true;
	else
		return false;
}


void SceneChanger::showingScenes(std::set<Scene *> * showingScenes) const
{
	if (scenes_.empty()) return;
	if (pageIndex_ < 0) return; //crash fix
	Scene * childScene = scenes_[pageIndex_];
	if (childScene)
	{
		showingScenes->insert(childScene);
		childScene->showingScenes(showingScenes);
	}
}

void SceneChanger::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");	
	writer.write(width_, "width");
	writer.write(height_, "height");
	writer.write(scenes_, "scenes");	

	writer.write(showMarkers_, "showMarkers");
	writer.write(activeMarkerFileName_, "activeMarker");
	writer.write(inactiveMarkerFileName_, "inactiveMarker");

	writer.write(allowDrag_, "allowDrag");
	writer.write(pageChangingMsg_, "pageChangingMsg");

	bool transparentBg = subscene_->transparentBg();
	writer.write(transparentBg, "transparentBg");

	writer.write(linkWithViewerLevelObjects_, "linkWithViewerLevelObjects_");

	writer.write(scrollDirection_);
	writer.write(markerLocation_);
	writer.write(markerOffsetX_);
	writer.write(markerOffsetY_);
	writer.write(preloadAdjacent_);

	writer.write(transitionMode_);
	writer.write(cameraMode_);
}

void SceneChanger::writeXml(XmlWriter & w) const
{	
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();

	w.writeTag("Width", width_);
	w.writeTag("Height", height_);

	w.startTag("Scenes");
	for (int i = 0; i < (int)scenes_.size(); ++i)
	{
		w.writeTag("Scene", w.idMapping().getId(scenes_[i]));
	}
	w.endTag(); //scenes
	
	w.writeTag("ShowMarkers", showMarkers_);
	w.writeTag("ActiveMarkerFileName", activeMarkerFileName_);
	w.writeTag("InactiveFileName", inactiveMarkerFileName_);
	w.writeTag("AllowDrag", allowDrag_);
	w.writeTag("PageChangingMsg", pageChangingMsg_);
	w.writeTag("TransparentBg", subscene_->transparentBg());
	w.writeTag("LinkWithViewerLevelObjects", linkWithViewerLevelObjects_);
	w.writeTag("ScrollDirection", (unsigned int)scrollDirection_);
	w.writeTag("MarkerLocation", (unsigned int)markerLocation_);
	w.writeTag("MarkerOffsetX", markerOffsetX_);
	w.writeTag("MarkerOffsetY", markerOffsetY_);
	w.writeTag("PreloadAdjacent", preloadAdjacent_);

	w.writeTag("TransitionMode", (unsigned int)transitionMode_);
	w.writeTag("CameraMode", (unsigned int)cameraMode_);

}

void SceneChanger::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);

	bool transBg;
	unsigned int val;

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;	
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
		}
		else if (r.isNodeName(curNode, "Scenes"))
		{
			for(xmlNode * sceneNode = curNode->children; sceneNode; sceneNode = sceneNode->next)
			{		
				if (sceneNode->type != XML_ELEMENT_NODE) continue;
				if(r.getNodeContentIfName(val, sceneNode, "Scene"))
				{
					scenes_.push_back((Scene *)val);
				}
			}
		}
		else if (r.getNodeContentIfName(width_, curNode, "Width"));		
		else if (r.getNodeContentIfName(height_, curNode, "Height"));
		else if (r.getNodeContentIfName(allowDrag_, curNode, "AllowDrag"));
		else if (r.getNodeContentIfName(showMarkers_, curNode, "ShowMarkers"));
		else if (r.getNodeContentIfName(activeMarkerFileName_, curNode, "ActiveMarkerFileName"));
		else if (r.getNodeContentIfName(inactiveMarkerFileName_, curNode, "InactiveFileName"));		
		else if (r.getNodeContentIfName(pageChangingMsg_, curNode, "PageChangingMsg"));
		else if (r.getNodeContentIfName(transBg, curNode, "TransparentBg"))
		{			
			subscene_->setTransparentBg(transBg);
		}
		else if (r.getNodeContentIfName(linkWithViewerLevelObjects_, curNode, "LinkWithViewerLevelObjects"));
		else if (r.getNodeContentIfName(val, curNode, "ScrollDirection"))
		{			
			scrollDirection_ = (ScrollDirection)val;
		}
		else if (r.getNodeContentIfName(val, curNode, "MarkerLocation"))
		{			
			markerLocation_ = (MarkerLocation)val;
		}
		else if (r.getNodeContentIfName(markerOffsetX_, curNode, "MarkerOffsetX"));
		else if (r.getNodeContentIfName(markerOffsetY_, curNode, "MarkerOffsetY"));
		else if (r.getNodeContentIfName(preloadAdjacent_, curNode, "PreloadAdjacent"));
		else if (r.getNodeContentIfName(val, curNode, "TransitionMode"))
		{
			transitionMode_ = (TransitionMode)val;
		}
		else if (r.getNodeContentIfName(val, curNode, "CameraMode"))
		{
			cameraMode_ = (Subscene::CameraMode)val;
		}
	}
}


void SceneChanger::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);
	reader.read(width_);
	reader.read(height_);
	reader.read(scenes_);
	for (int i = 0; i < (int)scenes_.size(); ++i)
		scenes_[i]->setCurContainerObject(this);

	if (version > 0)
	{
		if (version >= 6) reader.read(showMarkers_);		
		reader.read(activeMarkerFileName_);
		reader.read(inactiveMarkerFileName_);
		if (version < 6) showMarkers_ = !activeMarkerFileName_.empty();

	}
	if (version >= 2)
	{
		reader.read(allowDrag_);
	}

	if (version >= 3)
	{
		reader.read(pageChangingMsg_);
	}

	if (version >= 4)
	{
		bool transparentBg;
		reader.read(transparentBg);
		setTransparentBg(transparentBg);
	}

	if (version >= 5)
	{
		reader.read(linkWithViewerLevelObjects_);
	}

	if (version >= 7)
	{
		reader.read(scrollDirection_);
		reader.read(markerLocation_);
		reader.read(markerOffsetX_);
		reader.read(markerOffsetY_);
	}

	if (version >= 8)
	{
		reader.read(preloadAdjacent_);
	}

	if (version >= 9)
	{
		reader.read(transitionMode_);
        delete pageFlip_;
        pageFlip_ = NULL;
        
        if (transitionMode_ == PageFlipMode)
        {
            pageFlip_ = new PageFlip;
        }
        else if (transitionMode_ == PageFlipOrigMode)
        {
            pageFlip_ = new PageFlipOrig;
        }
	}

	if (version >= 10)
	{
		reader.read(cameraMode_);
		setCameraMode(cameraMode_);
	}

	setInitPage();
}

void SceneChanger::resizeText(GfxRenderer * gl)
{
	BOOST_FOREACH(Scene * scene, scenes_)
	{
		scene->resizeText(gl);
	}
}

void SceneChanger::setCameraMode(const Subscene::CameraMode & cameraMode)
{
	cameraMode_ = cameraMode;
	subscene_->setCameraMode(cameraMode);
	subscenePrev_->setCameraMode(cameraMode);
	subsceneNext_->setCameraMode(cameraMode);
}

bool SceneChanger::showPoint(Vector2 & outCoords, Scene * targetscene, const Vector2 & coords)
{

	if (!targetscene) return false;

	BOOST_FOREACH(Scene * scene, scenes_)
	{
		if (scene == targetscene) 
		{
			sceneSet(scene); 
			outCoords = parentTransform() * visualAttrib_.transformMatrix() * Vector3(coords.x, coords.y, 0.0f);
			return true;
		}
	}

	return false;
}

void SceneChanger::getSceneObjectsByID(
	const boost::uuids::uuid & type, std::vector<SceneObject *> * objs) const
{
	BOOST_FOREACH(Scene * scene, scenes_)
	{
		scene->getSceneObjectsByID(type, objs);
	}
}


void SceneChanger::reSize()
{
	subscene_->reSize();
	subscenePrev_->reSize();
	subsceneNext_->reSize();
}

void SceneChanger::requestLocalSaveObject()
{
	BOOST_FOREACH(Scene* scene, scenes_)
	{
		scene->requestLocalSaveObject();
	}
}

Scene * SceneChanger::curScene()
{
	return scenes_[pageIndex_];
}

AppObjectOffset SceneChanger::textFeaturesOffset() const
{
	AppObjectOffset ret;	
	

	const Vector3 & trans = subscene_->transform().translation();
	const Vector3 & pivot = subscene_->transform().pivot();
	float inverse14 = -subscene_->transform().computeMatrix().inverse()._14;
	float inverse24 = -subscene_->transform().computeMatrix().inverse()._24;

	Matrix m = parentTransform(); 
	m *= visualAttrib_.transformMatrix();
				
	ret.sx = m._11 * subscene_->transform().scaling().x;
	ret.sy = m._22 * subscene_->transform().scaling().y;
	ret.dx = ret.sx * inverse14 + m._14;
	ret.dy = ret.sy * inverse24 + m._24;

	ret.clip = true;


	float localWidth = width_ - fabs(trans.x);
	float localHeight = height_ - fabs(trans.y);

	ret.clipX = m._11 * max(trans.x, 0.0f) + m._14;
	ret.clipY = m._22 * max(trans.y, 0.0f) + m._24;
	ret.clipWidth = m._11 * localWidth;
	ret.clipHeight = m._22 * localHeight;	
	
	return ret;
}
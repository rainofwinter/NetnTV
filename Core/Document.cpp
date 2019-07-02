#include "stdafx.h"
#include "Document.h"
#include "Scene.h"
#include "Animation.h"
#include "Writer.h"
#include "Reader.h"
#include "PageFlip.h"
#include "Camera.h"
#include "GfxRenderer.h"
#include "FileUtils.h"
#include "Global.h"
#include "VideoStoppedEvent.h"
#include "ReceiveMessageEvent.h"

#include "DocumentTemplate.h"
#include "SingleSceneDocumentTemplate.h"

#include "AudioPlayer.h"

#include "SceneChangerDocumentTemplate.h"

#include "TextHandler.h" //REMOVE
#include "SceneObject.h"
#include "Text.h"

#include "VideoPlayer.h"

#include "ScriptProcessor.h"
#include "DocumentTemplate.h"

#include "AppObject.h"
#include "CustomObject.h"

#include "AudioObject.h"
#include "VideoObject.h"
#include "PhotoObject.h"

#include "TextFeatures.h"

#include "Xml.h"
#include "ElementMapping.h"


//#define FPS
void gfxLog(const std::string & str);

bool documentEventFromString(const std::string & eventStr, DocumentEvent * event)
{
	if (eventStr == "Close")
		*event = DocumentEventClose;
	else if (eventStr == "External Message")
		*event = DocumentEventExternalMessage;
	else if (eventStr == "Update")
		*event = DocumentEventUpdate;
	else
		return false;

	return true;
}


#ifdef NETNTV_WATERMARK
//nentv watermark
#include "Texture.h"
#include "netntv_img.h"
#endif
int8_t a;
using namespace std;

JSBool Document_testFunc(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Document * thisObj = (Document *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsScene;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsScene)) return JS_FALSE;
	JSObject * proto = JS_GetPrototype(cx, jsScene);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Document_getSceneById(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	const jschar * id;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &id)) return JS_FALSE;

	string idStr = jsCharStrToStdString(id);
	Document * document = (Document *)JS_GetPrivate(cx, obj);
	Scene * scene = document->findScene(idStr);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	if (!scene)
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	else
	{
		//create a JSObject based on the scene object
		JSObject * jsObject = scene->getScriptObject(s);
		jsval objJsVal = OBJECT_TO_JSVAL(jsObject);
		JS_SET_RVAL(cx, vp, objJsVal);


		jsval_layout theValue = *(jsval_layout*)&objJsVal;

		JSObject * obj = JSVAL_TO_OBJECT(objJsVal);
	}

	return JS_TRUE;
}

JSBool Document_getOrigWidth(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(s->document()->origWidth()));
	return JS_TRUE;
}

JSBool Document_getOrigHeight(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(s->document()->origHeight()));
	return JS_TRUE;
}

JSBool Document_getTopScene(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	//create a JSObject based on the scene object
	Scene * scene = s->document()->topScene();
	JSObject * jsObject = scene->getScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsObject));
	return JS_TRUE;
}

JSBool Document_scheduleRedraw(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Document * doc = (Document *)JS_GetPrivate(cx, obj);
	doc->triggerRedraw();	
	return JS_TRUE;
}

JSBool Document_createScene(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	Document * document = (Document *)JS_GetPrivate(cx, obj);
	Scene * newScene = new Scene;
	document->addScene(SceneSPtr(newScene));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsObject = newScene->getScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsObject));

	return JS_TRUE;
}
/*
JSBool Document_createEvent(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	const jschar * jsType;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &jsType)) return JS_FALSE;
	string type = jsCharStrToStdString(jsType);
	Event * event = Global::instance().createEvent(type);
	if (!event)
	{
		JS_ReportError(cx, "Document::createEvent - Invalid type.");
		return JS_FALSE;
	}
	event->setDeleteWithScriptObject(true);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsEvt = event->createScriptObject(s);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsEvt));
	return JS_TRUE;
}
*/
JSBool Document_addEventListener(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Document * d = (Document *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsHandler;
	jschar * jsEvent;
	if (!JS_ConvertArguments(cx, argc, argv, "Wo", &jsEvent, &jsHandler)) return JS_FALSE;

	if (!JS_ObjectIsFunction(cx, jsHandler))
	{
		JS_ReportError(cx, "Document::addEventListener - Parameter must be a function");
		return JS_FALSE;
	}

	std::string event = jsCharStrToStdString(jsEvent);

	bool ret = d->addEventListener(event, jsHandler);
	if (!ret)
	{
		JS_ReportError(cx, "Document::addEventListener - Invalid event type");
		return JS_FALSE;
	}

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Document_removeEventListener(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Document * d = (Document *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	jschar * type;
	JSObject * listener;
	if (!JS_ConvertArguments(cx, argc, argv, "Wo", &type, &listener))
        return JS_FALSE;

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	string typeStr = jsCharStrToStdString(type);
	
	bool removed = d->removeEventListener(typeStr, listener);	
	if (removed) s->unrootJSObject(listener);
	
	return JS_TRUE;
};


JSBool Document_showAppLayer(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSBool jsBool;
	if (!JS_ConvertArguments(cx, argc, argv, "b", &jsBool)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);	
	Global::showAppLayer((bool)jsBool);	
	return JS_TRUE;
};


JSBool Document_getTemplate(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Document * d = (Document *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	DocumentTemplate * templ = d->documentTemplate().get();
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(templ->getScriptObject(s)));
	return JS_TRUE;
};

JSBool Document_getOs(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	std::string ss = Global::instance().getOs();
	const char * str = ss.c_str();
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, str)));
	return JS_TRUE;
}

JSBool Document_getModelName(JSContext * cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	std::string ss = Global::instance().getModelName();
	const char * str = ss.c_str();
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, str)));
	return JS_TRUE;
}

JSBool Document_getOsVersion(JSContext * cx, uintN argc, jsval * vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	std::string ss = Global::instance().getOsVersion();
	const char * str = ss.c_str();
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, str)));
	return JS_TRUE;
}

JSBool Document_getScenes(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Document * d = (Document *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	const vector<SceneSPtr> & scenes = d->scenes();
	
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

JSBool Document_setScene(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Document * d = (Document *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jschar *jsId;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &jsId)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	std::string id = jsCharStrToStdString(jsId);

	Scene * scene = d->findScene(id);
	d->sceneSet(scene);

	return JS_TRUE;
}

JSBool Document_stopAllAudio(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Global::instance().audioPlayer()->stopAll();
	return JS_TRUE;
}

JSBool Document_cameraState(JSContext *cx, uintN argc, jsval *vp)
{
	Document * d = (Document *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(d->cameraState()));

	return JS_TRUE;
};

JSFunctionSpec DocumentFuncs[] = {	
	JS_FS("getSceneById",   Document_getSceneById, 1, 0),
	JS_FS("testFunc",   Document_testFunc, 1, 0),
	JS_FS("triggerRedraw",   Document_scheduleRedraw, 0, 0),
	JS_FS("createScene",   Document_createScene, 0, 0),
	JS_FS("addEventListener", Document_addEventListener, 2, 0),	
	JS_FS("removeEventListener",   Document_removeEventListener, 2, 0),
	//move createEvent to SceneObject, so that events can be identified by both object type and event name
	//JS_FS("createEvent", Document_createEvent, 1, 0),
	JS_FS("showAppLayer", Document_showAppLayer, 1, 0),
	JS_FS("getTopScene", Document_getTopScene, 0, 0),
	JS_FS("getTemplate", Document_getTemplate, 0, 0),	

	JS_FS("getOrigWidth", Document_getOrigWidth, 0, 0),
	JS_FS("getOrigHeight", Document_getOrigHeight, 0, 0),
	JS_FS("getOs", Document_getOs, 0, 0),
	JS_FS("getModelName", Document_getModelName, 0, 0),
	JS_FS("getOsVersion", Document_getOsVersion, 0, 0),
	JS_FS("getScenes", Document_getScenes, 0, 0),
	JS_FS("setScene", Document_setScene, 1, 0),
	JS_FS("stopAllAudio", Document_stopAllAudio, 0, 0),
	JS_FS("cameraState", Document_cameraState, 0, 0),
    JS_FS_END
};



///////////////////////////////////////////////////////////////////////////////


void Document::gcMark(JSTracer * trc, JSObject * obj)
{
	JSContext * cx = trc->context;
	Document * o = (Document *)JS_GetPrivate(cx, obj);
	if (o)
	{
		EventListeners::iterator iter;		
		for (iter = o->eventListeners_.begin(); iter != o->eventListeners_.end(); ++iter)
		{				
			JS_TRACER_SET_DEBUG_VARS(trc)
			JS_CallTracer(trc, (*iter).second, JSTRACE_OBJECT);			
		}
		
		BOOST_FOREACH(SceneSPtr & scene, o->scenes_)
		{
			if (scene->scriptObject_)
			{
				JS_TRACER_SET_DEBUG_VARS(trc)
				JS_CallTracer(trc, scene->scriptObject_, JSTRACE_OBJECT);
			}
		}		

		Scene * topScene = o->topScene();
		if (topScene->scriptObject_)
		{
			JS_TRACER_SET_DEBUG_VARS(trc)
			JS_CallTracer(trc, topScene->scriptObject_, JSTRACE_OBJECT);
		}

		DocumentTemplate * templ = o->documentTemplate().get();
		if (templ->scriptObject_)
		{
			JS_TRACER_SET_DEBUG_VARS(trc)
			JS_CallTracer(trc, templ->scriptObject_, JSTRACE_OBJECT);
		}
	}
}


ContentType curContentType = ContentTypeInternal;

//#ifdef ANDROID
//const float origMinMoveDist = 8;
//#else
const float origMinMoveDist = 16;
//#endif

Document::Document()
{
	create();
}

Document::Document(int width, int height)
{
	create();
	origWidth_ = width;
	origHeight_ = height;
}

Document::~Document()
{
	if (scriptProcessor_->isInit())
	{
		JSContext * cx = scriptProcessor_->jsContext();
		JSObject * evtObject = JS_NewObject(cx, 0, 0, 0);
		/*
		jsval val;
		val = BOOLEAN_TO_JSVAL(ev.lengthComputable);
		JS_SetProperty(cx, evtObject, "lengthComputable", &val);
		val = DOUBLE_TO_JSVAL(ev.loaded);
		JS_SetProperty(cx, evtObject, "loaded", &val);
		val = DOUBLE_TO_JSVAL(ev.total);
		JS_SetProperty(cx, evtObject, "total", &val);*/
		
		fireEvent(DocumentEventClose, evtObject);
	}

	if (!textFeaturesFileName_.empty())	tf_->saveXml(textFeaturesFileName_);

	uninit();
	BOOST_FOREACH(TempRenderObject * obj, tempRenderObjects_)
		delete obj;	
	
	delete scriptProcessor_;
	delete gl_;

#ifdef NETNTV_WATERMARK
	delete netntvTexture_;
#endif
}


void Document::create()
{
	docTime_ = 0;
	Scene * firstScene = new Scene;
	addScene(SceneSPtr(firstScene));
	gl_ = 0;

	width_ = 768;
	height_ = 1024;

	origWidth_ = origHeight_ = 0;

	prevSingleClickTime_ = -FLT_MAX;
	prevClickPos_ = Vector2(-FLT_MAX, -FLT_MAX);

	orientation_ = OrientationVertical;
	redrawTriggered_ = false;

	SingleSceneDocumentTemplate * singTemp = new SingleSceneDocumentTemplate();
	singTemp->setDocument(this);
	singTemp->setScene(firstScene);
		
	template_.reset(singTemp);

	curSceneScaleX_ = curSceneScaleY_ = 1;

	globalVersion_ = Global::version();

	minMoveDist_ = origMinMoveDist;

	scriptProcessor_ = new ScriptProcessor(this);
	gl_ = new GfxRenderer(width_, height_);

	videoPlayingContainers_.clear();
	videoPlayState_ = VideoStateStopped;

	topScene_.reset(new Scene);
	topScene_->setName("Top Scene");
	topScene_->setParentDocument(this);

	scriptsStarted_ = false;

#ifdef NETNTV_WATERMARK
	//netntv watermark
	netntvTexture_ = new Texture;	
#endif

	resizeText_ = true;

	allowMultitouch_ = false;

	isNaviBarDisplayed_ = true;
	doucumentExit_ = false;

	pausedTime_ = 0;
	checkTime_ = 0;
	isPaused_ = false;

	dpiScale_ = 1.0f;
	tf_.reset(new TextFeatures(this));
	cameraState_ = false;
	Global::instance().setCurDocument(this);
}

bool Document::addEventListener(const std::string & eventStr, JSObject * handler)
{
	DocumentEvent event;
	bool succ = documentEventFromString(eventStr, &event);
	if (!succ) return false;

	eventListeners_.insert(make_pair(event, handler));
	return true;
}

bool Document::removeEventListener(const std::string & eventStr, JSObject * handler)
{
	DocumentEvent event;
	bool succ = documentEventFromString(eventStr, &event);
	if (!succ) return false;

	bool removed = false;
	std::multimap<DocumentEvent, JSObject *>::iterator iter;
	for (iter = eventListeners_.begin(); iter != eventListeners_.end();)
	{
		DocumentEvent curEvent = (*iter).first;
		JSObject * curHandler = (*iter).second;
		
		if (curEvent == event && curHandler == handler)
		{			
			eventListeners_.erase(iter++);			
			removed = true;
		}
		else ++iter;
	}

	return removed;
}

void Document::fireEvent(DocumentEvent event, JSObject * evtObject)
{
	typedef multimap<DocumentEvent, JSObject *>::iterator iterator;
	pair<iterator, iterator> range = eventListeners_.equal_range(event);
	jsval evtVal;
	if (evtObject) evtVal = OBJECT_TO_JSVAL(evtObject);
	for (iterator iter = range.first; iter != range.second; ++iter)
	{
		JSObject * handler = (*iter).second;
		jsval rval;
		jsval funcVal = OBJECT_TO_JSVAL(handler);
		
		if (evtObject)
			JS_CallFunctionValue(scriptProcessor_->jsContext(), 
				scriptProcessor_->jsGlobal(), funcVal, 1, 
				&evtVal, &rval);
		else
			JS_CallFunctionValue(scriptProcessor_->jsContext(), 
				scriptProcessor_->jsGlobal(), funcVal, 0, 
				0, &rval);
	}
}

void Document::setScenes(const DocumentTemplateSPtr & docTemp, const std::vector<SceneSPtr> & scenes)
{
	scenes_ = scenes;
	setDocumentTemplate(docTemp);

	totalScenes();
}

void Document::addScene(const SceneSPtr & scene)
{
	scenes_.push_back(scene);
	scene->setParentDocument(this);

	totalScenes();
}

void Document::deleteScene(const Scene * scene)
{
	vector<SceneSPtr>::iterator iter = scenes_.begin();

	for (; iter != scenes_.end(); ++iter)
	{
		SceneSPtr & curScene = *iter;
		if (curScene.get() == scene) 
		{			
			scenes_.erase(iter);
			break;
		}
	}

	totalScenes();
}

SceneSPtr Document::findScene(const Scene * scene)
{
	vector<SceneSPtr>::iterator iter = scenes_.begin();
	for (; iter != scenes_.end(); ++iter)
	{
		SceneSPtr & curScene = *iter;
		if (curScene.get() == scene) return curScene;
	}
	return SceneSPtr();	
}

Scene * Document::findScene(const string & id) const
{
	vector<SceneSPtr>::const_iterator iter = scenes_.begin();
	for (; iter != scenes_.end(); ++iter)
	{
		const SceneSPtr & curScene = *iter;
		if (curScene->name() == id) return curScene.get();
	}
	return 0;	
}


void Document::remapDirectories(
	const std::string & oldDir, const std::string & newDir)
{
	vector<string> fileNames;
	referencedFiles(&fileNames);

	vector<string> newFileNames;
	BOOST_FOREACH(string fileName, fileNames)
	{
		string absFileName = getAbsFileName(oldDir, fileName);	
		newFileNames.push_back(
			convertToRelativePath(newDir, absFileName));
	}
	
	setReferencedFiles(newDir, newFileNames);	
}

void Document::appObjectBoundingBox(const AppObject * appObject,
	float * x, float * y, float * w, float * h,
	bool * clip, float * clipX, float * clipY, float * clipW, float * clipH) const
{	
	BoundingBox bbox = appObject->worldExtents();
	AppObjectOffset appOff = template_->totalAppObjectOffset(appObject->containerObjects());

	if (appObject->isAffectedByOffset())
	{		
		*x = appOff.sx * bbox.minPt.x + appOff.dx;
		*y = appOff.sy * bbox.minPt.y + appOff.dy;
		*w = appOff.sx * bbox.lengthX();
		*h = appOff.sy * bbox.lengthY();	
	}	
	else
	{
		*x = bbox.minPt.x;
		*y = bbox.minPt.y;
		*w = bbox.lengthX();
		*h = bbox.lengthY();
	}	
		
	*clip = appOff.clip;
	*clipX = appOff.clipX;
	*clipY = appOff.clipY;
	*clipW = appOff.clipWidth;
	*clipH = appOff.clipHeight;	
}

void Document::referencedFiles(vector<string> * refFiles) const
{
	refFiles->clear();

	BOOST_FOREACH(string scriptFile, scriptFiles_)
	{
		refFiles->push_back(scriptFile);
	}

	map<string, string>::const_iterator iter;
	for (iter = scriptAccessibleFiles_.begin(); iter != scriptAccessibleFiles_.end(); ++iter)
	{
		refFiles->push_back((*iter).second);
	}

	template_->referencedFiles(refFiles);
	
	BOOST_FOREACH(SceneSPtr scene, scenes_)
		scene->referencedFiles(refFiles);
}

void Document::setReferencedFiles(
	const std::string & baseDirectory, const vector<string> & refFiles)
{
	int index = 0;

	BOOST_FOREACH(string & scriptFile, scriptFiles_)
	{
		scriptFile = refFiles[index++];
	}

	map<string, string>::iterator iter;
	for (iter = scriptAccessibleFiles_.begin(); iter != scriptAccessibleFiles_.end(); ++iter)
	{
		(*iter).second = refFiles[index++];
	}

	index = template_->setReferencedFiles(baseDirectory, refFiles, index);

	BOOST_FOREACH(SceneSPtr scene, scenes_)
		index = scene->setReferencedFiles(baseDirectory, refFiles, index);

}

#ifdef FPS
float gFrames = 0;
const float cFpsInterval = 1.0f;
float gStartTime = -1.0f;	
#endif

void Document::draw()
{
	
#ifdef FPS
	gFrames++;
#endif

	gl_->beginDraw();

	template_->draw(gl_);

	gl_->pushMatrix();
	gl_->multMatrix(Matrix::Scale(
		width_/topScene_->screenWidth(), height_/topScene_->screenHeight(), 1.0f));

	topScene_->draw(gl_);
	gl_->popMatrix();
	
	
	

#ifdef NETNTV_WATERMARK
	gl_->useTextureProgram();
	gl_->setTextureProgramOpacity(1);
	gl_->use(netntvTexture_);
	gl_->drawRect(5, 5, netntvImgWidth, netntvImgHeight);

#endif
	glDepthFunc(GL_ALWAYS);

	tf_->draw();
	gl_->endDraw();
}

void Document::start()
{
	//noop if already initialized
	if (!scriptsStarted_)
	{
		scriptProcessor_->init();
		BOOST_FOREACH(string scriptFile, scriptFiles_)
		{
			string absScriptFile = getAbsFileName(Global::instance().readDirectory(), scriptFile);
			scriptProcessor_->execute(absScriptFile);
		}
		scriptsStarted_ = true;
	}

	template_->start(docTime_);
	topScene_->start(docTime_);
}

bool Document::isCurrentVideoScene(const ContainerObjects & containerObjects)
{
	if (containerObjects.size() > videoPlayingContainers_.size()) return false;
	if (videoPlayingContainers_.empty()) return false;

	bool ret = true;
	int vIndex = (int)videoPlayingContainers_.size() - 1;
	int cIndex = (int)containerObjects.size() - 1;
	for(;cIndex >= 0;--cIndex, --vIndex)
	{
		ret &= containerObjects[cIndex] == videoPlayingContainers_[vIndex];		
	}

	return ret;
}

bool Document::update()
{
	if (isPaused_) return false;
	docTime_ = Global::currentTime();

#ifdef FPS
	float time = docTime_;
	if (gStartTime < 0.0f) gStartTime = time;	

	if (time - gStartTime > cFpsInterval)
	{
		float fps = gFrames / (time - gStartTime);

		char str[256];
		sprintf(str, "fps: %f", fps);
		gfxLog(str);

		gFrames = 0;		
		gStartTime = -1.0f;
	}
#endif

	if (tf_->update(docTime_)) triggerRedraw();	
	
	bool needToRedraw = false;
	
	if (redrawTriggered_)
	{
		needToRedraw = true;
		redrawTriggered_ = false;
	}
	
	needToRedraw |= template_->update(docTime_);
	template_->asyncLoadUpdate();

	needToRedraw |= topScene_->update(docTime_);
	topScene_->asyncLoadUpdate();	

	vector<TempRenderObject *>::iterator iter = tempRenderObjects_.begin();
	for (; iter != tempRenderObjects_.end();)
	{		
		TempRenderObject * tempRObj = *iter;
		if (!tempRObj->isStarted())
		{
			tempRObj->start(docTime_);
		}
		needToRedraw |= tempRObj->update(docTime_);
		if (tempRObj->isFinished())	
		{
			iter = tempRenderObjects_.erase(iter);
			tempRObj->uninit();
			delete tempRObj;
		}
		else		
			++iter;		
	}	
		
	//TODO scale
	//Global::instance().videoPlayer()->setRegionOffset(offset.dx, offset.dy);

	if (videoPlayState_ == VideoStateRegion)
	{
		AppObjectOffset offset = template_->totalAppObjectOffset(videoPlayingContainers_);
		Global::instance().videoPlayer()->setRegion(
			offset.sx * videoX_ + offset.dx, offset.sy * videoY_ + offset.dy,
			offset.sx * videoW_, offset.sy * videoH_);
	}

	scriptProcessor_->update(docTime_);
	fireEvent(DocumentEventUpdate, NULL);

	return needToRedraw;
}

void Document::playVideo(
	const VideoObject * vidObj, const ContainerObjects & containerObjects, 
	const std::string & sourceName, int x, int y, int width, int height, 
	bool isUrl, bool showControls, float startTime)
{
	if (!videoPlayingContainers_.empty()) Global::instance().videoPlayer()->deactivate();

	videoPlayState_ = VideoStateRegion;
	videoPlayingContainers_ = containerObjects;
	videoX_ = x;
	videoY_ = y;
	videoW_ = width;
	videoH_ = height;
	

	AppObjectOffset offset = template_->totalAppObjectOffset(videoPlayingContainers_);

	Global::instance().videoPlayer()->activateRegion(
		Global::instance().ptrToId((void *)vidObj), sourceName, 
		offset.sx * x + offset.dx, offset.sy * y + offset.dy, 
		offset.sx * width, offset.sy * height, 
		isUrl, showControls, startTime);
}

void Document::playVideoFullscreen(const VideoObject * vidObj, 
	const ContainerObjects & containerObjects, 
	const std::string & fileName, bool isUrl, bool showControls, float startTime)
{
	if (!videoPlayingContainers_.empty()) Global::instance().videoPlayer()->deactivate();

	videoPlayState_ = VideoStateFullscreen;
	videoPlayingContainers_ = containerObjects;
	Global::instance().videoPlayer()->activate(Global::instance().ptrToId((void *)vidObj),
		fileName, isUrl, showControls, startTime);	
}

void Document::videoStoppedEvent()
{
	videoPlayState_ = VideoStateStopped;
	VideoStoppedEvent event;

	if (!videoPlayingContainers_.empty())
	{
		Scene * topScene  = videoPlayingContainers_.front().second;
		//clear videoPlayingContainers_ before handling the event
		//bc one of the events may result in a PlayVideo action
		//and we don't wanna clear videoPlayingContainers_ right after
		//it's been populated during a PlayVideo action.
		videoPlayingContainers_.clear();	

		bool handled = topScene->root()->handleEvent(
			&event, Global::currentTime());	

		if (handled) triggerRedraw();
	}
}

void Document::audioPlayedEvent(int id)
{
	AudioObject * obj = (AudioObject *)Global::instance().idToPtr(id);	
	if (obj) obj->notifyPlayed();
}

void Document::audioStoppedEvent(int id)
{
	AudioObject * obj = (AudioObject *)Global::instance().idToPtr(id);
	if (obj) obj->notifyStopped();
}

void Document::audioPausedEvent(int id)
{
	AudioObject * obj = (AudioObject *)Global::instance().idToPtr(id);
	if (obj) obj->notifyPaused();
}

void Document::audioResumedEvent(int id)
{
	AudioObject * obj = (AudioObject *)Global::instance().idToPtr(id);
	if (obj) obj->notifyResumed();
}

void Document::videoPlayedEvent(int id)
{
	VideoObject * obj = (VideoObject *)Global::instance().idToPtr(id);
	if (obj) obj->notifyPlayed();
}

void Document::videoStoppedEvent(int id)
{
	VideoObject * obj = (VideoObject *)Global::instance().idToPtr(id);
	if (obj) obj->notifyStopped();
}

void Document::videoPausedEvent(int id)
{
	VideoObject * obj = (VideoObject *)Global::instance().idToPtr(id);
	if (obj) obj->notifyPaused();
}

void Document::videoResumedEvent(int id)
{
	VideoObject * obj = (VideoObject *)Global::instance().idToPtr(id);
	if (obj) obj->notifyResumed();
}

void Document::videoSeekedEvent(int id, float time)
{
	VideoObject * obj = (VideoObject *)Global::instance().idToPtr(id);
	if (obj) obj->notifySeeked(time);
}

Vector2 Document::toTopSceneCoords(const Vector2 & coords) const
{
	return Vector2(
		(topScene_->screenWidth() / width_) * coords.x, 
		(topScene_->screenHeight() / height_) * coords.y);
}

void Document::pressEvent(const Vector2 & startPos, int pressId, bool propagate)
{
	if (!allowMultitouch_ && pressId != 0) return;

	PressEventData pressEventData;
	pressEventData.startPos = startPos;

	if (tf_->pressEvent(startPos, pressId))
	{
		pressEventData.eventTarget = PressEventData::TextFeatures;
	}
	else
	{
		bool handled;
		bool isTopScene;

		handled = dispatchPressEvent(&isTopScene, startPos, pressId, propagate);
		if (handled) triggerRedraw();


		if (isTopScene)
			pressEventData.eventTarget = PressEventData::TopScene;
		else
			pressEventData.eventTarget = PressEventData::Template;	
	}

	pressEventMap_[pressId] = pressEventData;
}

void Document::keyPressEvent(int keyCode)
{
	topScene_->keyPressEvent(keyCode);
	template_->keyPressEvent(keyCode);
}

void Document::keyReleaseEvent(int keyCode)
{
	topScene_->keyReleaseEvent(keyCode);
	template_->keyReleaseEvent(keyCode);
}

void Document::appObjectTapEvent(AppObject * appObject)
{
	template_->appObjectTapEvent(appObject);
}

void Document::moveEvent(const Vector2 & curPos, int pressId)
{
	if (!allowMultitouch_ && pressId != 0) return;

	PressEventData & pressData = pressEventMap_[pressId];

	if (tf_->moveEvent(curPos, pressId))
	{

	}
	else
	{
		if (!pressData.startedMoving)
		{
			if ((pressData.startPos - curPos).magnitude() > minMoveDist_)
				pressData.startedMoving = true;
		}

		if (pressData.startedMoving)
		{
			if (dispatchMoveEvent(pressData.eventTarget == PressEventData::TopScene, curPos, pressId)) 
				triggerRedraw();
		}	
	}
	
}


bool Document::dispatchPressEvent(bool * isTopScene, const Vector2 & curPos, int pressId, bool propagate)
{
	bool handled = topScene_->pressEvent(toTopSceneCoords(curPos), pressId);

	if (topScene_->wasPressIntersected(pressId))
		*isTopScene = true;
	else 
	{
		*isTopScene = false;
		handled = template_->pressEvent(curPos, pressId, propagate);
	}

	return handled;	
}


bool Document::dispatchMoveEvent(bool isTopScene, const Vector2 & curPos, int pressId)
{
	if (isTopScene)
		return topScene_->moveEvent(toTopSceneCoords(curPos), pressId);
	else
		return template_->moveEvent(curPos, pressId);	
}


bool Document::dispatchReleaseEvent(bool isTopScene, const Vector2 & curPos, int pressId)
{
	if (isTopScene)
		return topScene_->releaseEvent(toTopSceneCoords(curPos), pressId);
	else
		return template_->releaseEvent(curPos, pressId);
}

bool Document::dispatchClickEvent(bool isTopScene, const Vector2 & curPos, int pressId)
{
	if (isTopScene)
		return topScene_->clickEvent(toTopSceneCoords(curPos), pressId);
	else
		return template_->clickEvent(curPos, pressId);	
}

bool Document::dispatchDoubleClickEvent(bool isTopScene, const Vector2 & curPos, int pressId)
{
	if (isTopScene)
		return topScene_->doubleClickEvent(toTopSceneCoords(curPos), pressId);
	else
		return template_->doubleClickEvent(curPos, pressId);
}

void Document::releaseEvent(const Vector2 & curPos, int pressId)
{
	if (!allowMultitouch_ && pressId != 0) return;	

	PressEventData & pressData = pressEventMap_[pressId];
	

	bool isClick = false;	
	if (!pressData.startedMoving)
		isClick = (curPos - pressData.startPos).magnitude() < minMoveDist_;	

	
	if (tf_->releaseEvent(curPos, pressId, isClick))
	{
	}
	else
	{
		bool isTopScene = pressData.eventTarget == PressEventData::TopScene;
		bool handled = dispatchReleaseEvent(isTopScene, curPos, pressId);	

		if (isClick) 
		{
			float time = Global::currentTime();

			bool fastEnough = time - prevSingleClickTime_ < 0.5f;
			bool closeEnough = (curPos - prevClickPos_).magnitude() < minMoveDist_;
			bool isDblClick = fastEnough && closeEnough;
			if (isDblClick)		
				handled |= dispatchDoubleClickEvent(isTopScene, curPos, pressId);		

			handled |= dispatchClickEvent(isTopScene, curPos, pressId);

			if (!isDblClick)
				prevSingleClickTime_ = time;
			prevClickPos_ = curPos;
		}

		if (handled) triggerRedraw();
	}


	pressEventMap_.erase(pressId);
}

bool Document::isCurrentScene(Scene * scene) const
{
	return 
		scene == topScene_.get() || 
		topScene_->isShowing(scene) || 
		template_->isCurrentScene(scene);
}

void Document::broadcastMessage(Scene * scene, const std::string & msg)
{
	if (scene)
		scene->broadcastMessage(msg);
}

void Document::broadcastExternalMessage(const std::string & message)
{
	ReceiveMessageEvent * event = new ReceiveMessageEvent(message);
	fireEvent(
		DocumentEventExternalMessage, 
		event->createScriptObject(scriptProcessor(), false));
	Global::instance().broadcastExternalMessage(message);
}

std::string Document::getScriptAccessibleFile(const std::string & scriptSideName) const
{
	map<string, string>::const_iterator iter;
	iter = scriptAccessibleFiles_.find(scriptSideName);
	if (iter == scriptAccessibleFiles_.end()) return scriptSideName;
	return (*iter).second;
}


Vector2 Document::screenToDeviceCoords(const Vector2 & pos) const
{
	return Vector2(2*pos.x/origWidth_ - 1, 2*(1 - pos.y/origHeight_) - 1);
}

Vector2 Document::deviceToScreenCoords(const Vector2 & pos) const
{
	return Vector2(
		origWidth_ * 0.5f*(pos.x + 1.0f), origHeight_ * 0.5f * (-pos.y + 1.0f));
}

void Document::registerCustomObject(
	const boost::uuids::uuid & customType, 
	const std::string & name, const std::string & script)
{
	CustomObjectData * data = new CustomObjectData;
	data->setName(name);
	data->setScript(script);

	map<boost::uuids::uuid, boost::shared_ptr<CustomObjectData> >::iterator iter = 
		customObjectData_.find(customType);

	if (iter != customObjectData_.end())
		customObjectData_.erase(iter);

	customObjectData_[customType] = boost::shared_ptr<CustomObjectData>(data);
}

void Document::init()
{		
	uninit();
	
	//REMOVE
	/*
	boost::uuids::string_generator gen;
	boost::uuids::uuid id = gen("99ED2222-3AB7-2799-1717-82D5364987AF");
	
	string scriptFile = "c:/ST/scriptTest/customObject.js";
	FILE * file = 0;	
	std::vector<char> buffer;
	BOOST_SCOPE_EXIT( (&file)) {
		if (file) fclose(file);
    } BOOST_SCOPE_EXIT_END

	file = fopen(scriptFile.c_str(), "rb");
	if (!file) return;
	fseek(file, 0, SEEK_END);	
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	buffer.resize(fileSize);
	fread(&buffer[0], 1, fileSize, file);

	CustomObjectData * data = new CustomObjectData;
	data->setScript(buffer);
	customObjectData_.clear();
	customObjectData_[id] = boost::shared_ptr<CustomObjectData>(data);
	*/

	

	Global::instance().resetLoadThread();

#if defined(IOS) || defined(ANDROID)
	bool isMobile = true;
#else
	bool isMobile = false;
#endif
	gl_->init(isMobile);
	glEnable (GL_BLEND);
	glClearStencil(0);	
	glEnable(GL_STENCIL_TEST);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0 + 0);	
	glEnable(GL_DEPTH_TEST);

	gl_->useColorProgram();	
	template_->setDocument(this);
	template_->setSize(width_, height_);

	topScene_->setScreenWidth(origWidth_);
	topScene_->setScreenHeight(origHeight_);
		
	//for the case of ScriptProcessor, this is a no op if already initialized
	scriptProcessor_->init();

	/*
	templateWidth_ = width_;
	templateHeight_ = height_;
	*/
	template_->init(gl_);	



	topScene_->init(gl_, true);
#ifdef NETNTV_WATERMARK
	netntvTexture_->init(
		gl_, netntvImgWidth, netntvImgHeight, Texture::UncompressedRgba32, netntvImgBits);	
#endif
	
}

void Document::uninit()
{	
	topScene_->uninit();
	template_->uninit();

#ifdef NETNTV_WATERMARK
	netntvTexture_->uninit();
	
#endif
	tf_->uninit();	
}

void Document::setDocumentTemplate(const DocumentTemplateSPtr & docTemplate)
{
	template_ = docTemplate;
	template_->setDocument(this);
}

void Document::sceneChangeTo(Scene * scene)
{
	template_->sceneChangeTo(scene);
}

void Document::sceneSet(Scene * scene)
{
	template_->sceneSet(scene);
}

void Document::insertBefore(const SceneSPtr & newObj, Scene * refObj)
{
	vector<SceneSPtr>::iterator insertIter, removeIter, iter;
	if (newObj.get() == refObj) return;

	removeIter = scenes_.end();
	for (iter = scenes_.begin(); iter != scenes_.end(); ++iter)	
	{
		if (*iter == newObj) removeIter = iter;	
	}
	//if (removeIter == scenes_.end()) throw Exception(InvalidArguments);
	if (removeIter != scenes_.end())scenes_.erase(removeIter);

	insertIter = scenes_.end();	
	for (iter = scenes_.begin(); iter != scenes_.end(); ++iter)	
		if ((*iter).get() == refObj) insertIter = iter;	
	if (insertIter == scenes_.end() && refObj) throw Exception(InvalidArguments);

	scenes_.insert(insertIter, newObj);
}

void Document::setSize(
	int width, int height, float dpiScale, float minMoveDistScale, bool resizeGl)
{
	minMoveDist_ = origMinMoveDist * minMoveDistScale;

	/*
	int desiredWidth = width;
	int desiredHeight = height;
	
	float aspect = (float)origWidth_ / origHeight_;
	
	if (height * aspect <= width)
		width = height * aspect;
	else height = width / aspect;

	width_ = desiredWidth;
	height_ = desiredHeight;
	
	templateWidth_ = width;
	templateHeight_ = height;
	*/

	width_ = width;
	height_ = height;
	dpiScale_ = dpiScale;

	//cout << width << "\t" << height << "\n";

	if (resizeText_)
	{
		float w = width_;
		float ow = origWidth_;
		float h = height_;
		float oh = origHeight_;

		float mag = w / ow;
		if (mag > h / oh)
			mag = h / oh;

		/*
		static boost::uuids::uuid type = Text().type();
		std::vector<SceneObject *> texts;
		BOOST_FOREACH(SceneSPtr scene, scenes_)
		{
			scene->getSceneObjectsByID(type, &texts);		
			BOOST_FOREACH(SceneObject * obj, texts)
			{
				Text* text = (Text*)obj;
				text->resizeText(mag, gl_);
			}
		}
		*/
		if (mag != Global::instance().magnification())
		{
			Global::instance().setMagnification(mag);
			Global::instance().setCurSceneMagnification(mag);
			Global::instance().setGlobalCurMagnification(1);

			template_->reInitCurScene(gl_);
		}
	}

	template_->setDpiScale(dpiScale);
	template_->setSize(width, height, resizeGl);

	//topScene_->setScreenWidth(width);
	//topScene_->setScreenHeight(height);
	
	if (resizeGl) 
	{
		/*
		glViewPortX_ = (desiredWidth - width)/2;
		glViewPortY_ = (desiredHeight - height)/2;
		glViewport(glViewPortX_, glViewPortY_, width, height);
*/
		glViewport(0, 0, width, height);
		//properly set camera for all scenes
		//This is necessary for proper coordinate conversion when processing events
		BOOST_FOREACH(const SceneSPtr & scene, scenes_)
		{
			scene->resetCamera(scene->screenWidth() / scene->screenHeight());
		}	

		topScene_->resetCamera((float)width / height);
	}

	gl_->setMaskSize(width, height);
}

void Document::setMaskSize(int w, int h) 
{
	gl_->setMaskSize(w, h);
}

void Document::addTempRenderObject(TempRenderObject * tempRenderObject)
{
	tempRenderObject->init(gl_);
	tempRenderObjects_.push_back(tempRenderObject);
}

CustomObjectData * Document::customObjectData(const boost::uuids::uuid & id) const
{
	using namespace boost;
	map<uuids::uuid, boost::shared_ptr<CustomObjectData> >::const_iterator iter;

	iter = customObjectData_.find(id);
	if  (iter == customObjectData_.end()) return NULL;

	return (*iter).second.get();	
}

void Document::setTextData(const std::string & textData)
{
	textData_ = textData;
}

bool Document::setCurPageBookmark()
{
	return template_->setCurPageBookmark();
}
void Document::getBookmarkPages(std::vector<int> * pages)
{
	template_->getBookmarkPages(pages);
}
void Document::changeLandscape(const bool & var)
{
	template_->changeLandscape(var);
}

void Document::read(Reader & reader, unsigned char version)
{
	Global::instance().setCurDocument(this);
	if (version < 4)
	{
		throw Exception("Document version too old");
	}

	if (version >= 14)
	{
		reader.read(textData_);
	}

	if (version >= 8)
	{
		ContentType fileContentType;
		reader.read(fileContentType);
		if (fileContentType != curContentType && 
			curContentType != ContentTypeInternal)
		{
			throw Exception("Wrong content type");
		}
	}
	if (version >= 5)
	{
		reader.read(globalVersion_);
		if (globalVersion_ > Global::version())
		{
			throw Exception("Document version too new for Core");
		}
	}
	if (version >= 2)
	{
		reader.read(width_);
		reader.read(height_);

		origWidth_ = width_;
		origHeight_ = height_;
	}
	
	if (version >= 1)
	{
		reader.read(orientation_);
	}
	else
		orientation_ = OrientationVertical;
		
	reader.read(scenes_);
	BOOST_FOREACH(SceneSPtr scene, scenes_)
		scene->setParentDocument(this);

	reader.read(template_);

	template_->setDocument(this);

	if (version < 2)
	{
		width_ = scenes_.front().get()->screenWidth();
		height_ = scenes_.front().get()->screenHeight();		
	}

	if (version == 6) 
	{
		std::string scriptFile;
		reader.read(scriptFile);
		scriptFiles_.push_back(scriptFile);
	}
	else if (version >= 7)
	{		
		reader.read(scriptFiles_);
	}
	
	if (version >= 9)
	{
		reader.read(scriptAccessibleFiles_);	
	}

	if (version >= 10)
	{
		reader.read(remoteReadServer_);
	}

	if (version == 11)	
	{
		bool unused;
		reader.read(unused);	
	}

	if (version >= 13)
	{
		reader.read(customObjectData_);
	}

	if (version >= 15)
	{
		reader.read(allowMultitouch_);
	}

	if (version >= 16)
	{
		bool temp;
		reader.read(temp);
	}
		
}

void Document::readTextFeatures(const std::string & textFeaturesFileName)
{
	textFeaturesFileName_ = textFeaturesFileName;
	tf_->loadXml(textFeaturesFileName);
}

void Document::write(Writer & writer) const
{
	writer.write(textData_, "textData");
	writer.write(curContentType, "contentType");
	writer.write(globalVersion_, "globalVersion");
	writer.write(width_, "width");
	writer.write(height_, "height");
	writer.write(orientation_, "orientation");
	writer.write(scenes_, "scenes");
	writer.write(template_, "template");

	writer.write(scriptFiles_, "scriptFiles");

	writer.write(scriptAccessibleFiles_, "scriptAccessibleFiles");
	writer.write(remoteReadServer_, "remoteReadServer");

	writer.write(customObjectData_);

	writer.write(allowMultitouch_);	
}

void Document::writeXml(XmlWriter & w) const
{
	using namespace boost;
	string str;

	w.startTag("Document", "Version=\"0\"");	

	w.writeTag("TextData", textData_);
	w.writeTag("Width", lexical_cast<string>(origWidth_));
	w.writeTag("Height", lexical_cast<string>(origHeight_));	
	w.writeTag("Orientation", (unsigned int)orientation_);

	w.startTag("Scenes");
	for (int i = 0; i < (int)scenes_.size(); ++i)
	{
		Scene * scene = scenes_[i].get();
		string idStr = boost::lexical_cast<string>(w.idMapping().getId(scene));
		string attrStr = "Id = \"" + idStr + "\"";
		w.startTag("Scene", attrStr);
		scene->writeXml(w);		
		w.endTag();
	}
	w.endTag(); //scenes

	w.startTag("Template");

	w.startTag(template_->xmlTag());
	template_->writeXml(w);
	w.endTag(); //template

	w.endTag();

	w.startTag("ScriptFiles");
	for (unsigned int i = 0; i < (unsigned int)scriptFiles_.size(); ++i)
		w.writeTag("FileName", scriptFiles_[i]);
	w.endTag();
	w.startTag("ScriptAccessibleFiles");
	std::map<std::string, std::string>::const_iterator it;
	for(it = scriptAccessibleFiles_.begin() ; it != scriptAccessibleFiles_.end() ; it++)
	{
		string attrStr = "Key = \"" + it->first + "\"";
		w.writeTag("FileName", attrStr, it->second);
	}
	w.endTag();

	w.writeTag("RemoteReadServer", remoteReadServer_);
	w.startTag("CustomObjectDataMapping");
	std::map<boost::uuids::uuid, boost::shared_ptr<CustomObjectData> >::const_iterator mit;
	for(mit = customObjectData_.begin() ; mit != customObjectData_.end() ; mit++)
	{
		string str = boost::lexical_cast<std::string>(mit->first);
		string attrStr = "uuid = \"" + str + "\"";
		w.startTag("CustomObjectData", attrStr);
		mit->second->writeXml(w);
		w.endTag();
	}
	w.endTag();
	w.writeTag("AllowMultitouch", allowMultitouch_);


	w.endTag(); //document
}
void Document::readXml(XmlReader & r, xmlNode * parent)
{
	Global::instance().setCurDocument(this);
	string str;
	unsigned int version;
	if (!r.getNodeAttribute(version, parent, "Version")) throw XmlException(parent, "No version");
	r.setVersion(version);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(textData_, curNode, "TextData"));
		else if (r.getNodeContentIfName(origWidth_, curNode, "Width")) width_ = origWidth_;
		else if (r.getNodeContentIfName(origHeight_, curNode, "Height")) height_ = origHeight_;
		else if (r.getNodeContentIfName(remoteReadServer_, curNode, "RemoteReadServer"));
		else if (r.getNodeContentIfName(allowMultitouch_, curNode, "AllowMultitouch"));
		else if (r.getNodeContentIfName(val, curNode, "Orientation"))
		{
			orientation_ = (Orientation)val;
		}
		else if (r.isNodeName(curNode, "Scenes"))
		{
			readXmlScenes(r, curNode);
		}
		else if (r.isNodeName(curNode, "ScriptFiles"))
		{
			string fileName;
			for(xmlNode * fileNode = curNode->children; fileNode; fileNode = fileNode->next)
			{		
				if (fileNode->type != XML_ELEMENT_NODE) continue;				
				if (r.getNodeContentIfName(fileName, fileNode, "FileName"))
					scriptFiles_.push_back(fileName);
			}	
		}
		else if (r.isNodeName(curNode, "ScriptAccessibleFiles"))
		{
			for(xmlNode * fileNode = curNode->children; fileNode; fileNode = fileNode->next)
			{
				string key, value;
				if (fileNode->type != XML_ELEMENT_NODE) continue;
				if (r.getNodeContentIfName(value, curNode, "ActualFileNames"))
				{
					r.getNodeAttribute(key, fileNode, "Key");
					if (key.empty())
						throw XmlException(parent, "No Key");
					scriptAccessibleFiles_[key] = value;
				}
			}			
		}
		else if (r.isNodeName(curNode, "CustomObjectDataMapping"))
		{
			for(xmlNode * fileNode = curNode->children; fileNode; fileNode = fileNode->next)
			{
				string key;
				if (fileNode->type != XML_ELEMENT_NODE) continue;
				if (r.isNodeName(fileNode, "CustomObjectData"))
				{
					r.getNodeAttribute(key, fileNode, "uuid");
					boost::uuids::string_generator gen;
					boost::uuids::uuid uuid = gen(key.c_str());
					boost::shared_ptr<CustomObjectData> data;
					data.reset(new CustomObjectData);
					data->readXml(r, fileNode);
					customObjectData_[uuid] = data;
				}
			}
		}
	}
		
	BOOST_FOREACH(SceneSPtr scene, scenes_)
	{
		scene->remapReferences(*r.idMapping().elemMapping());
		scene->setParentDocument(this);
	}
	

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(curNode, "Template"))
		{
			if (!curNode->children) throw XmlException(curNode, "No children");
			readXmlTemplate(r, curNode);					
		}
	}

}

void Document::readXmlScenes(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * sceneNode = parent->children; sceneNode; sceneNode = sceneNode->next)			
	{
		if (sceneNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(sceneNode, "Scene"))
		{
			SceneSPtr s(new Scene);
			s->readXml(r, sceneNode);
			scenes_.push_back(s);
			r.idMapping().setId(s.get(), scenes_.size());
		}				
	}
}

void Document::readXmlTemplate(XmlReader & r, xmlNode * parent)
{
	bool read = false;
	for (xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getTemplate(template_, curNode))
		{
			read = true;
			break;
		}
	}

	if (!read)
		throw XmlException(parent, "No template");

	template_->setDocument(this);
		
}


void Document::goToPage(const int & index)
{
	template_->goToPage(index);
}

int Document::pageIndex()
{
	return template_->pageIndex();
}

void Document::pause()
{
	pausedTime_ = docTime_;
	isPaused_ = true;
}

void Document::resume()
{
	isPaused_ = false;
	docTime_ = Global::currentTime();
	if (pausedTime_ == 0)
		pausedTime_ = docTime_;
	double time = docTime_ - pausedTime_;
	checkTime_ = time;
	for (int i = 0; i < (int)scenes_.size(); ++i)
	{
		scenes_[i]->setPauseTime(time);
	}
	redrawTriggered_ = true;
}

void Document::pauseTimeReset()
{
	for (int i = 0; i < (int)scenes_.size(); ++i)
	{
		scenes_[i]->setPauseTime(-checkTime_);
	}
}

void Document::photoSetFileName(const std::string & fileName)
{
	BOOST_FOREACH(SceneSPtr child, scenes_)
	{
		const vector<SceneObjectSPtr> & children = child->root()->children();
		BOOST_FOREACH(SceneObjectSPtr child, children)
		{
			//static boost::uuids::uuid photoObjectType = PhotoObject().type();
			if(child->typeStr() == "PhotoObject")
			{
				PhotoObject * photoObject = (PhotoObject *)child.get();
				photoObject->setPhotoName(fileName);
			}
		}	
	}
}

void Document::photoSaveComplete()
{
	Scene * scene = template_->curScene();
	BOOST_FOREACH(SceneSPtr child, scenes_)
	{
		const vector<SceneObjectSPtr> & children = child->root()->children();
		BOOST_FOREACH(SceneObjectSPtr child, children)
		{
			//static boost::uuids::uuid photoObjectType = PhotoObject().type();
			if(child->typeStr() == "PhotoObject")
			{
				PhotoObject * photoObject = (PhotoObject *)child.get();
				photoObject->photoSave();
			}
		}	
	}
}

void Document::photoCaptureComplete()
{
	Scene * scene = template_->curScene();
	BOOST_FOREACH(SceneSPtr child, scenes_)
	{
		const vector<SceneObjectSPtr> & children = child->root()->children();
		BOOST_FOREACH(SceneObjectSPtr child, children)
		{
			//static boost::uuids::uuid photoObjectType = PhotoObject().type();
			if(child->typeStr() == "PhotoObject")
			{
				PhotoObject * photoObject = (PhotoObject *)child.get();
				photoObject->photoTakePicture();
			}
		}	
	}
}

void Document::photoCameraOn(
		const PhotoObject * photoObj,
		const ContainerObjects & containerObjects, 
		float x, float y, float width, float height)
{
	AppObjectOffset offset = template_->totalAppObjectOffset(containerObjects);

	Global::instance().photoCameraOn(offset.sx * x + offset.dx, offset.sy * y + offset.dy, 
		offset.sx * width, offset.sy * height);
}

void Document::photoSave(
		const PhotoObject * photoObj,
		const ContainerObjects & containerObjects, 
		float x, float y, float width, float height)
{
	AppObjectOffset offset = template_->totalAppObjectOffset(containerObjects);

	Global::instance().photoSave(offset.sx * x + offset.dx, offset.sy * y + offset.dy, 
		offset.sx * width, offset.sy * height);
}

void Document::photoDefaultImage()
{
	Scene * scene = template_->curScene();
	BOOST_FOREACH(SceneSPtr child, scenes_)
	{
		const vector<SceneObjectSPtr> & children = child->root()->children();
		BOOST_FOREACH(SceneObjectSPtr child, children)
		{
			//static boost::uuids::uuid photoObjectType = PhotoObject().type();
			if(child->typeStr() == "PhotoObject")
			{
				PhotoObject * photoObject = (PhotoObject *)child.get();
				photoObject->photoDefaultImage();
			}
		}	
	}
}

void Document::textEditUpdate()
{
	Scene * scene = template_->curScene();
	scene->broadcastMessage("textEditUpdate");
}

void Document::totalScenes()
{
	totalScenes_.clear();
	for (int i = 0; i < scenes_.size(); i++)
	{
		totalScenes_.push_back(scenes_[i]);
	}
}

Scene * Document::curScene() const 
{
	SingleSceneDocumentTemplate* templ = 
			(SingleSceneDocumentTemplate *)documentTemplate().get();

	if (templ)
	{
		return templ->curScene();	
	}
}
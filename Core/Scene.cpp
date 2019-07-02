#include "stdafx.h"
#include <boost/lexical_cast.hpp>
#include "Scene.h"
#include "SubScene.h"
#include "Root.h"
#include "GfxRenderer.h"
#include "SceneChanger.h"
#include "SceneStartEvent.h"
#include "ScenePreStartEvent.h"
#include "SceneStopEvent.h"
#include "Animation.h"
#include "Camera.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "MathStuff.h"
#include "Global.h"
#include "AppObject.h"
#include "VideoPlayer.h"
#include "Document.h"
#include "ScriptProcessor.h"
#include "DocumentTemplate.h"
#include "CameraObject.h"
#include "Text.h"
#include "LightObject.h"
#include "ModelFile.h"
#include "CustomObject.h"
#include "MaskObject.h"

#include "KeyPressEvent.h"
#include "KeyReleaseEvent.h"

#include "ElementMapping.h"

#include "EventListener.h"
#include "Action.h"
#include "WaitUntilAction.h"

#ifdef WATERMARK
//nentv watermark
#include "Texture.h"
#include "netntv_img.h"
#endif

using namespace std;


////////////////////////////////////////////////////////////////////////////////
JSClass jsSceneClass = InitClass(
	"Scene", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, 0, Scene::gcMark);

JSBool Scene_setZBuffer(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);
	
	JSBool val;
	if (!JS_ConvertArguments(cx, argc, argv, "b", &val)) return JS_FALSE;

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	scene->setZBuffer((bool)val);
	return JS_TRUE;
}

JSBool Scene_createSceneObject(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);
	
	const jschar * jsStr;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &jsStr)) return JS_FALSE;
	std::string typeStr = jsCharStrToStdString(jsStr);

	SceneObject * newObj = Global::instance().createSceneObject(typeStr);

	if (newObj) 
	{
		scene->root()->addChild(SceneObjectSPtr(newObj));
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(newObj->getScriptObject(s)));
	}
	else
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	
	return JS_TRUE;
}

JSBool Scene_createAnimation(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);
		
	Animation * animation = new Animation;
	scene->addAnimation(AnimationSPtr(animation));
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(animation->getScriptObject(s)));
		
	return JS_TRUE;
}

JSBool Scene_getSceneObjectById(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	const jschar * id;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &id)) return JS_FALSE;

	string idStr = jsCharStrToStdString(id);
	Scene * scene = 0;
	SceneObject * sceneObj = 0;
	scene = (Scene *)JS_GetPrivate(cx, obj);
	if (scene)
	{
		sceneObj = scene->findObject(idStr).get();
	}

	if (!sceneObj)
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	else
	{
		//create a JSObject based on the scene object
		JSObject * jsObject = sceneObj->getScriptObject(s);
	
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsObject));
	}

	return JS_TRUE;
}

JSBool Scene_getAppObjectById(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	const jschar * id;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &id)) return JS_FALSE;

	string idStr = jsCharStrToStdString(id);
	Scene * scene = 0;
	AppObject * appObj = 0;
	scene = (Scene *)JS_GetPrivate(cx, obj);
	if (scene)
	{
		appObj = scene->findAppObject(idStr).get();
	}

	if (!appObj)
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	else
	{
		//create a JSObject based on the scene object
		JSObject * jsObject = appObj->getScriptObject(s);
	
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsObject));
	}

	return JS_TRUE;
}

JSBool Scene_getRoot(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);	

	JSObject * jsObject = scene->root()->getScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsObject));

	return JS_TRUE;
}

JSBool Scene_getWidth(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);	
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(scene->screenWidth()));
	return JS_TRUE;
}

JSBool Scene_getHeight(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);		
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(scene->screenHeight()));
	return JS_TRUE;
}

JSBool Scene_setWidth(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	uint32 val;
	if (!JS_ConvertArguments(cx, argc, argv, "u", &val)) return JS_FALSE;

	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);	
	scene->setScreenWidth((float)val);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Scene_setHeight(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	uint32 val;
	if (!JS_ConvertArguments(cx, argc, argv, "u", &val)) return JS_FALSE;

	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);	
	scene->setScreenHeight((float)val);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Scene_getAnimationById(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	const jschar * id;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &id)) return JS_FALSE;

	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);
	std::string animId = jsCharStrToStdString(id);
	Animation * animObj = scene->findAnimation(animId.c_str()).get();

	if (!animObj)
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	else
	{
		//create a JSObject based on the scene object
		JSObject * jsObject = animObj->getScriptObject(s);	
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsObject));
	}

	return JS_TRUE;
}

JSBool Scene_addEventListener(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	jschar * type;
	JSObject * listener;
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	if (!JS_ConvertArguments(cx, argc, argv, "Wo", &type, &listener))
        return JS_FALSE;

	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);
	SceneObject * sceneObj = scene->root();
	string typeStr = jsCharStrToStdString(type);

	Event * event = Global::instance().event(sceneObj, typeStr);
	if (!event) 
	{
		JS_ReportError(cx, "Unrecognized event type");
		return JS_FALSE;
	}

	if (!JS_ObjectIsFunction(cx, listener))
	{
		JS_ReportError(cx, "Event listener must be a function");
		return JS_FALSE;
	}
	
	bool added = 
		sceneObj->addScriptEventListener(ScriptEventListener(event->type(), listener));
	
	return JS_TRUE;
}


JSBool Scene_removeEventListener(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	jschar * type;
	JSObject * listener;
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	if (!JS_ConvertArguments(cx, argc, argv, "Wo", &type, &listener))
        return JS_FALSE;

	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);
	SceneObject * sceneObj = scene->root();
	string typeStr = jsCharStrToStdString(type);

	Event * event = Global::instance().event(sceneObj, typeStr);
	if (!event) 
	{
		JS_ReportError(cx, "Unrecognized event type");
		return JS_FALSE;	
	}
	
	bool removed = 
		sceneObj->removeScriptEventListener(ScriptEventListener(event->type(), listener));

	if (removed) s->unrootJSObject(listener);
	
	return JS_TRUE;
};


JSBool Scene_appendChild(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Scene * thisObj = (Scene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject *jsNewChild;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsNewChild)) 
		return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (!isSceneObject(s, jsNewChild))
	{
		JS_ReportError(cx, "Scene::appendChild - parameter must be a SceneObject");
		return JS_FALSE;
	}

	SceneObject * newChild;
	newChild = (SceneObject *)JS_GetPrivate(cx, jsNewChild);

	Scene * parentScene = newChild->parentScene();
	SceneObjectSPtr newChildSPtr;
	if (parentScene)
		newChildSPtr = 
			parentScene->findObject(newChild->parent(), newChild);
	else
		newChildSPtr = 
			s->removeKeepAliveData(newChild);

	if (!newChildSPtr)
	{
		JS_ReportError(cx, "Scene::apendChild - internal error");
		return JS_FALSE;
	}
	thisObj->root()->addChild(newChildSPtr);

	return JS_TRUE;
};

JSBool Scene_addChild(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Scene * thisObj = (Scene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject *jsNewChild;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsNewChild)) 
		return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (!isSceneObject(s, jsNewChild))
	{
		JS_ReportError(cx, "Scene::appdChild - parameter must be a SceneObject");
		return JS_FALSE;
	}

	SceneObject * newChild;
	newChild = (SceneObject *)JS_GetPrivate(cx, jsNewChild);

	Scene * parentScene = newChild->parentScene();
	SceneObjectSPtr newChildSPtr;
	
	if (parentScene)
		newChildSPtr = 
			parentScene->findObject(newChild->parent(), newChild);
	else
		newChildSPtr = 
			s->removeKeepAliveData(newChild);

	if (!newChildSPtr)
	{
		JS_ReportError(cx, "Scene::apendChild - internal error");
		return JS_FALSE;
	}
	thisObj->root()->addChild(newChildSPtr);

	return JS_TRUE;
};

JSBool Scene_removeChild(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Scene * thisObj = (Scene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject *jsChild;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsChild)) 
		return JS_FALSE;	

	if (!isSceneObject(s, jsChild))
	{
		JS_ReportError(cx, "Group::removeChild - parameter must be a SceneObject");
		return JS_FALSE;
	}

	SceneObject * child;
	child = (SceneObject *)JS_GetPrivate(cx, jsChild);
	SceneObjectSPtr childSPtr = thisObj->root()->removeChild(child);

	if (childSPtr)
	{
		s->addKeepAliveData(childSPtr);
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(childSPtr->getScriptObject(s)));
	}
	else JS_SET_RVAL(cx, vp, JSVAL_NULL);
	
	return JS_TRUE;
}

JSBool Scene_insertAfter(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Scene * thisObj = (Scene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsRefChild, *jsNewChild;
	if (!JS_ConvertArguments(cx, argc, argv, "oo", &jsNewChild, &jsRefChild)) 
		return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (!isSceneObject(s, jsRefChild))
	{
		JS_ReportError(cx, "Scene::insertAfter - parameters must be SceneObjects");
		return JS_FALSE;
	}

	if (!isSceneObject(s, jsNewChild))
	{
		JS_ReportError(cx, "Scene::insertAfter - parameters must be SceneObjects");
		return JS_FALSE;
	}

	SceneObject * refChild, * newChild;
	refChild = (SceneObject *)JS_GetPrivate(cx, jsRefChild);
	newChild = (SceneObject *)JS_GetPrivate(cx, jsNewChild);

	SceneObjectSPtr newChildSPtr = 
		newChild->parentScene()->findObject(newChild->parent(), newChild);
	thisObj->root()->insertAfter(newChildSPtr, refChild);

	return JS_TRUE;
};

JSBool Scene_getChildren(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Scene * thisObj = (Scene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	const vector<SceneObjectSPtr> & children = thisObj->root()->children();

	JSObject * jsChildren = JS_NewArrayObject(cx, (jsint)children.size(), 0);

	for (int i = 0; i < (int)children.size(); ++i)
	{
		jsval val;
		val = OBJECT_TO_JSVAL(children[i]->getScriptObject(s));
		JS_SetElement(cx, jsChildren, (jsint)i, &val);
	}

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsChildren));	

	return JS_TRUE;
};

JSBool Scene_getAnimations(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Scene * thisObj = (Scene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	const vector<AnimationSPtr> & animations = thisObj->animations();

	JSObject * jsAnimations = JS_NewArrayObject(cx, (jsint)animations.size(), 0);

	for (int i = 0; i < (int)animations.size(); ++i)
	{
		jsval val;
		val = OBJECT_TO_JSVAL(animations[i]->getScriptObject(s));
		JS_SetElement(cx, jsAnimations, (jsint)i, &val);
	}

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsAnimations));	

	return JS_TRUE;
};

JSBool Scene_resumeAnimation(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Scene * thisObj = (Scene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsAnim;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsAnim)) 
		return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (JS_GetPrototype(cx, jsAnim) != s->animationProto()) 
	{
		JS_ReportError(cx, "Scene::resumeAnimation - parameter must be an Animation");
		return JS_FALSE;
	}

	Animation * anim = (Animation *)JS_GetPrivate(cx, jsAnim);

	thisObj->resume(anim);

	return JS_TRUE;
};

JSBool Scene_playAnimation(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Scene * thisObj = (Scene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsAnim;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsAnim)) 
		return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (JS_GetPrototype(cx, jsAnim) != s->animationProto()) 
	{
		JS_ReportError(cx, "Scene::playAnimation - parameter must be an Animation");
		return JS_FALSE;
	}
		
	Animation * anim = (Animation *)JS_GetPrivate(cx, jsAnim);

	thisObj->play(anim);

	return JS_TRUE;
};

JSBool Scene_resetAnimation(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Scene * thisObj = (Scene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsAnim;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsAnim)) 
		return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (JS_GetPrototype(cx, jsAnim) != s->animationProto()) 
	{
		JS_ReportError(cx, "Scene::resetAnimation - parameter must be an Animation");
		return JS_FALSE;
	}
		
	Animation * anim = (Animation *)JS_GetPrivate(cx, jsAnim);

	thisObj->stop(anim);
	anim->update(0.0f);

	return JS_TRUE;
};

JSBool Scene_stopAnimation(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Scene * thisObj = (Scene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsAnim;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsAnim)) 
		return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (JS_GetPrototype(cx, jsAnim) != s->animationProto()) 
	{
		JS_ReportError(cx, "Scene::resetAnimation - parameter must be an Animation");
		return JS_FALSE;
	}
		
	Animation * anim = (Animation *)JS_GetPrivate(cx, jsAnim);

	thisObj->stop(anim);

	return JS_TRUE;
};

JSBool Scene_isInit(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);		
	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL((JSBool)scene->isInit()));
	return JS_TRUE;
}

JSBool Scene_init(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);
	scene->init(scene->parentDocument()->renderer());
	JS_SET_RVAL(cx, vp, JSVAL_VOID);	
	return JS_TRUE;
}

JSBool Scene_uninit(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);
	scene->uninit();
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Scene_isCurrentScene(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);
	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL((JSBool)scene->isCurrentScene()));
	return JS_TRUE;
}

JSBool Scene_createEvent(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	SceneObject * thisObj = ((Scene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp)))->root();
	const jschar * jsType;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &jsType)) return JS_FALSE;
	string type = jsCharStrToStdString(jsType);
	Event * event = Global::instance().createEvent(thisObj, type);
	if (!event)
	{
		JS_ReportError(cx, "Scene::createEvent - Invalid type.");
		return JS_FALSE;
	}
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsEvt = event->createScriptObject(s, true);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsEvt));
	return JS_TRUE;
}

JSBool Scene_dispatchEvent(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * jsEvt;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsEvt)) return JS_FALSE;
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!isEvent(s, jsEvt))
	{
		JS_ReportError(cx, "Scene::dispatchEvent - Parameter must be an Event.");
		return JS_FALSE;
	}

	SceneObject * thisObj = ((Scene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp)))->root();
	Event * event = (Event *)JS_GetPrivate(cx, jsEvt);
	thisObj->handleEvent(event, Global::currentTime());
	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Scene_stopActions(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);
	scene->stopEventListeners();
	return JS_TRUE;
}

JSBool Scene_getSceneId(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Scene * thisObj = (Scene *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "")) return JS_FALSE;
	
	const char * idStr = thisObj->name().c_str();
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, idStr)));
	return JS_TRUE;
}

JSBool Scene_getRootScene(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Scene * scene = (Scene *)JS_GetPrivate(cx, obj);
	Scene * scene_ = scene->rootScene();
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!scene_)
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	else
	{
		//create a JSObject based on the scene object
		JSObject * jsObject = scene_->getScriptObject(s);
		jsval objJsVal = OBJECT_TO_JSVAL(jsObject);
		JS_SET_RVAL(cx, vp, objJsVal);


		jsval_layout theValue = *(jsval_layout*)&objJsVal;

		JSObject * obj = JSVAL_TO_OBJECT(objJsVal);
	}
	return JS_TRUE;
}

JSFunctionSpec SceneFuncs[] = {	
	JS_FS("setZBuffer",   Scene_setZBuffer, 1, 0),	
	
	JS_FS("createAnimation", Scene_createAnimation, 0, 0),	
	JS_FS("getAnimationById", Scene_getAnimationById, 1, 0),

	JS_FS("createSceneObject", Scene_createSceneObject, 1, 0),
	JS_FS("getSceneObjectById",   Scene_getSceneObjectById, 1, 0),	
	JS_FS("getAppObjectById", Scene_getAppObjectById, 1, 0),
	JS_FS("getRoot",   Scene_getRoot, 0, 0),

	JS_FS("getWidth",   Scene_getWidth, 0, 0),
	JS_FS("setWidth",	Scene_setWidth, 1, 0),
	JS_FS("getHeight",   Scene_getHeight, 0, 0),
	JS_FS("setHeight",	Scene_setHeight, 1, 0),

	JS_FS("appendChild",	Scene_appendChild, 1, 0),
	JS_FS("addChild",	Scene_addChild, 1, 0),
	JS_FS("insertAfter",   Scene_insertAfter, 2, 0),
	JS_FS("removeChild",	Scene_removeChild, 1, 0),
	JS_FS("getChildren",	Scene_getChildren, 0, 0),

	JS_FS("addEventListener",   Scene_addEventListener, 2, 0),
	JS_FS("removeEventListener",   Scene_removeEventListener, 2, 0),

	JS_FS("getAnimations",	Scene_getAnimations, 0, 0),
	JS_FS("playAnimation", Scene_playAnimation, 1, 0),
	JS_FS("resumeAnimation", Scene_resumeAnimation, 1, 0),
	JS_FS("resetAnimation", Scene_resetAnimation, 1, 0),
	JS_FS("stopAnimation", Scene_stopAnimation, 1, 0),


	JS_FS("isInit", Scene_isInit, 0, 0),
	JS_FS("init", Scene_init, 0, 0),
	JS_FS("uninit", Scene_uninit, 0, 0),

	JS_FS("isCurrentScene", Scene_isCurrentScene, 0, 0),

	JS_FS("createEvent", Scene_createEvent, 1, 0),
	JS_FS("dispatchEvent", Scene_dispatchEvent, 1, 0),

	JS_FS("stopActions", Scene_stopActions, 0, 0),

	JS_FS("getSceneId", Scene_getSceneId, 0 ,0),
	JS_FS("getRootScene", Scene_getRootScene, 0, 0),
    JS_FS_END
};


////////////////////////////////////////////////////////////////////////////////
JSObject * Scene::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), 0, &jsSceneClass,
		0, 0, 0, SceneFuncs, 0, 0);
	s->rootJSObject(proto);
	return proto;
}

JSObject * Scene::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsSceneClass, s->sceneProto(), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);
	}

	return scriptObject_;
}

void Scene::gcMark(JSTracer * trc, JSObject * obj)
{
	JSContext * cx = trc->context;
	Scene * s = (Scene *)JS_GetPrivate(cx, obj);
	if (s)
	{
		SceneObject * o = s->root();
		vector<ScriptEventListener>::iterator iter;		
		for (iter = o->scriptEventListeners_.begin(); iter != o->scriptEventListeners_.end(); ++iter)
		{				
			JS_TRACER_SET_DEBUG_VARS(trc)
			JS_CallTracer(trc, (*iter).functionScriptObject(), JSTRACE_OBJECT);			
		}

		
		BOOST_FOREACH(SceneObjectSPtr & child, o->children_)
		{			
			if (child->scriptObject_)
			{
				JS_TRACER_SET_DEBUG_VARS(trc)

				JS_CallTracer(trc, child->scriptObject_, JSTRACE_OBJECT);	
			}
		}

		BOOST_FOREACH(AnimationSPtr & child, s->animations_)
		{
			if (child->scriptObject_)
			{
				JS_TRACER_SET_DEBUG_VARS(trc)
				JS_CallTracer(trc, child->scriptObject_, JSTRACE_OBJECT);	
			}
		}
		
	}
}

void Scene::create()
{
	lightIntensity_ = 0.75f;
	lightSpecularIntensity_ = 0.0f;
	lightAmbient_ = 0.1f;

	keepInit_ = false;
	root_.reset(new Root);
	root_->parentScene_ = this;
	name_ = "Scene0";
	
	screenWidth_ = 768;
	screenHeight_ = 1024;
	zBuffer_ = false;
	zoom_ = false;
	zoomMagState_ = 0;

	bgColor_ = Color(0.5, 0.5, 0.5, 1);
	
	camera_.reset(new Camera);
	camera_->SetTo2DArea(
		screenWidth_/2, screenHeight_/2,
		screenWidth_, screenHeight_, 60, 1);
	init_ = false;

	curContainerObject_ = NULL;

	parentDocument_ = 0;
	container_ = 0;

	scriptObject_ = 0;

	cameraObject_ = NULL;
	objectsListChanged_ = true;

	startCount_ = 0;
    
    currentScale_ = Vector3(1,1,1);

#ifdef WATERMARK
	netntvTexture_ = new Texture;
	waterMark_ = false;
#endif
}

Scene::Scene()
{	
	create();
}


Scene::Scene(int width, int height)
{
	create();
	screenWidth_ = width;
	screenHeight_ = height;
	resetCamera(screenWidth_ / screenHeight_);
	if (cameraObject_)
		cameraObject_->setAspectRatio(screenWidth_ / screenHeight_);
}

Scene::Scene(const Scene & rhs, ElementMapping * mapping)
{
	if (mapping)
		mapping->addSceneMapping(const_cast<Scene *>(&rhs), this);
	create();
	name_ = rhs.name_;
	animations_.reserve(rhs.animations_.size());
	BOOST_FOREACH(const AnimationSPtr & anim, rhs.animations_)
	{
		animations_.push_back(AnimationSPtr(
			new Animation(*anim, mapping)));
	}

	root_.reset(rhs.root_->clone(mapping));
	camera_.reset(new Camera(*rhs.camera_));

	screenWidth_ = rhs.screenWidth_;
	screenHeight_ = rhs.screenHeight_;
	zBuffer_ = rhs.zBuffer_;
	bgColor_ = rhs.bgColor_;

	appObjects_ = rhs.appObjects_;
}

void Scene::remapReferences(const ElementMapping & mapping)
{
	root_->remapReferences(mapping);
	BOOST_FOREACH(AnimationSPtr anim, animations_)
	{
		anim->remapReferences(mapping);
	}

	parentDocument_ = mapping.destinationDocument();

	cameraObject_ = (CameraObject *)mapping.mapObject(cameraObject_);

	map<Animation *, SceneObject *> oldSA = syncedAnimations_;
	map<Animation *, SceneObject *>::iterator itr;

	syncedAnimations_.clear();
	for (itr = oldSA.begin(); itr != oldSA.end(); ++itr)
	{
		Animation * anim = (*itr).first;
		SceneObject * obj = (*itr).second;

		syncedAnimations_[mapping.mapAnimation(anim)] = mapping.mapObject(obj);
	}

	updateObjsWithSyncedAnimations();

}


void Scene::referencedFiles(vector<string> * refFiles) const
{
	root_->referencedFiles(refFiles);
	BOOST_FOREACH(AppObjectSPtr appObject, appObjects_)
	{
		appObject->referencedFiles(refFiles);
	}
}

int Scene::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = root_->setReferencedFiles(baseDirectory, refFiles, index);
	BOOST_FOREACH(AppObjectSPtr appObject, appObjects_)
	{
		index = appObject->setReferencedFiles(baseDirectory, refFiles, index);
	}
	return index;
}

Scene::~Scene()
{
	uninit();
	
	//operate on a copy, because the original will be continually deleted from
	std::set<SceneReferencer *> objRefsCopy = objRefs_;
	BOOST_FOREACH(SceneReferencer * obj, objRefsCopy) obj->setScene(0);	

#ifdef WATERMARK
	delete netntvTexture_;
#endif
}

void Scene::init(GfxRenderer * gl, bool firstTime)
{		
	uninit();
	init_ = true;
	root_->init(gl, firstTime);	
	flagObjectsListChanged();
	
#ifdef WATERMARK
	netntvTexture_->init(
		gl, netntvImgWidth, netntvImgHeight, Texture::UncompressedRgba32, netntvImgBits);
#endif
}

void Scene::uninit()
{
	if (!init_) return;	
	init_ = false;
	root_->uninit();
	vector<LightObject *>().swap(lights_);
	vector<ModelFile *>().swap(models_);
#ifdef WATERMARK
	netntvTexture_->uninit();
#endif
}

void Scene::updateLightsList()
{
	vector<SceneObject *> sceneObjs;

	sceneObjs.clear();
	getSceneObjectsByID(LightObject().type(), &sceneObjs);
	lights_.clear();
	for (int i = 0; i < (int)sceneObjs.size(); ++i)
		lights_.push_back((LightObject *)sceneObjs[i]);
}

void Scene::updateModelsList()
{
	models_.clear();
	updateModelsList(root_.get());
}

void Scene::updateModelsList(SceneObject * sceneObject)
{
	if (ModelFile().type() == sceneObject->type())
		models_.push_back((ModelFile *)sceneObject);

	BOOST_FOREACH(const SceneObjectSPtr & obj, sceneObject->children())
		updateModelsList(obj.get());
}

void Scene::initSceneLighting(GfxRenderer * gl, const Camera * curViewingCamera) const
{
	Vector3 camDir;
	if (curViewingCamera)
	{
		camDir = (curViewingCamera->lookAt() - curViewingCamera->eye()).normalize();
	}
	else
	{
		const Camera * sceneCam = camera();
		camDir = (sceneCam->lookAt() - sceneCam->eye()).normalize();
	}

	if (!lights_.empty())
		gl->initMeshProgramLights(camDir, lightAmbient_, lights_);
	else
	{		
		static vector<LightObject *> camLight;
		static LightObject camLightObject;

		camLightObject.setLightDirection(camDir);
		camLightObject.setIntensity(lightIntensity_);
		camLightObject.setSpecularIntensity(lightSpecularIntensity_);
		camLight.clear();
		camLight.push_back(&camLightObject);

		gl->initMeshProgramLights(camDir, lightAmbient_, camLight);		
	}
}

void Scene::draw(GfxRenderer * gl) const
{
	//render objects
	
	GLint prevDepthFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);
	if (zBuffer_)
	{
		gl->clearZBuffer();
		glDepthFunc(GL_LEQUAL);
	}
	else glDepthFunc(GL_ALWAYS);

	initSceneLighting(gl, NULL);

	root_->draw(gl);	

	BOOST_FOREACH(ModelFile * model, models_)
		model->drawAlpha(gl);

#ifdef WATERMARK

	if (waterMark_)
	{
		gl->useTextureProgram();
		gl->setTextureProgramOpacity(1);
		gl->use(netntvTexture_);
		gl->drawRect(screenWidth_-185,screenHeight_-81, netntvImgWidth, netntvImgHeight);		
	}
#endif

	glDepthFunc(prevDepthFunc);
}

bool Scene::waterMark() 
{
#ifdef WATERMARK
	waterMark_ = true;;
	return waterMark_;
#endif
	return false;
}

int Scene::numAppObjects() const
{
	return (int)appObjects_.size();
}

AppObjectSPtr Scene::deleteAppObject(AppObject * appObject)
{
	vector<AppObjectSPtr>::iterator iter;
	for (iter = appObjects_.begin(); iter != appObjects_.end(); ++iter)
	{
		AppObjectSPtr appObjSptr = *iter;
		if ((*iter).get() == appObject)
		{			
			appObjects_.erase(iter);
			return appObjSptr;
		}
	}

	return AppObjectSPtr();
}

void Scene::addElement(const SceneObjectSPtr & object)
{
	root_->addChild(object);
	setupNewObject(object.get());
	flagObjectsListChanged();
}

void Scene::flagObjectsListChanged()
{
	objectsListChanged_ = true;
}

void Scene::handleObjectsListChanged()
{
	if (objectsListChanged_)
	{
		updateModelsList();
		updateLightsList();
		objectsListChanged_ = false;
	}
}

void Scene::insertBefore(const AppObjectSPtr & newObj, AppObject * refObj)
{
	vector<AppObjectSPtr>::iterator insertIter, iter;
	if (newObj.get() == refObj) return;
	
	insertIter = appObjects_.end();	
	for (iter = appObjects_.begin(); iter != appObjects_.end(); ++iter)	
		if ((*iter).get() == refObj) insertIter = iter;

	if (insertIter == appObjects_.end() && refObj) throw Exception(InvalidArguments,
		"refObj not found");

	appObjects_.insert(insertIter, newObj);
	//newObj->setParentScene(this);
}

void Scene::setupNewObject(SceneObject * obj)
{
	obj->parentScene_ = this;

	BOOST_FOREACH(SceneObjectSPtr child, obj->children())
	{
		setupNewObject(child.get());
	}
}



void Scene::start(float docTime, bool triggerEvent)
{
	if (startCount_ > 0) return;
	//Commenting out clear(): Fix for problem where calling Document init
	//twice would stop playing animations
	//playingAnimations_.clear();

	vector<EventListenerSPtr> listeners;
	allListeners(root_.get(), &listeners);
	BOOST_FOREACH(const EventListenerSPtr & el, listeners)
	{
		std::vector<ActionSPtr> actions = el->actions();
		BOOST_FOREACH(const ActionSPtr & action, actions)
		{
			static boost::uuids::uuid waituntiltype = WaitUntilAction().type();
			if (action->type() == waituntiltype)
			{
				WaitUntilAction * waituntil = (WaitUntilAction*)action.get();
				waituntil->setDelayedTime(0);
			}
		}
	}

	if (triggerEvent)
	{
		SceneStartEvent event;
		event.target = root_.get();
		handleEvent(&event);
	}

	root_->start(docTime);
	update(docTime);
	++startCount_;
}

void Scene::preStart(float docTime)
{
	ScenePreStartEvent event;
	event.target = root_.get();
	handleEvent(&event);

	root_->preStart(docTime);
	update(docTime);
}

bool Scene::isCurrentScene() const
{
	return parentDocument_->isCurrentScene((Scene *)this);
}

void Scene::stopEventListeners()
{
	root_->stopEventListeners();
}

void Scene::stop(bool triggerEvent)
{
	if (startCount_ <= 0) return;
	if (triggerEvent)
	{
		SceneStopEvent event;
		event.target = root_.get();
		handleEvent(&event);
	}

	playingAnimations_.clear();
	root_->stop();
	--startCount_;
	parentDocument_->pauseTimeReset();
}



bool Scene::update(float time)
{
	handleObjectsListChanged();
	bool needToRedraw = root_->update(time);

	for (int i = 0; i < (int)playingAnimations_.size(); ++i)
	{

		float animTime = time - playingAnimations_[i].startTime;
		needToRedraw |= 
			playingAnimations_[i].animation->update(animTime);
		bool isRepeat = playingAnimations_[i].animation->repeat();
		bool isDurOver = 
			animTime > playingAnimations_[i].animation->duration();
		if (!isRepeat && isDurOver)
		{
			stop(playingAnimations_[i].animation);
			--i;
		}
	}

	
	return needToRedraw;
}

bool Scene::asyncLoadUpdate()
{
	return root_->asyncLoadUpdate();
}

bool Scene::isLoaded() const
{
	return root_->isLoaded();
}

bool Scene::handleEvent(Event * event)
{
	return handleEvent(root_.get(), event);
}

bool Scene::handleEvent(SceneObject * obj, Event * event)
{	
	bool handled = false;
	BOOST_FOREACH(const SceneObjectSPtr & child, obj->children())
	{
		handled |= handleEvent(child.get(), event);
	}

	handled |= obj->handleEvent(event, Global::currentTime());

	return handled;
}

SceneObjectSPtr Scene::findObject(SceneObject * obj) const
{
	return findObject(root_.get(), obj);
}

AppObjectSPtr Scene::findAppObject(AppObject * appObj) const
{
	BOOST_FOREACH(AppObjectSPtr appObjSPtr, appObjects_)
	{
		if (appObjSPtr.get() == appObj) return appObjSPtr;
	}
	return AppObjectSPtr();
}

SceneObjectSPtr Scene::findObject(
	const SceneObject * parent, SceneObject * obj) const
{
	BOOST_FOREACH(const SceneObjectSPtr & child, parent->children())
	{
		if (child.get() == obj) return child;

		SceneObjectSPtr found = findObject(child.get(), obj);
		if (found) return found;
	}

	return SceneObjectSPtr();
}

SceneObjectSPtr Scene::findObject(const std::string & id) const
{
	return findObject(root_.get(), id);
}

SceneObjectSPtr Scene::findObject(
	const SceneObject * parent, const std::string & id) const
{
	BOOST_FOREACH(const SceneObjectSPtr & child, parent->children())
	{
		if (child->id() == id) return child;

		SceneObjectSPtr found = findObject(child.get(), id);
		if (found) return found;
	}

	return SceneObjectSPtr();
}

AppObjectSPtr Scene::findAppObject(const std::string & id) const
{
	BOOST_FOREACH(const AppObjectSPtr & child, appObjects_)
	{
		if (child->id() == id) return child;
	}

	return AppObjectSPtr();
}

void Scene::addAnimation(const AnimationSPtr & animation)
{
	animations_.push_back(animation);
	animation->setParentScene(this);
}

void Scene::deleteAnimation(Animation * animation)
{
	vector<AnimationSPtr>::iterator iter = animations_.begin();
	for (;iter != animations_.end(); ++iter)
	{
		if ((*iter).get() == animation) 
		{
			animations_.erase(iter);
			return;
		}
	}
}


void Scene::setAnimations(const std::vector<AnimationSPtr> & animations)
{
	animations_ = animations;
}

void Scene::insertBefore(const AnimationSPtr & newObj, Animation * refObj)
{
	vector<AnimationSPtr>::iterator insertIter, removeIter, iter;
	if (newObj.get() == refObj) return;

	removeIter = animations_.end();
	insertIter = animations_.end();	
	for (iter = animations_.begin(); iter != animations_.end(); ++iter)	
	{
		if (*iter == newObj) removeIter = iter;	
		if ((*iter).get() == refObj) insertIter = iter;
	}

	if (removeIter == animations_.end()) throw Exception(InvalidArguments);
	if (insertIter == animations_.end() && refObj) throw Exception(InvalidArguments);

	animations_.erase(removeIter);

	insertIter = animations_.end();	
	for (iter = animations_.begin(); iter != animations_.end(); ++iter)	
		if ((*iter).get() == refObj) insertIter = iter;	

	animations_.insert(insertIter, newObj);
}


int Scene::animationIndex(Animation * animation) const
{
	for (int i = 0; i < (int)animations_.size(); ++i)
	{
		if (animations_[i].get() == animation) return i;
	}
	return -1;
}

AnimationSPtr Scene::findAnimation(Animation * animation) const
{
	BOOST_FOREACH(const AnimationSPtr & anim, animations_)
	{
		if (anim.get() == animation) return anim;
	}
	return AnimationSPtr();
}

AnimationSPtr Scene::findAnimation(const char * animName) const
{
	BOOST_FOREACH(const AnimationSPtr & anim, animations_)
	{
		if (anim->name() == animName) return anim;
	}
	return AnimationSPtr();
}

int Scene::numTopLevelObjects() const 
{
	return root_->children().size();
}

Vector2 Scene::screenToDeviceCoords(const Vector2 & pos) const
{
	return Vector2(2*pos.x/screenWidth_ - 1, 2*(1 - pos.y/screenHeight_) - 1);
}

bool Scene::clickEvent(const Vector2 & mousePos, int pressId)
{
	bool handled = false;
	Ray mouseRay = 
		(cameraObject_)?
		cameraObject_->unproject(screenToDeviceCoords(mousePos)):
		camera_->unproject(screenToDeviceCoords(mousePos));

	//BOOST_FOREACH(const AppObjectSPtr & appobj, appObjects_)
	//{
	AppObject * appobj = intersectAppObject(mouseRay);
	if (appobj)
		handled |= appobj->clickEvent(mousePos, pressId);
//	}

	if (!handled)
	{		
		Vector3 intPt;
		SceneObject * obj = intersect(mouseRay, &intPt);
		while(obj)
		{
			handled |= obj->clickEvent(mousePos, pressId);
			obj = obj->parent();
		}
	}

	return handled;
}

bool Scene::doubleClickEvent(const Vector2 & mousePos, int pressId)
{
	bool handled = false;
	Ray mouseRay = 
		(cameraObject_)?
		cameraObject_->unproject(screenToDeviceCoords(mousePos)):
	camera_->unproject(screenToDeviceCoords(mousePos));

	AppObject * appobj = intersectAppObject(mouseRay);
	if (appobj)
		handled = true;
	if (!handled)
	{
		Vector3 intPt;
		SceneObject * obj = intersect(mouseRay, &intPt);
		while(obj)
		{
			handled |= obj->doubleClickEvent(mousePos, pressId);
			obj = obj->parent();
		}
	}

	return handled;
}

bool Scene::pressEvent(const Vector2 & startPos, int pressId)
{
	bool handled = false;
	Ray mouseRay = 
		(cameraObject_)?
		cameraObject_->unproject(screenToDeviceCoords(startPos)):
		camera_->unproject(screenToDeviceCoords(startPos));

	AppObject * appobj = intersectAppObject(mouseRay);
	if (!appobj)
	{
		Vector3 intPt;
		SceneObject * obj = intersect(mouseRay, &intPt);
		bool intersected = obj != root_.get();

		vector<SceneObject *> objs;
		if (obj->typeStr() != "TextEditObject")
		{
			Global::instance().focusOutTextEditObj();
		}
		while(obj)
		{
			objs.push_back(obj);
			handled |= obj->pressEvent(startPos, pressId);
			obj = obj->parent();
		}


		pressEventMap_[pressId] = PressEventData(startPos, objs, intersected);
	}
	return handled;
}

bool Scene::wasPressIntersected(int pressId) const
{
	map<int, PressEventData>::const_iterator iter = 
		pressEventMap_.find(pressId);
	if (iter == pressEventMap_.end()) return false;
	return (*iter).second.intersected;
}

bool Scene::moveEvent(const Vector2 & curPos, int pressId)
{
	bool handled = false;
	BOOST_FOREACH(SceneObject * obj, pressEventMap_[pressId].objects)	
		handled |= obj->moveEvent(curPos, pressId);

	return handled;
}


bool Scene::releaseEvent(const Vector2 & curPos, int pressId)
{

	bool handled = false;
	Ray mouseRay = 
		(cameraObject_)?
		cameraObject_->unproject(screenToDeviceCoords(curPos)):
		camera_->unproject(screenToDeviceCoords(curPos));
	AppObject * appobj = intersectAppObject(mouseRay);
	if (appobj)
		handled = true;
	if (!handled)
	{
		BOOST_FOREACH(SceneObject * obj, pressEventMap_[pressId].objects)
			handled |= obj->releaseEvent(curPos, pressId);

		pressEventMap_.erase(pressId);
	}
	return handled;
}

bool Scene::keyPressEvent(int keyCode)
{
	static KeyPressEvent event;
	event.target = root_.get();
	event.keyCode = keyCode;

	bool handled = handleEvent(&event);		
	return handled;
}

bool Scene::keyReleaseEvent(int keyCode)
{
	static KeyReleaseEvent event;
	event.target = root_.get();
	event.keyCode = keyCode;

	bool handled = handleEvent(&event);
	return handled;
}

void Scene::play(Animation * animation, float time)
{
	stop(animation);
	float startTime = Global::currentTime() - time;
	playingAnimations_.push_back(AnimationPlayData(startTime, animation));
}

bool Scene::isPlaying(Animation * animation) const
{
	for (int i = 0; i < (int)playingAnimations_.size(); ++i)
	{
		if (playingAnimations_[i].animation == animation) return true;
	}
	return false;
}

void Scene::resume(Animation * animation)
{	
	stop(animation);
	playingAnimations_.push_back(AnimationPlayData(
		Global::currentTime() - animation->lastUpdateTime(), animation));
}

void Scene::stop(Animation * animation)
{
	vector<AnimationPlayData>::iterator iter = playingAnimations_.begin();
	for (; iter != playingAnimations_.end(); ++iter)
	{
		if ((*iter).animation == animation) 
		{
			playingAnimations_.erase(iter);
			break;
		}
	}
}

void Scene::resetCamera(float aspectRatio)
{
	camera_->SetTo2DArea(
		screenWidth_/2, screenHeight_/2,
		screenWidth_, screenHeight_, 60, aspectRatio);	
}


void Scene::setSceneObjData(SceneObject * obj)
{
	obj->setParentScene(this);
	BOOST_FOREACH(SceneObjectSPtr child, obj->children())
	{
		child->setParentScene(this);
		setSceneObjData(child.get());
	}
}

void Scene::allListeners(vector<EventListenerSPtr> * listeners) const
{
	listeners->clear();
	allListeners(root_.get(), listeners);
}

void Scene::allListeners(SceneObject * obj, 
	vector<EventListenerSPtr> * listeners) const
{
	BOOST_FOREACH(const EventListenerSPtr & listener, obj->eventListeners())	
		listeners->push_back(listener);	

	BOOST_FOREACH(const SceneObjectSPtr & child, obj->children())
		allListeners(child.get(), listeners);
}


void Scene::setKeepInit(bool keepInit) 
{
	keepInit_ = keepInit;
	handleInitBasedOnRefs();
}

void Scene::addObjRef(SceneReferencer * obj)
{
	objRefs_.insert(obj);
	handleInitBasedOnRefs();
}

void Scene::removeObjRef(SceneReferencer * obj)
{
	objRefs_.erase(obj);
	handleInitBasedOnRefs();
}

void Scene::handleInitBasedOnRefs(bool firstTimeInit)
{
	if (keepInit_) return;
	bool existsInitSubscene = false;
	BOOST_FOREACH(SceneReferencer * obj, objRefs_)
	{
		if (obj->isInit())
			existsInitSubscene = true;
	}
	
	if (existsInitSubscene && !isInit())
		init(parentDocument_->renderer(), firstTimeInit);

	else if (!existsInitSubscene && isInit()) uninit();
}



SceneObject * Scene::intersect(const Ray & ray, Vector3 * intPt) const
{	
	//AppObject* appobj = intersectAppObject(ray);
	//if (appobj)
	//	return root_.get();
	SceneObject * intObj = root_->intersect(intPt, ray);
	if (intObj) return intObj;
	else return root_.get();
}

AppObject * Scene::intersectAppObject(const Ray & ray) const
{
	AppObject * hoverObj = 0;

	Plane plane = Plane::fromPointNormal(Vector3(0, 0, 0), Vector3(0, 0, 1));
	Vector3 worldMousePt;
	plane.intersect(&worldMousePt, ray);

	BOOST_FOREACH(AppObjectSPtr appObj, appObjects_)
	{
		BoundingBox bbox = appObj->worldExtents();

		if (bbox.minPt.x <= worldMousePt.x && worldMousePt.x <= bbox.maxPt.x &&
			bbox.minPt.y <= worldMousePt.y && worldMousePt.y <= bbox.maxPt.y)
		{
			VisualAttrib * attr = appObj->visualAttrib();

			if (!attr || attr->isVisible())
				hoverObj = appObj.get();
		}
	}

	return hoverObj;
}

bool Scene::broadcastMessage(const std::string & message)
{
	return root_->receiveMessageEvent(message);
}

bool Scene::dependsOn(Scene * scene) const
{
	return root_->dependsOn(scene);
}

void Scene::dependsOnScenes(std::vector<Scene *> * dependsOnScenes, bool recursive) const
{
	root_->dependsOnScenes(dependsOnScenes, recursive);
}

bool Scene::isShowing(Scene * scene) const
{
	return root_->isShowing(scene);
}

void Scene::showingScenes(std::set<Scene *> * showingScenes) const
{
	root_->showingScenes(showingScenes);
}

void Scene::updateObjsWithSyncedAnimations()
{
	objsWithSyncedAnimations_.clear();
	map<Animation *, SceneObject *>::iterator iter;
	for (iter = syncedAnimations_.begin(); iter != syncedAnimations_.end(); ++iter)
	{
		Animation * key = (*iter).first;
		SceneObject * value = (*iter).second;

		objsWithSyncedAnimations_.insert(make_pair(value, key));
	}
}

void Scene::write(Writer & writer) const
{
	writer.write(name_, "name");	
	writer.write(animations_, "animations");
	writer.write(camera_, "camera");
	writer.write(cameraObject_, "cameraObject");
	writer.write(appObjects_, "appObjects");
	writer.write(root_, "root");
	writer.write(screenWidth_, "screenWidth");
	writer.write(screenHeight_, "screenHeight");
	writer.write(zBuffer_);
	writer.write(bgColor_, "bgColor");
	writer.write(zoom_);
	writer.write(syncedAnimations_);
	writer.write(zoomMagState_);
	writer.write(trackingUrl_);

	writer.write(lightIntensity_);
	writer.write(lightSpecularIntensity_);
	writer.write(lightAmbient_);
}

void Scene::writeXml(XmlWriter & w) const
{
	using namespace boost;
	w.setScene(this);
	string str;

	w.writeTag("Name", name_);

	w.writeTag("BgColor", bgColor_);
	if (zoom_) w.writeTag("Zoom", zoomMagState_);	
	w.writeTag("Width", lexical_cast<string>(screenWidth_));
	w.writeTag("Height", lexical_cast<string>(screenHeight_));

	if (!trackingUrl_.empty())
	{
		w.writeTag("TrackingUrl", trackingUrl_);
	}

	if (cameraObject_)
		w.writeTag("Camera", w.idMapping().getId(cameraObject_));

	if (zBuffer_) w.writeTag("ZBuffer", zBuffer_);
	w.startTag("DefaultLighting");
	w.writeTag("Intensity", lightIntensity_);
	w.writeTag("SpecularIntensity", lightSpecularIntensity_);
	w.writeTag("Ambient", lightAmbient_);
	w.endTag();
	
	w.startTag("AppObjects");
	BOOST_FOREACH(AppObjectSPtr appObj, appObjects_)
	{
		w.writeAppObject(appObj.get());
	}
	w.endTag();

	w.writeObject(root_.get());
	
	w.startTag("Animations");
	BOOST_FOREACH(AnimationSPtr anim, animations_)
	{
		string idStr = boost::lexical_cast<string>(w.idMapping().getId(anim.get()));
		string attrStr = "Id = \"" + idStr + "\"";
		w.startTag("Animation", attrStr);
		anim->writeXml(w);
		w.endTag();		
	}
	w.endTag();

	w.startTag("SyncedAnimations");
	map<Animation *, SceneObject *>::const_iterator itr;
	for (itr = syncedAnimations_.begin(); itr != syncedAnimations_.end(); ++itr)
	{
		Animation * animation = (*itr).first;
		SceneObject * obj = (*itr).second;
		w.startTag("SyncedAnimation");
		w.writeTag("Animation", animation);
		w.writeTag("Object", obj);
		w.endTag();
	}
	w.endTag();		
}

void Scene::read(Reader & reader, unsigned char version)
{
	reader.read(name_);
	reader.read(animations_);

	BOOST_FOREACH(AnimationSPtr animation, animations_)
	{
		animation->setParentScene(this);
	}

	if (version >= 6)
	{
		CameraSPtr camera;
		reader.read(camera);
	}
	else if (3 <= version && version < 6)
		reader.read(camera_);	
	else
	{
		unsigned char bytes[sizeof(Matrix) + 4 * sizeof(float) + 3 * sizeof(Vector3)];
		unsigned int dummy;
		reader.read(dummy);
		reader.read(bytes);
		camera_->readOld(bytes);

	}
	
	if (version >= 4)
	{
		reader.read(cameraObject_);		
	}
	
	if (version >= 2)
	{
		reader.read(appObjects_);
	}

	reader.read(root_);
	if (version < 1) 
	{
		unsigned int nextUnused;
		reader.read(nextUnused);
	}
	reader.read(screenWidth_);
	reader.read(screenHeight_);

	if (cameraObject_)
		cameraObject_->setAspectRatio(screenWidth_ / screenHeight_);
	resetCamera(screenWidth_ / screenHeight_);

	if (version < 5)
	{
		bool unused;
		reader.read(unused);
	}
	if (version >= 7) reader.read(zBuffer_);
	
	reader.read(bgColor_);

	if (version >= 8) reader.read(zoom_);

	if (version >= 9) reader.read(syncedAnimations_);
	updateObjsWithSyncedAnimations();

	if (version >= 10) reader.read(zoomMagState_);

	if (version >= 11) reader.read(trackingUrl_);

	if (version >= 12)
	{
		reader.read(lightIntensity_);
		reader.read(lightSpecularIntensity_);
		reader.read(lightAmbient_);
	}
	////

	setSceneObjData(root_.get());
}	

void Scene::readXmlRoot(XmlReader & r, xmlNode * parentNode)
{
	unsigned int objId = 0;
	r.getNodeAttribute(objId, parentNode, "Id");
	if (!objId) throw XmlException(parentNode, "No Valid object id");

	r.idMapping().setId(root_.get(), objId);

	root_->readXml(r, parentNode);
	setSceneObjData(root_.get());
}

void Scene::readXmlAnimations(XmlReader & r, xmlNode * parentNode)
{
	for(xmlNode * animNode = parentNode->children; animNode; animNode = animNode->next)
	{		
		if (animNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(animNode, "Animation"))
		{
			AnimationSPtr a(new Animation);
			a->readXml(r, animNode);		
			a->setParentScene(this);			
			animations_.push_back(a);
		}			
	}
}

void Scene::readXmlSyncedAnimations(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * sNode = parent->children; sNode; sNode = sNode->next)
	{		
		if (sNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(sNode, "SyncedAnimation"))
		{
			unsigned int animId = 0;
			unsigned int objId = 0;
			for(xmlNode * cNode = sNode->children; cNode; cNode = cNode->next)
			{		
				if (cNode->type != XML_ELEMENT_NODE) continue;
				if (r.getNodeContentIfName(animId, cNode, "Animation"));
				else if (r.getNodeContentIfName(objId, cNode, "Object"));
			}

			if (animId && objId)
				syncedAnimations_[(Animation *)animId] = (SceneObject *)objId;					
		}			
	}
}

void Scene::readXml(XmlReader & r, xmlNode * parent)
{
	r.setScene(this);
	
	unsigned int id = 0;
	r.getNodeAttribute(id, parent, "Id");
	if (!id) throw XmlException(parent, "No id");	
	r.idMapping().setId(this, id);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(name_, curNode, "Name"));
		else if (r.getNodeContentIfName(bgColor_, curNode, "BgColor"));
		else if (r.getNodeContentIfName(zoomMagState_, curNode, "Zoom"));
		else if (r.getNodeContentIfName(screenWidth_, curNode, "Width"));
		else if (r.getNodeContentIfName(screenHeight_, curNode, "Height"));
		else if (r.getNodeContentIfName(trackingUrl_, curNode, "TrackingUrl"));
		else if (r.getNodeContentIfName(val, curNode, "Camera"))
		{
			cameraObject_ = (CameraObject *)val;
		}
		else if (r.getNodeContentIfName(zBuffer_, curNode, "ZBuffer"));
		else if (r.isNodeName(curNode, "DefaultLighting"))
		{
			for(xmlNode * lightNode = curNode->children; lightNode; lightNode = lightNode->next)
			{		
				if (lightNode->type != XML_ELEMENT_NODE) continue;
				if (r.getNodeContentIfName(lightIntensity_, lightNode, "Intensity"));
				else if (r.getNodeContentIfName(lightSpecularIntensity_, lightNode, "SpecularIntensity"));
				else if (r.getNodeContentIfName(lightAmbient_, lightNode, "Ambient"));
			}
		}
		else if (r.isNodeName(curNode, "AppObjects"))
		{
			for(xmlNode * aNode = curNode->children; aNode; aNode = aNode->next)
			{		
				if (aNode->type != XML_ELEMENT_NODE) continue;

				AppObjectSPtr a;
				if (r.getAppObject(a, aNode))
				{
					appObjects_.push_back(a);
				}
			}
		}
		else if (r.isNodeName(curNode, "Root"))
		{	
			readXmlRoot(r, curNode);
		}
		else if (r.isNodeName(curNode, "Animations"))
		{
			readXmlAnimations(r, curNode);
		}
		else if (r.isNodeName(curNode, "SyncedAnimations"))
		{
			readXmlSyncedAnimations(r, curNode);
		}
	}
	
}

void Scene::initAppObjects()
{

	ContainerObjects containerObjects;
	getContainerObjects(&containerObjects);

	root_->initAppObjects();

	BOOST_FOREACH(AppObjectSPtr & appObject, appObjects_)
	{
		Global::initAppObject(containerObjects, appObject.get());		
	}	
}

void Scene::uninitAppObjects()
{
	BOOST_FOREACH(AppObjectSPtr & appObject, appObjects_)
	{
		Global::uninitAppObject(appObject.get());
	}

	root_->uninitAppObjects();
}

void Scene::getContainerObjects(ContainerObjects * containerObjects)
{
	containerObjects->clear();
	
	Scene * scene = this;
	SceneObject * containerObject = curContainerObject_;

	while(containerObject)
	{
		containerObjects->push_back(		
			make_pair(containerObject, scene));

		scene = containerObject->parentScene();
		assert(scene);
		containerObject = scene->curContainerObject_;		
	}

	containerObjects->push_back(make_pair(containerObject, scene));	
}

const Camera * Scene::camera() const
{
	return (cameraObject_) ? cameraObject_->camera() : camera_.get();
}

void Scene::cameraObjects(std::vector<CameraObject *> * cameraObjs) const
{
	cameraObjects(root_.get(), cameraObjs);
}

void Scene::cameraObjects(SceneObject * root, std::vector<CameraObject *> * cameraObjs) const
{
	static CameraObject camObj;
	const vector<SceneObjectSPtr> & children = root->children();

	BOOST_FOREACH(SceneObjectSPtr child, children)
	{
		if (child->type() == camObj.type())
		{
			cameraObjs->push_back((CameraObject *)child.get());
		}

		cameraObjects(child.get(), cameraObjs);
	}
	
}

void Scene::setSceneNum(const int & sceneNum)
{	
	string c = boost::lexical_cast<string> (sceneNum);
	string Scenename = name_ + c ;
	name_ = Scenename;	
}

void Scene::setScreenWidth(const float & screenWidth)
{
	screenWidth_ = screenWidth;
	resetCamera(screenWidth_ / screenHeight_);
	if (cameraObject_)
		cameraObject_->setAspectRatio(screenWidth_ / screenHeight_);
}

void Scene::setScreenHeight(const float & screenHeight)
{
	screenHeight_ = screenHeight;
	resetCamera(screenWidth_ / screenHeight_);
	if (cameraObject_)
		cameraObject_->setAspectRatio(screenWidth_ / screenHeight_);
}

void Scene::setUserCamera(CameraObject * userCamera)
{	
	if (userCamera != NULL)
	{
		if (!this->appObjects().empty())		
			throw Exception("There can be no app objects in a scene with a custom camera");

		std::vector<Scene *> scenes;
		dependsOnScenes(&scenes);

		if (!scenes.empty())
			throw Exception("There can be no subscenes in a scene with a custom camera");
	}

	cameraObject_ = userCamera;
}

void Scene::setZBuffer(const bool & val)
{
	if (val == true)
	{
		if (!this->appObjects().empty())		
			throw Exception("There can be no app objects in a scene with Z-buffering enabled");

		std::vector<Scene *> scenes;
		dependsOnScenes(&scenes);

		if (!scenes.empty())
			throw Exception("There can be no subscenes in a scene with Z-buffering enabled");

		vector<SceneObject *> objs;
		getSceneObjectsByID(MaskObject().type(), &objs);

		if (!objs.empty())
			throw Exception("There can be no MaskObjects in a scene with Z-buffering enabled");
	}

	zBuffer_ = val;
}

void Scene::setZoom(const bool & val)
{
	//if (val == true)
	//{
	//	//if (!this->appObjects().empty())		
	//	//	throw Exception("There can be no app objects in a scene with Zoom In/Out enabled");

	//	std::vector<Scene *> scenes;
	//	dependsOnScenes(&scenes);

	//	if (!scenes.empty())
	//		throw Exception("There can be no subscenes in a scene with Zoom In/Out enabled");
	//}

	zoom_ = val;
}

void Scene::getSceneObjectsByID(
	const boost::uuids::uuid & type, std::vector<SceneObject *> * objs) const
{
	root_->getSceneObjectsByID(type, objs);
}
void Scene::playSyncedAnimations(SceneObject * obj)
{
	typedef multimap<SceneObject *, Animation *>::iterator IterType;
	IterType itr1, itr2;
	pair<IterType, IterType> ret =  objsWithSyncedAnimations_.equal_range(obj);
	for (IterType itr = ret.first; itr != ret.second; ++itr)
	{
		Animation * anim = (*itr).second;
		play(anim);
	}
}

void Scene::stopSyncedAnimations(SceneObject * obj)
{
	typedef multimap<SceneObject *, Animation *>::iterator IterType;
	IterType itr1, itr2;
	pair<IterType, IterType> ret =  objsWithSyncedAnimations_.equal_range(obj);
	for (IterType itr = ret.first; itr != ret.second; ++itr)
	{
		Animation * anim = (*itr).second;
		stop(anim);
	}
}

void Scene::seekSyncedAnimations(SceneObject * obj, float time)
{
	typedef multimap<SceneObject *, Animation *>::iterator IterType;
	IterType itr1, itr2;
	pair<IterType, IterType> ret =  objsWithSyncedAnimations_.equal_range(obj);
	for (IterType itr = ret.first; itr != ret.second; ++itr)
	{
		Animation * anim = (*itr).second;
		play(anim, time);
	}
}

void Scene::resumeSyncedAnimations(SceneObject * obj)
{
	typedef multimap<SceneObject *, Animation *>::iterator IterType;
	IterType itr1, itr2;
	pair<IterType, IterType> ret =  objsWithSyncedAnimations_.equal_range(obj);
	for (IterType itr = ret.first; itr != ret.second; ++itr)
	{
		Animation * anim = (*itr).second;
		resume(anim);
	}
}

SceneObject * Scene::syncObject(Animation * animation) const
{
	map<Animation *, SceneObject *>::const_iterator iter;
	iter = syncedAnimations_.find(animation);
	if (iter != syncedAnimations_.end())
	{
		return (*iter).second;
	}
	else return NULL;
}

void Scene::setSyncObject(Animation * animation, SceneObject * syncObj)
{
	if (!syncObj)
	{
		//error
		return;
	}

	typedef multimap<SceneObject *, Animation *>::iterator IterType;
	IterType itr1, itr2;
	syncedAnimations_[animation] = syncObj;

	pair<IterType, IterType> ret =  objsWithSyncedAnimations_.equal_range(syncObj);
	bool exists = false;
	for (IterType itr = ret.first; itr != ret.second; ++itr)
	{
		Animation * anim = (*itr).second;
		if (anim == animation) exists = true;
	}

	if (!exists)
	{
		objsWithSyncedAnimations_.insert(make_pair(syncObj, animation));
	}	
}

void Scene::removeSyncedAnimation(Animation * animation)
{
	map<Animation *, SceneObject *>::iterator iter = syncedAnimations_.find(animation);
	SceneObject * syncObj = NULL;
	if (iter != syncedAnimations_.end())
	{
		syncObj = (*iter).second;
		syncedAnimations_.erase(iter);
	}

	//-----

	typedef multimap<SceneObject *, Animation *>::iterator IterType;
	IterType itr1, itr2;
	pair<IterType, IterType> ret =  objsWithSyncedAnimations_.equal_range(syncObj);
	for (IterType itr = ret.first; itr != ret.second;)
	{
		IterType eraseIter = itr++;
		Animation * anim = (*eraseIter).second;
		if (anim == animation)
			objsWithSyncedAnimations_.erase(eraseIter);
	}
}

void Scene::removeSyncObject(SceneObject * object)
{
	typedef multimap<SceneObject *, Animation *>::iterator IterType;
	IterType itr1, itr2;
	pair<IterType, IterType> ret =  objsWithSyncedAnimations_.equal_range(object);
	for (IterType itr = ret.first; itr != ret.second; ++itr)
	{		
		Animation * anim = (*itr).second;
		syncedAnimations_.erase(anim);
	}

	//------

	objsWithSyncedAnimations_.erase(object);
}

vector<Animation *> Scene::syncedAnimations(SceneObject * syncObj) const
{
	vector<Animation *> retVec;

	typedef multimap<SceneObject *, Animation *>::const_iterator IterType;
	IterType itr1, itr2;
	pair<IterType, IterType> ret = objsWithSyncedAnimations_.equal_range(syncObj);
	for (IterType itr = ret.first; itr != ret.second; ++itr)
	{		
		Animation * anim = (*itr).second;
		retVec.push_back(anim);
	}

	return retVec;
}
void Scene::resizeText(GfxRenderer * gl)
{
	if (!this->isInit()) return;
	static boost::uuids::uuid typeText = Text().type();
	std::vector<SceneObject *> objects;
	objects.clear();
	this->getSceneObjectsByID(typeText, &objects);
	BOOST_FOREACH(SceneObject * obj, objects)
	{
		Text* text = (Text*)obj;
		text->init(gl);
	}
	static boost::uuids::uuid typeSceneChanger = SceneChanger().type();
	objects.clear();
	this->getSceneObjectsByID(typeSceneChanger, &objects);
	BOOST_FOREACH(SceneObject * obj, objects)
	{
		SceneChanger* sceneChanger = (SceneChanger*)obj;
		sceneChanger->resizeText(gl);
	}

	static boost::uuids::uuid typeCustomObj = CustomObject().type();
	objects.clear();
	this->getSceneObjectsByID(typeCustomObj, &objects);
	BOOST_FOREACH(SceneObject * obj, objects)
	{
		CustomObject* customObject = (CustomObject*)obj;
		customObject->init(gl);
	}

}

void Scene::requestLocalSaveObject()
{
	root_->requestLocalSaveObject();
}

void Scene::setPauseTime(const float & time)
{
	for (int i = 0; i < (int)playingAnimations_.size(); ++i)
	{
		playingAnimations_[i].startTime += time;
	}
	if (isLoaded())
	{
		vector<EventListenerSPtr> listeners;
		allListeners(root_.get(), &listeners);
		BOOST_FOREACH(const EventListenerSPtr & el, listeners)
		{
			std::vector<ActionSPtr> actions = el->actions();
			if(el->isWorking())
			{
				BOOST_FOREACH(const ActionSPtr & action, actions)
				{
					static boost::uuids::uuid waituntiltype = WaitUntilAction().type();
					WaitUntilAction * waituntil = (WaitUntilAction*)action.get();
					if (action->type() == waituntiltype)
					{
						WaitUntilAction * waituntil = (WaitUntilAction*)action.get();
						waituntil->setDelayedTime(waituntil->delayedTime() + time);
						//float temp = waituntil->waitUntilTime();
						//waituntil->setWaitUntilTime(temp + time);
					} 				
				}
			}
		}
	}
}

Scene * Scene::rootScene()
{
	ContainerObjects containerObjects;
	getContainerObjects(&containerObjects);

	return containerObjects.back().second;
}


void Scene::textObjects(std::vector<Text *> * textObjs) const
{
	textObjects(root_.get(), textObjs);
}

void Scene::textObjects(SceneObject * root, std::vector<Text *> * textObjs) const
{
	static Text textObj;
	const vector<SceneObjectSPtr> & children = root->children();

	BOOST_FOREACH(SceneObjectSPtr child, children)
	{
		if (child->type() == textObj.type())
		{
			textObjs->push_back((Text *)child.get());
		}

		textObjects(child.get(), textObjs);
	}
}
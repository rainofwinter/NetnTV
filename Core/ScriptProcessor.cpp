#include "stdafx.h"
#include "ScriptProcessor.h"
#include "Exception.h"
#include "Global.h"
#include "Document.h"
#include "Scene.h"
#include "Root.h"
#include "SceneObject.h"

#include "ClickEvent.h"
#include "DoubleClickEvent.h"
#include "EventListener.h"

#include "DocumentTemplate.h"

#include "Transform.h"
#include "Animation.h"
#include "Map.h"
#include "VideoPlayer.h"
#include "AudioPlayer.h"
#include "XMLHttpRequest.h"
#include "FileUtils.h"

#include "PixelBuffer.h"
#include "SpatialPartitioning.h"

#include <jsobj.h>
#include <jsstr.h>

#include <utf8.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
/*
JSBool JSHasInstance(JSContext *c, JSObject *obj, const jsval * val, JSBool *vp)
{
	*vp = JS_FALSE;
	if (JSVAL_IS_OBJECT(*val)) {
		JSObject *p;
		JSClass *js_class = JS_GET_CLASS(c, obj);
		p = JSVAL_TO_OBJECT(*val);
		if (JS_InstanceOf(c, p, js_class, NULL) ) *vp = JS_TRUE;
	}
	return JS_TRUE;
}
*/
JSClass InitClass(const char * name, uint32 flags, JSPropertyOp getProperty, 
	JSStrictPropertyOp setProperty, JSFinalizeOp finalize, JSTraceOp mark)
{
	JSClass theClass; 
	memset(&theClass, 0, sizeof(JSClass));
	theClass.name = name;
	theClass.flags = flags;
	theClass.addProperty = JS_PropertyStub;
	theClass.delProperty = JS_PropertyStub;
	theClass.getProperty = getProperty;
	theClass.setProperty = setProperty;
	theClass.enumerate = JS_EnumerateStub;
	theClass.resolve = JS_ResolveStub;
	theClass.convert = JS_ConvertStub;
	theClass.finalize = finalize;
	
	if (mark)
	{
		theClass.mark = JS_CLASS_TRACE(mark);
	}
	return theClass;
}

bool getPropertyFloat(
	JSContext * cx, JSObject * obj, const char * propName, float * outVal)
{
	jsval val;
	if (JS_GetProperty(cx, obj, propName, &val))
	{
		if (JSVAL_IS_INT(val))
		{
			*outVal = (float)JSVAL_TO_INT(val);
			return true;
		}	
		else if (JSVAL_IS_DOUBLE(val))
		{
			*outVal = (float)JSVAL_TO_DOUBLE(val);
			return true;
		}
	}
	return false;	
}

bool getFloat(JSContext * cx, jsval val, float * outVal)
{
	if (JSVAL_IS_INT(val))
	{
		*outVal = (float)JSVAL_TO_INT(val);
		return true;
	}	
	else if (JSVAL_IS_DOUBLE(val))
	{
		*outVal = (float)JSVAL_TO_DOUBLE(val);
		return true;
	}
	return false;
}

bool getInt(JSContext * cx, jsval val, int * outVal)
{
	if (JSVAL_IS_INT(val))
	{
		*outVal = (int)JSVAL_TO_INT(val);
		return true;
	}
	return false;
}

bool getString(JSContext * cx, jsval val, std::string * outStr)
{
	if (JSVAL_IS_STRING(val))
	{
		JSString * jsStr = JSVAL_TO_STRING(val);
		char * cstr = JS_EncodeString(cx, jsStr);
		*outStr = cstr;
		JS_free(cx, cstr);			
		return true;
	}
	return false;
}


bool getPropertyString(
	JSContext * cx, JSObject * obj, const char * propName, std::string * outStr)
{
	jsval val;
	if (JS_GetProperty(cx, obj, propName, &val))
	{
		if (JSVAL_IS_STRING(val))
		{
			JSString * jsStr = JSVAL_TO_STRING(val);
			char * cstr = JS_EncodeString(cx, jsStr);
			*outStr = cstr;
			JS_free(cx, cstr);			
			return true;
		}		
	}

	return false;	
}

bool getPropertyUint(
	JSContext * cx, JSObject * obj, const char * propName, unsigned int * outVal)
{
	jsval val;
	int intVal;
	if (JS_GetProperty(cx, obj, propName, &val))
	{
		if (JSVAL_IS_INT(val))
		{
			intVal = (int)JSVAL_TO_INT(val);
			if (intVal >= 0)
			{
				*outVal = (unsigned int)intVal;
				return true;
			}
		}	
		else if (JSVAL_IS_DOUBLE(val))
		{
			intVal = (int)JSVAL_TO_DOUBLE(val);
			if (intVal >= 0)
			{
				*outVal = (unsigned int)intVal;
				return true;
			}			
		}
	}
	return false;	
}

bool getPropertyInt(
	JSContext * cx, JSObject * obj, const char * propName, int * outVal)
{
	jsval val;
	if (JS_GetProperty(cx, obj, propName, &val))
	{
		if (JSVAL_IS_INT(val))
		{
			*outVal = (int)JSVAL_TO_INT(val);
			return true;
		}	
		else if (JSVAL_IS_DOUBLE(val))
		{
			*outVal = (int)JSVAL_TO_DOUBLE(val);
			return true;
		}
	}
	return false;	
}


bool getPropertyBool(
	JSContext * cx, JSObject * obj, const char * propName, bool * outVal)
{
	jsval val;
	if (JS_GetProperty(cx, obj, propName, &val))
	{
		if (JSVAL_IS_BOOLEAN(val))
		{
			*outVal = (bool)JSVAL_TO_BOOLEAN(val);
			return true;
		}	
		else if (JSVAL_IS_INT(val))
		{
			*outVal = (bool)JSVAL_IS_INT(val);
			return true;
		}
	}
	return false;	
}


////////////////////////////////////////////////////////////////////////////////
JSObjectRoots::JSObjectRoots()
{
	jsObjects_.reserve(1024);
	firstFreeIndex_ = 0;
}

void JSObjectRoots::rootJSObject(JSContext * cx, JSObject * origObject)
{
	if (firstFreeIndex_ == jsObjects_.capacity())
	{
		if (jsObjectsSet_.find(origObject) == jsObjectsSet_.end())
		{
			JSObject * rooted = *jsObjectsSet_.insert(origObject).first;
			JS_AddObjectRoot(cx, &rooted);
		}
	}
	else
	{
		if (firstFreeIndex_ == jsObjects_.size()) 
			jsObjects_.push_back(JSObjectRootData());

		JSObjectRootData & rootData = jsObjects_[firstFreeIndex_++];
		rootData.stored = true;
		rootData.rootedObject = origObject;	
		JS_AddObjectRoot(cx, &rootData.rootedObject);

		while(firstFreeIndex_ < (int)jsObjects_.size() &&
		jsObjects_[firstFreeIndex_].stored)
		{
			firstFreeIndex_++;
		}

	}
}

void JSObjectRoots::unrootJSObject(JSContext * cx, JSObject * delObject)
{
	
	for (int i = (int)jsObjects_.size() - 1; i >= 0; --i)
	{
		if (jsObjects_[i].rootedObject == delObject)
		{			
			JS_RemoveObjectRoot(cx, &jsObjects_[i].rootedObject);
			jsObjects_[i].stored = false;
			if (i < firstFreeIndex_) firstFreeIndex_ = i;
			return;
		}
	}

	std::set<JSObject *>::iterator iter = jsObjectsSet_.find(delObject);
	if (iter != jsObjectsSet_.end())
	{
		JSObject * rooted = *iter;
		JS_RemoveObjectRoot(cx, &rooted);
		jsObjectsSet_.erase(iter);		
	}
	
}

void JSObjectRoots::unrootAllJSObjects(JSContext * cx)
{
	
	for (int i = 0; i < (int)jsObjects_.size(); ++i)
	{
		JS_RemoveObjectRoot(cx, &jsObjects_[i].rootedObject);
		jsObjects_[i].stored = false;
	}
	firstFreeIndex_ = 0;

	std::set<JSObject *>::iterator iter;
	for (iter = jsObjectsSet_.begin(); iter != jsObjectsSet_.end(); ++iter)
	{
		JSObject * rooted = *iter;
		JS_RemoveObjectRoot(cx, &rooted);
	}
	jsObjectsSet_.clear();
	
}


////////////////////////////////////////////////////////////////////////////////
///javascript alert function

size_t _js_strlen(const jschar *s)
{
    const jschar *t;

    for (t = s; *t != 0; t++)
        continue;
    return (size_t)(t - s);
}

std::string jsCharStrToStdString(const jschar * msg)
{
#ifdef _WIN32 || OSX
	std::locale loc("KOR");
	std::wstring ws(msg);
	std::string str = wideCharStringToMultiByteString(ws, loc);

#else
    std::string str;
	size_t len = _js_strlen(msg);
	utf8::utf16to8(msg, msg + len, back_inserter(str));
#endif
	return str;	
}

std::string jsStrToStdString(JSContext * cx, JSString * msg)
{
	std::string outStr;
	char * cstr = JS_EncodeString(cx, msg);
	outStr = cstr;
	JS_free(cx, cstr);
	return outStr;
}


std::wstring jsCharStrToStdWString(const jschar * msg)
{	
	std::wstring wstr;
	size_t len = _js_strlen(msg);	
	wstr.reserve(len);

	for (size_t i = 0; i < len; ++i)
	{
		wstr.push_back((wchar_t)msg[i]);
	}
	return wstr;
}

std::wstring multiByteStringToWideCharString(std::string const& str, std::locale loc)
{
	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
	codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
	std::mbstate_t state = std::mbstate_t();
	std::vector<wchar_t> buf(str.size() + 1);
	char const* in_next = str.c_str();
	wchar_t* out_next = &buf[0];
	std::codecvt_base::result r = codecvt.in(state, 
		str.c_str(), str.c_str() + str.size(), in_next, 
		&buf[0], &buf[0] + buf.size(), out_next);
	if (r == std::codecvt_base::error)
		throw std::runtime_error("can't convert string to wstring");   
	return std::wstring(&buf[0]);
}

std::string wideCharStringToMultiByteString(std::wstring const& str, std::locale loc)
{
	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
	codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
	std::mbstate_t state = std::mbstate_t();
	std::vector<char> buf((str.size() + 1) * codecvt.max_length());
	wchar_t const* in_next = str.c_str();
	char* out_next = &buf[0];
	std::codecvt_base::result r = codecvt.out(state, 
		str.c_str(), str.c_str() + str.size(), in_next, 
		&buf[0], &buf[0] + buf.size(), out_next);
	if (r == std::codecvt_base::error)
		throw std::runtime_error("can't convert wstring to string");   
	return std::string(&buf[0]);
}

JSBool Global_alert(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	const jschar * msg;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &msg)) return JS_FALSE;	
	Global::msgBox(jsCharStrToStdString(msg).c_str());
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Global_include(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);	
	const jschar * msg;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &msg)) return JS_FALSE;
	ScriptProcessor * s = (ScriptProcessor *)obj->privateData;
	s->execute(jsCharStrToStdString(msg).c_str(), s->curExecGlobal());
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

///javascript setTimeout function
JSBool Global_setTimeout(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	JSObject * execObj;
	uint32 timeInterval;
	if (!JS_ConvertArguments(cx, argc, argv, "ou", &execObj, &timeInterval))
		return JS_FALSE;

	ScriptProcessor * s = (ScriptProcessor *)obj->privateData;
	unsigned int intervalId = 0;
	if (JS_ObjectIsFunction(cx, execObj))
	{	
		s->rootJSObject(execObj);
		intervalId = s->setFuncTimeout(execObj, timeInterval);
		
	}
	else return JS_FALSE;

	JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(intervalId));
	return JS_TRUE;
}

JSBool Global_clearTimeout(JSContext * cx, uintN argc, jsval * vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	int32 intervalId;
	if (!JS_ConvertArguments(cx, argc, argv, "i",&intervalId)) return JS_FALSE;
	ScriptProcessor * s = (ScriptProcessor *)obj->privateData;
	s->clearTimeout(intervalId);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Global_setInterval(JSContext * cx, uintN argc, jsval * vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	JSObject * execObj;
	uint32 timeInterval;
	if (!JS_ConvertArguments(cx, argc, argv, "ou", &execObj, &timeInterval))
		return JS_FALSE;

	ScriptProcessor * s = (ScriptProcessor *)obj->privateData;
	unsigned int intervalId = 0;
	if (JS_ObjectIsFunction(cx, execObj))
	{	
		s->rootJSObject(execObj);
		intervalId = s->setFuncInterval(execObj, timeInterval);		
	}
	else 
	{
		JS_ReportError(cx, "Global::setInterval - Parameter must be a function.");
		return JS_FALSE;
	}

	JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(intervalId));
	return JS_TRUE;
}

JSBool Global_clearInterval(JSContext * cx, uintN argc, jsval * vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);

	uint32 intervalId;
	if (!JS_ConvertArguments(cx, argc, argv, "u",&intervalId)) return JS_FALSE;
	ScriptProcessor * s = (ScriptProcessor *)obj->privateData;
	s->clearInterval(intervalId);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Global_readStringFromFile(JSContext * cx, uintN argc, jsval * vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);	
	jschar * jsChars;
	if (!JS_ConvertArguments(cx, argc, argv, "W",&jsChars)) return JS_FALSE;

	std::string fileName = s->getActualFileName(jsCharStrToStdString(jsChars));	
	std::string readStr;
	
	FILE * file = fopen(fileName.c_str(), "rb");

	if (file)
	{
		const int BUFFER_SIZE = 4096;
		char buffer[BUFFER_SIZE];
		
		size_t read;
		do 
		{	
			read = fread(buffer, 1, BUFFER_SIZE, file);
			readStr.insert(readStr.end(), buffer, buffer + read);
		} while (read == BUFFER_SIZE);

		fclose(file);
	}
	std::vector<jschar> retStr;
	
	try {
		utf8::utf8to16(readStr.begin(), readStr.end(), back_inserter(retStr));
	} catch(...) {
	
		JS_ReportError(cx, "global::readStringFromFile - Could not decode UTF8");
		return JS_FALSE;
	}
	
	if (retStr.size() == 0)
		JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "")));
	else
		JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewUCStringCopyN(cx, &retStr[0], retStr.size())));

	return JS_TRUE;
}


JSBool Global_writeStringToFile(JSContext * cx, uintN argc, jsval * vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	jschar * jsChars;
	jschar * jsChars2;
	if (!JS_ConvertArguments(cx, argc, argv, "WW",&jsChars, &jsChars2)) return JS_FALSE;

	std::string fileName = jsCharStrToStdString(jsChars);

	std::string writeStr;
	utf8::utf16to8(jsChars2, jsChars2 + _js_strlen(jsChars2), back_inserter(writeStr));

	FILE * file = fopen(fileName.c_str(), "wb");

	if (file)
	{
		fwrite(writeStr.c_str(), 1, writeStr.size(), file);
		fclose(file);
		JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(true));		
	}
	else 
		JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(false));	

	return JS_TRUE;
}

JSBool Global_deleteFile(JSContext * cx, uintN argc, jsval * vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	jschar * jsChars;
	if (!JS_ConvertArguments(cx, argc, argv, "W",&jsChars)) return JS_FALSE;

	std::string fileName = jsCharStrToStdString(jsChars);	

	int ret = remove(fileName.c_str());	
	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(ret == 0));

	return JS_TRUE;
}

JSBool Global_playVideoRegion(JSContext *cx, uintN argc, jsval *vp)
{
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	jsval * argv = JS_ARGV(cx, vp);	
	jschar * jsFile;
	int32 jsX, jsY, jsW, jsH;
	JSBool jsIsUrl, jsShowControls;	
	if (!JS_ConvertArguments(cx, argc, argv, "Wiiiibb",
		&jsFile, &jsX, &jsY, &jsW, &jsH, &jsIsUrl, &jsShowControls)) return JS_FALSE;
	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	std::string file;
	if (!jsIsUrl)
		file = s->getActualFileName(jsCharStrToStdString(jsFile));
	else
		file = jsCharStrToStdString(jsFile);

	Global::instance().videoPlayer()->activateRegion(
		NULL, file, (int)jsX, (int)jsY, (int)jsW, (int)jsH, 
		(bool)jsIsUrl, (bool)jsShowControls, 0.0f);

	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	return JS_TRUE;
}

JSBool Global_playVideo(JSContext *cx, uintN argc, jsval *vp)
{
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	jsval * argv = JS_ARGV(cx, vp);	
	jschar * jsFile;
	JSBool jsIsUrl, jsShowControls;	
	if (!JS_ConvertArguments(cx, argc, argv, "Wbb",
		&jsFile, &jsIsUrl, &jsShowControls)) return JS_FALSE;
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	std::string file;
	if (!jsIsUrl)
		file = s->getActualFileName(jsCharStrToStdString(jsFile));
	else
		file = jsCharStrToStdString(jsFile);

	Global::instance().videoPlayer()->activate(NULL,
		file, (bool)jsIsUrl, (bool)jsShowControls, 0.0f);
	
	

	return JS_TRUE;
}

JSBool Global_playSound(JSContext * cx, uintN argc, jsval * vp)
{
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	jsval * argv = JS_ARGV(cx, vp);	
	jschar * jsFile;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &jsFile)) return JS_FALSE;
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	std::string file = s->getActualFileName(jsCharStrToStdString(jsFile));
	Global::instance().audioPlayer()->play(file, 0.0f);	

	return JS_TRUE;
}

JSBool Global_curRecSoundAmplitude(JSContext * cx, uintN argc, jsval * vp)
{
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(Global::instance().audioPlayer()->curRecordVolume()));
	return JS_TRUE;
}

JSBool Global_stopVideo(JSContext * cx, uintN argc, jsval *vp)
{
	Global::instance().videoPlayer()->deactivate();	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Global_getFreeDiskSpace(JSContext *cx, uintN argc, jsval *vp)
{
	double val = Global::freeDiskSpace();	
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL((uint32)val));
	return JS_TRUE;
}

JSBool Global_getTotalDiskSpace(JSContext *cx, uintN argc, jsval *vp)
{
	double val = Global::totalDiskSpace();	
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL((uint32)val));
	return JS_TRUE;
}

JSBool Global_broadcastExternalMessage(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);	
	jschar * jsMsg;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &jsMsg)) return JS_FALSE;
	std::string msg = jsCharStrToStdString(jsMsg);
	s->document()->broadcastExternalMessage(msg);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}


//-----------------------------------------------------------------------------

JSBool Global_createRotationMatrix(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	double angle, x, y, z;
	if (!JS_ConvertArguments(cx, argc, argv, "dddd", &angle, &x, &y, &z)) return JS_FALSE;	
	Matrix ret = Matrix::Rotate((float)angle, (float)x, (float)y, (float)z);
	JSObject * retJs = ret.createScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retJs));
	return JS_TRUE;
}

JSBool Global_createTranslationMatrix(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	double x, y, z;
	if (!JS_ConvertArguments(cx, argc, argv, "ddd", &x, &y, &z)) return JS_FALSE;	
	Matrix ret = Matrix::Translate((float)x, (float)y, (float)z);
	JSObject * retJs = ret.createScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retJs));
	return JS_TRUE;
}


JSBool Global_createScaleMatrix(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	double x, y, z;
	if (!JS_ConvertArguments(cx, argc, argv, "ddd", &x, &y, &z)) return JS_FALSE;	
	Matrix ret = Matrix::Scale((float)x, (float)y, (float)z);
	JSObject * retJs = ret.createScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retJs));
	return JS_TRUE;
}

//-----------------------------------------------------------------------------

static JSFunctionSpec GlobalFuncs[] = {
    JS_FS("alert",   Global_alert,   0, 0),
	JS_FS("setTimeout", Global_setTimeout, 2, 0),
	JS_FS("setInterval", Global_setInterval, 2, 0),	
	JS_FS("clearInterval", Global_clearInterval, 1, 0),	
	JS_FS("clearTimeout", Global_clearTimeout, 1, 0),	

	JS_FS("readStringFromFile", Global_readStringFromFile, 1, 0),	
	JS_FS("writeStringToFile", Global_writeStringToFile, 2, 0),	
	JS_FS("deleteFile", Global_deleteFile, 1, 0),

	JS_FS("playVideoRegion", Global_playVideoRegion, 7, 0),
	JS_FS("playVideo", Global_playVideo, 3, 0),	
	JS_FS("stopVideo", Global_stopVideo, 0, 0),
	JS_FS("playSound", Global_playSound, 1, 0),
	JS_FS("curRecSoundVolume", Global_curRecSoundAmplitude, 0, 0),

	JS_FS("getFreeDiskSpace", Global_getFreeDiskSpace, 0, 0),
	JS_FS("getTotalDiskSpace", Global_getTotalDiskSpace, 0, 0),

	JS_FS("createRotationMatrix", Global_createRotationMatrix, 4, 0),
	JS_FS("createTranslationMatrix", Global_createTranslationMatrix, 3, 0),
	JS_FS("createScaleMatrix", Global_createScaleMatrix, 3, 0),

	JS_FS("broadcastExternalMessage", Global_broadcastExternalMessage, 1, 0),

	JS_FS("include", Global_include, 1, 0),

    JS_FS_END
};
////////////////////////////////////////////////////////////////////////////////
JSClass jsConsoleClass = InitClass("Console", 0, 
		JS_PropertyStub, JS_StrictPropertyStub, JS_FinalizeStub, 0);

JSBool Console_log(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	jschar * msg;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &msg)) return JS_FALSE;	
	{
		std::string str = jsCharStrToStdString(msg);		
		Global::logMessage(str.c_str());
	}	

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}


static JSFunctionSpec ConsoleFuncs[] = {
    JS_FS("log",   Console_log,   1, 0),

    JS_FS_END
};

////////////////////////////////////////////////////////////////////////////////

extern JSFunctionSpec DocumentFuncs[];


////////////////////////////////////////////////////////////////////////////////

void SceneObject_finalize(JSContext * cx, JSObject * obj)
{		
	SceneObject * privateData = (SceneObject *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	
	s->discardKeepAliveData(privateData);
}



///The error reporter callback.
void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
	stringstream ss;
	ss << 
		(report->filename ? report->filename : "<no filename>") << ":" <<
		(unsigned int) report->lineno << ":" <<
		message << "\n";

	Global::logMessage(ss.str().c_str());
	
}


////////////////////////////////////////////////////////////////////////////////

ScriptProcessor::ScriptProcessor(Document * document)
{
	document_ = document;
	init_ = false;

	//global class
	///The class of the global object.
	memset(&globalClass, 0, sizeof(JSClass));
	globalClass.name = "global";
	globalClass.flags = JSCLASS_GLOBAL_FLAGS | JSCLASS_HAS_PRIVATE;
	globalClass.addProperty = JS_PropertyStub;
	globalClass.delProperty = JS_PropertyStub;
	globalClass.getProperty = JS_PropertyStub;
	globalClass.setProperty = JS_StrictPropertyStub;
	globalClass.enumerate = JS_EnumerateStub;
	globalClass.resolve = JS_ResolveStub;
	globalClass.convert = JS_ConvertStub;
	globalClass.finalize = JS_FinalizeStub;

	documentClass = InitClass("Document", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, 
		JS_PropertyStub, JS_StrictPropertyStub, 0, Document::gcMark);
		

	curExecScriptId_ = 0;
}

#ifdef _DEBUG
JS_BEGIN_EXTERN_C
extern JS_PUBLIC_API(JSBool)
JS_DumpHeap(JSContext *cx, FILE *fp, void* startThing, uint32 startKind,
            void *thingToFind, size_t maxDepth, void *thingToIgnore);
JS_END_EXTERN_C
#endif

ScriptProcessor::~ScriptProcessor()
{
#ifdef _DEBUG
	/*
	FILE * file = fopen("c:/st/kbs bug/dump.txt", "wb");
	JS_DumpHeap(cx_, file, 0, 0, 0, 99999, 0);
	fclose(file);
	*/
#endif
	uninit();
}



void ScriptProcessor::uninit()
{
	if (!init_) return;

	execScriptData_.clear();

	privateDataAllocs_.deleteAllPrivateData();
	unrootAllJSObjects();
	//JS_RemoveRoot(cx_, &documentInstance_);
	JS_DestroyContext(cx_);
    JS_DestroyRuntime(rt_);
    JS_ShutDown();

	

	init_ = false;
}

JSObject * ScriptProcessor::newGlobal()
{
	JSObject * global;
	global = JS_NewGlobalObject(cx_, &globalClass);
    if (global == NULL) throw Exception("JS_NewGlobalObject");

	JS_SetPrivate(cx_, global, this);

	if (!JS_DefineFunctions(cx_, global, GlobalFuncs))
        throw Exception("JS_DefineFunctions");
	
	if (!JS_InitStandardClasses(cx_, global))
		throw Exception("JS_InitStandardClasses");

	JSObject * consoleInstance = JS_DefineObject(cx_, global, "console", 0, consoleProto, 0);
	
	JSObject * documentInstance = 
		JS_DefineObject(cx_, global, "document", &documentClass, documentProto, 0);
	JS_SetPrivate(cx_, documentInstance, document_);

	JSObject * xmlHttpRequestProto = XMLHttpRequest::createScriptObjectProto(this, global);
	JSObject * pixelBufferProto = PixelBuffer::createScriptObjectProto(this, global);
	JSObject * kdTreeProto = KDTree::createScriptObjectProto(this, global);
	JSObject * vector3Proto = Vector3::scriptObjectProto(this, global);
	JSObject * pathProto = Path::scriptObjectProto(this, global);
	JSObject * vector2Proto = Vector2::scriptObjectProto(this, global);
	JSObject * matrixProto = Matrix::scriptObjectProto(this, global);
	//Apparently the proto object doesn't need to be rooted.
	//JS_GC(cx_);

	return global;
}

void ScriptProcessor::init()
{
	if (init_) return;
	init_ = true;
	
    /* Create a JS runtime. */
    rt_ = JS_NewRuntime(8 * 1024 * 1024);
    if (rt_ == NULL)
		throw std::runtime_error("JS_NewRuntime");

    /* Create a context. */
    cx_ = JS_NewContext(rt_, 8192);
	JS_SetContextPrivate(cx_, this);

    if (cx_ == NULL)
        throw std::runtime_error("JS_NewContext");
    JS_SetOptions(cx_, JSOPTION_VAROBJFIX /*| JSOPTION_JIT | JSOPTION_METHODJIT*/);
    JS_SetVersion(cx_, JSVERSION_LATEST);
    JS_SetErrorReporter(cx_, reportError);
#ifdef _DEBUG
	//JS_SetGCZeal(cx_, 2);
#endif
    /* Create the global object. */

	global_ = JS_NewCompartmentAndGlobalObject(cx_, &globalClass, NULL);
    if (global_ == NULL) throw Exception("JS_NewCompartmentAndGlobalObject");
	
	JS_SetPrivate(cx_, global_, this);	

	documentProto = 
		JS_InitClass(cx_, global_, 0, &documentClass, 0, 0, 0, DocumentFuncs, 0, 0);
	rootJSObject(documentProto);

	consoleProto =
		JS_InitClass(cx_, global_, 0, &jsConsoleClass, 0, 0, 0, ConsoleFuncs, 0, 0);
	rootJSObject(consoleProto);


	xmlHttpRequestProto_ = XMLHttpRequest::createScriptObjectProto(this, global_);
	matrixProto_ = Matrix::scriptObjectProto(this, global_);
	vector3Proto_ = Vector3::scriptObjectProto(this, global_);
	pathProto_ = Path::scriptObjectProto(this, global_);
	vector2Proto_ = Vector2::scriptObjectProto(this, global_);
	transformProto_ = Transform::scriptObjectProto(this);
	sceneProto_ = Scene::createScriptObjectProto(this);	
	baseSceneObjectProto_ = SceneObject::createBaseScriptObjectProto(this);	
	baseAppObjectProto_ = AppObject::createBaseScriptObjectProto(this);
	baseDocumentTemplateProto_ = DocumentTemplate::createBaseScriptObjectProto(this);
	baseEventProto_ = Event::createBaseScriptObjectProto(this);
	animationProto_ = Animation::createScriptObjectProto(this);
	pixelBufferProto_ = PixelBuffer::createScriptObjectProto(this, global_);
	kdTreeProto_ = KDTree::createScriptObjectProto(this, global_);

/*
	if (!JS_InitStandardClasses(cx_, global_))
		throw Exception("JS_InitStandardClasses");

	if (!JS_DefineFunctions(cx_, global_, GlobalFuncs))
        throw Exception("JS_DefineFunctions");

	documentInstance_ = 
		JS_DefineObject(cx_, global_, "document", &documentClass, documentProto, 0);
	JS_SetPrivate(cx_, documentInstance_, document_);

	consoleInstance_ = 
		JS_DefineObject(cx_, global_, "console", 0, consoleProto, 0);
*/
	lastUpdateTime_ = Global::currentTime();
}

bool ScriptProcessor::execute(JSObject * funcObj)
{
	JSBool ok = JS_FALSE;
	jsval rval;
	
	//if (JS_ObjectIsFunction(cx_, funcObj))
	{
		jsval funcVal = OBJECT_TO_JSVAL(funcObj);
		ok = JS_CallFunctionValue(cx_, global_, funcVal, 0, 0, &rval);
	}

	return ok;
}

bool ScriptProcessor::executeAndUnroot(JSObject * funcObj)
{
	JSBool ok = JS_FALSE;
	jsval rval;
	
	//if (JS_ObjectIsFunction(cx_, funcObj))
	{
		jsval funcVal = OBJECT_TO_JSVAL(funcObj);
		ok = 
			JS_CallFunctionValue(cx_, global_, funcVal, 0, 0, &rval);

		unrootJSObject(funcObj);

	}

	return ok;
}

bool ScriptProcessor::execute(const std::string & pscriptFile, JSObject * global)
{
	jsval rval;
	JSBool ok;

	FILE * file = 0;
	char * buffer = 0;
	BOOST_SCOPE_EXIT( (&file) (&buffer)) {
		if (file) fclose(file);
		if (buffer) free(buffer);
    } BOOST_SCOPE_EXIT_END


	string scriptFile = getActualFileName(pscriptFile);

	file = fopen(scriptFile.c_str(), "rb");
	if (!file) return false;
	fseek(file, 0, SEEK_END);	
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	buffer = (char *)malloc(fileSize);
	if (!buffer) return false;
	fread(buffer, 1, fileSize, file);

	std::vector<jschar> wText;
	utf8::utf8to16(buffer, buffer + fileSize, back_inserter(wText));

	if (!global) 
	{
		global = newGlobal();
		rootJSObject(global);
	}
	/*
	ok = JS_EvaluateUCScript(cx_, global, 
		&wText[0], wText.size(), scriptFile.c_str(), 1, &rval);	
	*/

	/*
	Convert file name to utf-8 (this isn't an accurate conversion)
	just to prevent "malformed utf-8" errors when filename is included
	in JS error reporting.
	TODO: maybe store filenames in propery encoded utf 8
	*/
	std::wstring wFileName(scriptFile.begin(), scriptFile.end());	
	std::string scriptFileUtf8;
	utf8::utf16to8(wFileName.begin(), wFileName.end(), back_inserter(scriptFileUtf8));

	execGlobals_.push(global);
	ok = JS_EvaluateScript(cx_, global, 
		buffer, fileSize, scriptFileUtf8.c_str(), 1, &rval);	
	execGlobals_.pop();
	return ok; 
}

bool ScriptProcessor::execute(
	const std::string & fileName, const std::string & script, JSObject * global)
{
	jsval rval;
	JSBool ok;

	if (!global) global = newGlobal();

	/*
	prevent malformed utf 8 (for non ANSI chars)
	*/
	std::wstring wFileName(fileName.begin(), fileName.end());	
	std::string scriptFileUtf8;
	utf8::utf16to8(wFileName.begin(), wFileName.end(), back_inserter(scriptFileUtf8));
	
	ok = JS_EvaluateScript(cx_, global, 
		&script[0], script.size(), scriptFileUtf8.c_str(), 1, &rval);	

	return ok; 
}

bool ScriptProcessor::executeScriptEvent(
	Event * event, const ScriptEventListener & listener)
{
	jsval rval;
	JSBool ok;

	JSObject * evtObject = event->createScriptObject(this);
	jsval funcVal = OBJECT_TO_JSVAL(listener.functionScriptObject());
	jsval evtVal = OBJECT_TO_JSVAL(evtObject);
	ok = 
		JS_CallFunctionValue(cx_, global_, funcVal, 1, &evtVal, &rval);

	return ok; 
}


unsigned int ScriptProcessor::setFuncTimeout(JSObject * functionObj, unsigned int interval)
{
	ExecScriptData e;
	e.id = curExecScriptId_++;
	e.repeatInterval = -1;
	e.execTime = Global::currentTime() + (float)interval/1000.0f;
	e.funcObj = functionObj;	
	execScriptData_.push_back(e);

	return e.id;
}

unsigned int ScriptProcessor::setFuncInterval(JSObject * functionObj, unsigned int interval)
{
	ExecScriptData e;
	e.id = curExecScriptId_++;
	e.repeatInterval = (float)interval/1000.0f;
	e.execTime = Global::currentTime() + e.repeatInterval;
	e.funcObj = functionObj;	
	execScriptData_.push_back(e);

	return e.id;
}

void ScriptProcessor::clearInterval(int id)
{
	vector<ExecScriptData>::iterator iter;
	for (iter = execScriptData_.begin(); iter != execScriptData_.end();)
	{
		ExecScriptData & e = *iter;
		if (e.id == id)
		{
			iter = execScriptData_.erase(iter);
			unrootJSObject(e.funcObj);
		}
		else 
			++iter;
	}
}

void ScriptProcessor::clearTimeout(int id)
{
	vector<ExecScriptData>::iterator iter;
	for (iter = execScriptData_.begin(); iter != execScriptData_.end();)
	{
		ExecScriptData & e = *iter;
		if (e.id == id && e.repeatInterval < 0)
		{
			iter = execScriptData_.erase(iter);
			unrootJSObject(e.funcObj);
		}
		else 
			++iter;
	}
}

void ScriptProcessor::update(float docTime)
{
	lastUpdateTime_ = docTime;
#ifdef _DEBUG
	//JS_GC(cx_);
#endif
	vector<ExecScriptData>::iterator iter;
	
	bool iterMaybeInvalid = false;

	for (iter = execScriptData_.begin(); iter != execScriptData_.end(); ++iter)
	{
		ExecScriptData & e = *iter;
		e.executed = false;
	}
	
	do 
	{
		iterMaybeInvalid = false;
		for (iter = execScriptData_.begin(); iter != execScriptData_.end(); ++iter)
		{
			ExecScriptData & e = *iter;
			if (!e.executed && e.execTime <= docTime)
			{					
				if (e.repeatInterval < 0)
				{	
					ExecScriptData eCopy = e;
					//erase ahead of time because e.funcObj may also cause the deletion of an interval func which would invalidate iter... etc.
					execScriptData_.erase(iter);						
					executeAndUnroot(eCopy.funcObj);
					eCopy.executed = true;					
					iterMaybeInvalid = true;
					break;							
				}
				else
				{
					execute(e.funcObj);
					e.executed = true;
					e.execTime += e.repeatInterval;
					iterMaybeInvalid = true;
					break;				
				}
			}
		}
	} while (iterMaybeInvalid);

	BOOST_FOREACH(XMLHttpRequest * x, xmlHttpRequests_)
	{
		x->update(docTime);
	}

}

JSObject * ScriptProcessor::appObjectProto(AppObject * appObject)
{
	map<boost::uuids::uuid, JSObject *>::iterator iter;
	iter = sceneObjectProtos_.find(appObject->type());

	if (iter == sceneObjectProtos_.end())
	{
		JSObject * proto = appObject->createScriptObjectProto(this);
		sceneObjectProtos_[appObject->type()] = proto;
		return proto;
	}
	else
	{
		return (*iter).second;
	}
}

JSObject * ScriptProcessor::sceneObjectProto(SceneObject * sceneObject)
{
	map<boost::uuids::uuid, JSObject *>::iterator iter;
	iter = sceneObjectProtos_.find(sceneObject->type());

	if (iter == sceneObjectProtos_.end())
	{
		JSObject * proto = sceneObject->createScriptObjectProto(this);
		sceneObjectProtos_[sceneObject->type()] = proto;
		return proto;
	}
	else
	{
		return (*iter).second;
	}
}

JSObject * ScriptProcessor::documentTemplateProto(DocumentTemplate * templ)
{
	map<boost::uuids::uuid, JSObject *>::iterator iter;
	iter = documentTemplateProtos_.find(templ->type());

	if (iter == documentTemplateProtos_.end())
	{
		JSObject * proto = templ->createScriptObjectProto(this);
		sceneObjectProtos_[templ->type()] = proto;
		return proto;
	}
	else
	{
		return (*iter).second;
	}
}


JSObject * ScriptProcessor::eventProto(Event * obj)
{
	map<boost::uuids::uuid, JSObject *>::iterator iter;
	iter = eventObjectProtos_.find(obj->type());

	if (iter == eventObjectProtos_.end())
	{
		JSObject * proto = obj->createScriptObjectProto(this);
		eventObjectProtos_[obj->type()] = proto;
		return proto;
	}
	else
	{
		return (*iter).second;
	}
}

void ScriptProcessor::registerEventListener(
	SceneObject * object, JSObject * funcObj)
{
}

void ScriptProcessor::rootJSObject(JSObject * origObject)
{
	rootedObjects_.rootJSObject(cx_, origObject);
}

void ScriptProcessor::unrootAllJSObjects()
{
	rootedObjects_.unrootAllJSObjects(cx_);
}

void ScriptProcessor::unrootJSObject(JSObject * rootedObject)
{
	rootedObjects_.unrootJSObject(cx_, rootedObject);
}

void ScriptProcessor::deletePrivateData(void * privateData)
{
	privateDataAllocs_.deletePrivateData(privateData);
}

void ScriptProcessor::addXmlHttpRequest(XMLHttpRequest * x)
{
	xmlHttpRequests_.insert(x);
}

void ScriptProcessor::removeXmlHttpRequest(XMLHttpRequest * x)
{
	xmlHttpRequests_.erase(x);
}

std::string ScriptProcessor::getActualFileName(const std::string & scriptSideFileName) const
{
	return getAbsFileName(
		Global::instance().readDirectory(),
		document_->getScriptAccessibleFile(scriptSideFileName));
}
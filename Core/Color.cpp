#include "stdafx.h"
#include "Color.h"
#include "ScriptProcessor.h"

JSClass jsColorClass = InitClass(
	"Color", 0, JS_PropertyStub, 
	JS_StrictPropertyStub, JS_FinalizeStub, 0);


Color Color::fromScriptObject(ScriptProcessor * s, JSObject * obj)
{
	Color ret;
	JSContext * cx = s->jsContext();

	if (!obj) return ret;

	float * vecVals[4] = {&ret.r, &ret.g, &ret.b, &ret.a};	

	if (JS_IsArrayObject(cx, obj))
	{
		jsuint jsLen;
		JS_GetArrayLength(cx, obj, &jsLen);		

		int len = 4;
		if (jsLen < len) len = jsLen;		
		for (int i = 0; i < len; ++i)
		{
			jsval jsCurVal;
			JS_GetElement(cx, obj, i, &jsCurVal);
			float val;
			if (!getFloat(cx, jsCurVal, &val)) continue;
			*vecVals[i] = val;
		}
	}
	else
	{
		getPropertyFloat(s->jsContext(), obj, "r", &ret.r);
		getPropertyFloat(s->jsContext(), obj, "g", &ret.g);
		getPropertyFloat(s->jsContext(), obj, "b", &ret.b);
		getPropertyFloat(s->jsContext(), obj, "a", &ret.a);
	}	

	return ret;
}

JSObject * Color::createScriptObject(ScriptProcessor * s) const
{
	JSContext * cx = s->jsContext();
	JSObject * scriptObject = JS_NewObject(
		cx, &jsColorClass, 0, s->jsGlobal());

	jsval val;
	JS_NewNumberValue(cx, r, &val);	
	JS_SetProperty(cx, scriptObject, "r", &val);
	JS_NewNumberValue(cx, g, &val);	
	JS_SetProperty(cx, scriptObject, "g", &val);
	JS_NewNumberValue(cx, b, &val);	
	JS_SetProperty(cx, scriptObject, "b", &val);
	JS_NewNumberValue(cx, a, &val);	
	JS_SetProperty(cx, scriptObject, "a", &val);
	
	return scriptObject;
}
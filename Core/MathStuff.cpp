#include "stdafx.h"
#include "MathStuff.h"
#include "ScriptProcessor.h"
#include <boost/random/uniform_real_distribution.hpp>
#include "math.h"
///////////////////////////////////////////////////////////////////////////////

void Vector3_finalize(JSContext * cx, JSObject * obj)
{		
	Vector3 * privateData = (Vector3 *)JS_GetPrivate(cx, obj);
	delete privateData;
}

JSClass jsVector3Class = InitClass(
	"Vector3", JSCLASS_HAS_PRIVATE, JS_PropertyStub, 
	JS_StrictPropertyStub, Vector3_finalize, 0);


//-----------------------------------------------------------------------------

JSBool Vector3_constructor(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsArray;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsArray)) return JS_FALSE;

	Vector3 * vec = new Vector3;
	vec->x = 0.0f; vec->y = 0.0f; vec->z = 0.0f;

	JSObject * newObject = JS_NewObject(cx, &jsVector3Class, s->vector3Proto(), 0);	
	JS_SetPrivate(cx, newObject, vec);


	float * vecVals[3] = {&vec->x, &vec->y, &vec->z};	
	if (JS_IsArrayObject(cx, jsArray))
	{
		jsuint jsLen;
		JS_GetArrayLength(cx, jsArray, &jsLen);		

		int len = 3;
		if (jsLen < len) len = jsLen;		
		for (int i = 0; i < len; ++i)
		{
			jsval jsCurVal;
			JS_GetElement(cx, jsArray, i, &jsCurVal);
			float val;
			if (!getFloat(cx, jsCurVal, &val)) continue;
			*vecVals[i] = val;
		}
	}
	else
	{
		*vec = Vector3::fromScriptObject(s, jsArray);
	}


	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(newObject));
	return JS_TRUE;
};


JSBool Vector3_dot(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	
	Vector3 * thisV = (Vector3 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * proto = JS_GetPrototype(cx, jsObj);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	Vector3 v = Vector3::fromScriptObject(s, jsObj);	
	float ret = (*thisV) * v;	
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL((jsdouble)ret));

	return JS_TRUE;
};

JSBool Vector3_cross(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	
	Vector3 * thisV = (Vector3 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * proto = JS_GetPrototype(cx, jsObj);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	Vector3 v = Vector3::fromScriptObject(s, jsObj);	
	Vector3 ret = (*thisV) ^ v;
	JSObject * retJs = ret.createScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retJs));

	return JS_TRUE;
};

JSBool Vector3_normalize(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	JSObject * thisJsObj = JS_THIS_OBJECT(cx, vp);
	Vector3 * thisV = (Vector3 *)JS_GetPrivate(cx, thisJsObj);
	*thisV = thisV->normalize();
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(thisJsObj));
	return JS_TRUE;
};

JSBool Vector3_add(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	
	Vector3 * thisV = (Vector3 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	Vector3 v = Vector3::fromScriptObject(s, jsObj);	
	Vector3 ret = (*thisV) + v;	
	JSObject * retJs = ret.createScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retJs));

	return JS_TRUE;
};

JSBool Vector3_subtract(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	
	Vector3 * thisV = (Vector3 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	Vector3 v = Vector3::fromScriptObject(s, jsObj);	
	Vector3 ret = (*thisV) - v;	
	JSObject * retJs = ret.createScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retJs));

	return JS_TRUE;
};

/**
Scalar multiplication
*/
JSBool Vector3_multiply(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	jsdouble val;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &val)) return JS_FALSE;	
	Vector3 * thisV = (Vector3 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	Vector3 ret = ((float)val) * (*thisV);	
	JSObject * retJs = ret.createScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retJs));

	return JS_TRUE;
}

JSBool Vector3_magnitude(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	Vector3 * thisV = (Vector3 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL((jsdouble)thisV->magnitude()));
	return JS_TRUE;
};

JSBool Vector3_distanceFrom(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	
	Vector3 * thisV = (Vector3 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	Vector3 v = Vector3::fromScriptObject(s, jsObj);	
	float ret = thisV->distanceFrom(v);		
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL((jsdouble)ret));
	return JS_TRUE;
};

JSFunctionSpec Vector3Funcs[] = {
	JS_FS("dot", Vector3_dot, 1, 0),
	JS_FS("distanceFrom", Vector3_distanceFrom, 1, 0),
	JS_FS("cross", Vector3_cross, 1, 0),
	JS_FS("normalize", Vector3_normalize, 0, 0),
	JS_FS("magnitude", Vector3_magnitude, 0, 0),
	JS_FS("add", Vector3_add, 1, 0),
	JS_FS("subtract", Vector3_subtract, 1, 0),
	JS_FS("multiply", Vector3_multiply, 1, 0),
    JS_FS_END
};


//-----------------------------------------------------------------------------


enum
{
	x, y, z
};


static JSBool Vector3_getProperty(
	JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{	
	Vector3 * thisObj = (Vector3 *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	*vp = JSVAL_VOID;

	switch (nID) {
	
	case x: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->x); break;
	case y: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->y); break;
	case z: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->z); break;	
	}
	return JS_TRUE;
}


JSBool Vector3_setProperty(
	JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp)
{	
	Vector3 * thisObj = (Vector3 *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);

	switch (nID) {
	case x: getFloat(cx, *vp, &thisObj->x); break;
	case y: getFloat(cx, *vp, &thisObj->y); break;
	case z: getFloat(cx, *vp, &thisObj->z); break;
	}

	return JS_TRUE;
}


JSPropertySpec Vector3Props[] = {
	
	{"x",	x, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Vector3_getProperty, Vector3_setProperty},

	{"y",	y, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Vector3_getProperty, Vector3_setProperty},

	{"z",	z, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Vector3_getProperty, Vector3_setProperty},	

	{0}
};

//-----------------------------------------------------------------------------


JSObject * Vector3::scriptObjectProto(ScriptProcessor * s, JSObject * global)
{
	JSObject * jsVector3Proto = JS_InitClass(
		s->jsContext(), global, 0, &jsVector3Class, Vector3_constructor, 1, 
		Vector3Props, Vector3Funcs, 
		0, 0);
	s->rootJSObject(jsVector3Proto);
	return jsVector3Proto;	
}

JSObject * Vector3::createScriptObject(ScriptProcessor * s) const
{	
	JSContext * cx = s->jsContext();
	JSObject * scriptObject = JS_NewObject(
		cx, &jsVector3Class, s->vector3Proto(), 0);
	Vector3 * retJs = new Vector3(*this);
	JS_SetPrivate(cx, scriptObject, retJs);
	return scriptObject;
}

Vector3 Vector3::fromScriptObject(ScriptProcessor * s, JSObject * jsVec)
{
	Vector3 ret;
	JSContext * cx = s->jsContext();
	jsval val;

	ret.x = 0.0f;
	ret.y = 0.0f;
	ret.z = 0.0f;

	if (!jsVec) return ret;

	if (JS_IsArrayObject(cx, jsVec))
	{
		jsuint jsLen;
		JS_GetArrayLength(cx, jsVec, &jsLen);		

		int len = 3;
		float * vecVals[3] = {&ret.x, &ret.y, &ret.z};
		if (jsLen < len) len = jsLen;
		for (int i = 0; i < len; ++i)
		{
			jsval jsCurVal;
			JS_GetElement(cx, jsVec, i, &jsCurVal);
			float val;
			if (!getFloat(cx, jsCurVal, &val)) continue;
			*vecVals[i] = val;
		}
	}
	else
	{
		JS_GetProperty(cx, jsVec, "x", &val);
		getFloat(cx, val, &ret.x);

		JS_GetProperty(cx, jsVec, "y", &val);
		getFloat(cx, val, &ret.y);

		JS_GetProperty(cx, jsVec, "z", &val);
		getFloat(cx, val, &ret.z);	
	}
	

	return ret;
}


//-----------------------------------------------------------------------------

void Vector3::projectOnToPlane(const Vector3 & vec1, const Vector3 & vec2)
{
	Vector3 planeNormal = vec1 ^ vec2;	
	planeNormal.makeNormal();

	float vecOnToPlaneNormalDist = *this * planeNormal;

	*this = *this - vecOnToPlaneNormalDist * planeNormal;
}

void Vector3::projectOnToPlane(const Plane & plane)
{
	float projDist = plane.dotCoord(*this);
    Vector3 n;
    
	n.x = plane.a*projDist;
    n.y = plane.b*projDist;
    n.z = plane.c*projDist;

    *this = *this - n;
}

void Vector3::makeOrthonormalTo(const Vector3 & vec2)
{
	Vector3 uVec2 = vec2;
	uVec2.makeNormal();

	float v1ProjToV2Dist = *this * uVec2;

	*this = *this - v1ProjToV2Dist*uVec2;
	makeNormal();	
}
////////////////////////////////////////////////////////////////////////////////
void Vector2_finalize(JSContext * cx, JSObject * obj)
{		
	Vector2 * privateData = (Vector2 *)JS_GetPrivate(cx, obj);
	delete privateData;
}


JSClass jsVector2Class = InitClass(
	"Vector2", JSCLASS_HAS_PRIVATE, JS_PropertyStub, 
	JS_StrictPropertyStub, Vector2_finalize, 0);


//-----------------------------------------------------------------------------

JSBool Vector2_constructor(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsArray;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsArray)) return JS_FALSE;

	Vector2 * vec = new Vector2;
	vec->x = 0.0f; vec->y = 0.0f;;

	JSObject * newObject = JS_NewObject(cx, &jsVector2Class, s->vector2Proto(), 0);	
	JS_SetPrivate(cx, newObject, vec);


	float * vecVals[2] = {&vec->x, &vec->y};	
	if (JS_IsArrayObject(cx, jsArray))
	{
		jsuint jsLen;
		JS_GetArrayLength(cx, jsArray, &jsLen);		

		int len = 2;
		if (jsLen < len) len = jsLen;		
		for (int i = 0; i < len; ++i)
		{
			jsval jsCurVal;
			JS_GetElement(cx, jsArray, i, &jsCurVal);
			float val;
			if (!getFloat(cx, jsCurVal, &val)) continue;
			*vecVals[i] = val;
		}
	}
	else
	{
		*vec = Vector2::fromScriptObject(s, jsArray);
	}


	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(newObject));
	return JS_TRUE;
};


JSBool Vector2_dot(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	
	Vector2 * thisV = (Vector2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * proto = JS_GetPrototype(cx, jsObj);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	Vector2 v = Vector2::fromScriptObject(s, jsObj);	
	float ret = (*thisV) * v;	
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL((jsdouble)ret));

	return JS_TRUE;
};

JSBool Vector2_normalize(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	JSObject * thisJsObj = JS_THIS_OBJECT(cx, vp);
	Vector2 * thisV = (Vector2 *)JS_GetPrivate(cx, thisJsObj);
	*thisV = thisV->normalize();
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(thisJsObj));
	return JS_TRUE;
};

JSBool Vector2_add(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	
	Vector2 * thisV = (Vector2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	Vector2 v = Vector2::fromScriptObject(s, jsObj);	
	Vector2 ret = (*thisV) + v;	
	JSObject * retJs = ret.createScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retJs));

	return JS_TRUE;
};

JSBool Vector2_subtract(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	
	Vector2 * thisV = (Vector2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	Vector2 v = Vector2::fromScriptObject(s, jsObj);	
	Vector2 ret = (*thisV) - v;	
	JSObject * retJs = ret.createScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retJs));

	return JS_TRUE;
};

JSBool Vector2_magnitude(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	Vector2 * thisV = (Vector2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL((jsdouble)thisV->magnitude()));
	return JS_TRUE;
};


/**
Scalar multiplication
*/
JSBool Vector2_multiply(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	jsdouble val;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &val)) return JS_FALSE;	
	Vector2 * thisV = (Vector2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	Vector2 ret = ((float)val) * (*thisV);	
	JSObject * retJs = ret.createScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retJs));

	return JS_TRUE;
}

JSFunctionSpec Vector2Funcs[] = {
	JS_FS("dot", Vector2_dot, 1, 0),	
	JS_FS("normalize", Vector2_normalize, 0, 0),
	JS_FS("add", Vector2_add, 1, 0),
	JS_FS("subtract", Vector2_subtract, 1, 0),
	JS_FS("multiply", Vector2_multiply, 1, 0),
	JS_FS("magnitude", Vector2_magnitude, 0, 0),
    JS_FS_END
};


//-----------------------------------------------------------------------------


enum
{
	Vector2_x, Vector2_y
};


static JSBool Vector2_getProperty(
	JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{	
	Vector2 * thisObj = (Vector2 *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	*vp = JSVAL_VOID;

	switch (nID) {
	
	case Vector2_x: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->x); break;
	case Vector2_y: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->y); break;
	}
	return JS_TRUE;
}


JSBool Vector2_setProperty(
	JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp)
{	
	Vector2 * thisObj = (Vector2 *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);

	switch (nID) {
	case Vector2_x: getFloat(cx, *vp, &thisObj->x); break;
	case Vector2_y: getFloat(cx, *vp, &thisObj->y); break;
	}

	return JS_TRUE;
}


JSPropertySpec Vector2Props[] = {
	
	{"x",	Vector2_x, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Vector2_getProperty, Vector2_setProperty},

	{"y",	Vector2_y, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Vector2_getProperty, Vector2_setProperty},

	{0}
};

//-----------------------------------------------------------------------------


JSObject * Vector2::scriptObjectProto(ScriptProcessor * s, JSObject * global)
{
	JSObject * jsVector2Proto = JS_InitClass(
		s->jsContext(), global, 0, &jsVector2Class, Vector2_constructor, 1, 
		Vector2Props, Vector2Funcs, 
		0, 0);
	s->rootJSObject(jsVector2Proto);
	return jsVector2Proto;	
}

JSObject * Vector2::createScriptObject(ScriptProcessor * s) const
{	
	JSContext * cx = s->jsContext();
	JSObject * scriptObject = JS_NewObject(
		cx, &jsVector2Class, s->vector2Proto(), 0);
	Vector2 * retJs = new Vector2(*this);
	JS_SetPrivate(cx, scriptObject, retJs);
	return scriptObject;
}

Vector2 Vector2::fromScriptObject(ScriptProcessor * s, JSObject * jsVec)
{
	Vector2 ret;
	JSContext * cx = s->jsContext();
	jsval val;

	ret.x = 0.0f;
	ret.y = 0.0f;	

	if (!jsVec) return ret;
	
	if (JS_IsArrayObject(cx, jsVec))
	{
		jsuint jsLen;
		JS_GetArrayLength(cx, jsVec, &jsLen);		

		int len = 2;
		float * vecVals[2] = {&ret.x, &ret.y};
		if (jsLen < len) len = jsLen;
		for (int i = 0; i < len; ++i)
		{
			jsval jsCurVal;
			JS_GetElement(cx, jsVec, i, &jsCurVal);
			float val;
			if (!getFloat(cx, jsCurVal, &val)) continue;
			*vecVals[i] = val;
		}
	}
	else
	{
		JS_GetProperty(cx, jsVec, "x", &val);
		getFloat(cx, val, &ret.x);

		JS_GetProperty(cx, jsVec, "y", &val);
		getFloat(cx, val, &ret.y);	
	}
	

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
void Matrix_finalize(JSContext * cx, JSObject * obj)
{		
	Matrix * privateData = (Matrix *)JS_GetPrivate(cx, obj);
	delete privateData;
}


JSClass jsMatrixClass = InitClass(
	"Matrix", JSCLASS_HAS_PRIVATE, JS_PropertyStub, 
	JS_StrictPropertyStub, Matrix_finalize, 0);


JSBool Matrix_constructor(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsArray;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsArray)) return JS_FALSE;

	Matrix * matrix = new Matrix;

	JSObject * newObject = JS_NewObject(cx, &jsMatrixClass, s->matrixProto(), 0);	
	JS_SetPrivate(cx, newObject, matrix);

	if (JS_IsArrayObject(cx, jsArray))
	{
		jsuint jsLen;
		JS_GetArrayLength(cx, jsArray, &jsLen);		

		int len = 16;
		if (jsLen < len) len = jsLen;
		for (int i = 0; i < len; ++i)
		{
			jsval jsCurVal;
			JS_GetElement(cx, jsArray, i, &jsCurVal);
			float val;
			if (!getFloat(cx, jsCurVal, &val)) continue;
			matrix->vals()[i] = val;
		}
	}


	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(newObject));
	return JS_TRUE;
};

JSBool Matrix_add(JSContext *cx, uintN argc, jsval *vp)
{
	return JS_TRUE;
};

JSBool Matrix_multiply(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	
	Matrix * thisM = (Matrix *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * proto = JS_GetPrototype(cx, jsObj);

	if (proto == s->matrixProto())
	{
		Matrix * m = (Matrix *)JS_GetPrivate(cx, jsObj);
		Matrix ret = *thisM * (*m);
		JSObject * retJs = ret.createScriptObject(s);
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retJs));
	}
	else if (proto == s->vector3Proto())
	{
		//matrix * vector
		Vector3 * v = (Vector3 *)JS_GetPrivate(cx, jsObj);
		Vector3 ret = *thisM * (*v);
		JSObject * retJs = ret.createScriptObject(s);
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retJs));
	}
	else
	{
		//TODO support arrays, etc...
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
	}

	return JS_TRUE;
};

JSBool Matrix_transform(JSContext *cx, uintN argc, jsval *vp)
{	
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);	
	jsval * argv = JS_ARGV(cx, vp);		
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	
	Matrix * thisM = (Matrix *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * proto = JS_GetPrototype(cx, jsObj);	

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	Vector3 v = Vector3::fromScriptObject(s, jsObj);	
	Vector3 ret = *thisM * v;
	JSObject * retJs = ret.createScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retJs));

	return JS_TRUE;
};


JSFunctionSpec MatrixFuncs[] = {
	JS_FS("add", Matrix_add, 1, 0),	
	JS_FS("multiply", Matrix_multiply, 1, 0),
	JS_FS("transform", Matrix_transform, 1, 0),
    JS_FS_END
};


enum
{
	_11, _21, _31, _41, 
	_12, _22, _32, _42, 
	_13, _23, _33, _43, 
	_14, _24, _34, _44
};


static JSBool Matrix_getProperty(
	JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{	
	Matrix * thisObj = (Matrix *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	*vp = JSVAL_VOID;

	switch (nID) {
	
	case _11: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_11); break;
	case _21: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_21); break;
	case _31: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_31); break;
	case _41: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_41); break;

	case _12: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_12); break;
	case _22: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_22); break;
	case _32: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_32); break;
	case _42: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_42); break;

	case _13: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_13); break;
	case _23: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_23); break;
	case _33: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_33); break;
	case _43: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_43); break;

	case _14: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_14); break;
	case _24: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_24); break;
	case _34: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_34); break;
	case _44: *vp = DOUBLE_TO_JSVAL((jsdouble)thisObj->_44); break;		
	}
	return JS_TRUE;
}


JSBool Matrix_setProperty(
	JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp)
{	
	Matrix * thisObj = (Matrix *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);

	switch (nID) {
	case _11: getFloat(cx, *vp, &thisObj->_11); break;
	case _21: getFloat(cx, *vp, &thisObj->_21); break;
	case _31: getFloat(cx, *vp, &thisObj->_31); break;
	case _41: getFloat(cx, *vp, &thisObj->_41); break;

	case _12: getFloat(cx, *vp, &thisObj->_12); break;
	case _22: getFloat(cx, *vp, &thisObj->_22); break;
	case _32: getFloat(cx, *vp, &thisObj->_32); break;
	case _42: getFloat(cx, *vp, &thisObj->_42); break;

	case _13: getFloat(cx, *vp, &thisObj->_13); break;
	case _23: getFloat(cx, *vp, &thisObj->_23); break;
	case _33: getFloat(cx, *vp, &thisObj->_33); break;
	case _43: getFloat(cx, *vp, &thisObj->_43); break;

	case _14: getFloat(cx, *vp, &thisObj->_14); break;
	case _24: getFloat(cx, *vp, &thisObj->_24); break;
	case _34: getFloat(cx, *vp, &thisObj->_34); break;
	case _44: getFloat(cx, *vp, &thisObj->_44); break;
	}

	return JS_TRUE;
}


JSPropertySpec MatrixProps[] = {
	
	{"_11",	_11, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_21",	_21, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_31",	_31, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_41",	_41, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_12",	_12, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_22",	_22, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_32",	_32, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_42",	_42, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_13",	_13, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_23",	_23, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_33",	_33, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_43",	_43, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_14",	_14, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_24",	_24, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_34",	_34, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{"_44",	_44, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Matrix_getProperty, Matrix_setProperty},

	{0}
};


JSObject * Matrix::scriptObjectProto(ScriptProcessor * s, JSObject * global)
{
	JSObject * jsMatrixProto = JS_InitClass(
		s->jsContext(), global, 0, &jsMatrixClass, Matrix_constructor, 1, 
		MatrixProps, MatrixFuncs, 
		0, 0);
	s->rootJSObject(jsMatrixProto);
	return jsMatrixProto;	
}

JSObject * Matrix::createScriptObject(ScriptProcessor * s) const
{	
	JSContext * cx = s->jsContext();
	JSObject * scriptObject = JS_NewObject(
		cx, &jsMatrixClass, s->matrixProto(), 0);

	Matrix * newMatrix = new Matrix(*this);
	JS_SetPrivate(cx, scriptObject, newMatrix);
		
	return scriptObject;
}

////////////////////////////////////////////////////////////////////////////////

Matrix::Matrix(const Matrix & rhs)
{
	memcpy(&_11, rhs.vals(), sizeof(float)*16);
}

Matrix Matrix::Identity()
{
	Matrix ret;
	float * v = &ret._11;
	for (int i = 0; i < 16; ++i) v[i] = 0;
	ret._11 = ret._22 = ret._33 = ret._44 = 1;
	return ret;
}

Matrix Matrix::Perspective(float fovY, float aspect, float zNear, float zFar)
{
	Matrix ret;
	float f = tan(M_PI_2 - (fovY/2) * M_PI/180);
	float * v = &ret._11;
	for (int i = 0; i < 16; ++i) v[i] = 0;

	ret._11 = f/aspect;
	ret._22 = f;
	ret._33 = (zFar + zNear) / (zNear - zFar);
	ret._43 = -1;
	ret._34 = (2 * zFar * zNear) / (zNear - zFar);

	return ret;
}

Matrix Matrix::Scale(float sx, float sy, float sz)
{
	Matrix ret = Matrix::Identity();
	ret._11 = (float)sx;
	ret._22 = (float)sy;
	ret._33 = (float)sz;	
	return ret;
}

Matrix Matrix::Scale(const Vector3 & pivot, float sx, float sy, float sz)
{
	Matrix ret = 
		Matrix::Translate(pivot.x, pivot.y, pivot.z) *
		Matrix::Scale(sx, sy, sz) *
		Matrix::Translate(-pivot.x, -pivot.y, -pivot.z);
	return ret;
}


Matrix Matrix::Translate(float tx, float ty, float tz)
{
	Matrix ret = Matrix::Identity();
	ret._14 = tx;
	ret._24 = ty;
	ret._34 = tz;
	return ret;
}

float Matrix::Determinant() const
{
	return 
		_14 * _23 * _32 * _41 - _13 * _24 * _32 * _41-
		_14 * _22 * _33 * _41 + _12 * _24 * _33 * _41+
		_13 * _22 * _34 * _41 - _12 * _23 * _34 * _41-
		_14 * _23 * _31 * _42 + _13 * _24 * _31 * _42+
		_14 * _21 * _33 * _42 - _11 * _24 * _33 * _42-
		_13 * _21 * _34 * _42 + _11 * _23 * _34 * _42+
		_14 * _22 * _31 * _43 - _12 * _24 * _31 * _43-
		_14 * _21 * _32 * _43 + _11 * _24 * _32 * _43+
		_12 * _21 * _34 * _43 - _11 * _22 * _34 * _43-
		_13 * _22 * _31 * _44 + _12 * _23 * _31 * _44+
		_13 * _21 * _32 * _44 - _11 * _23 * _32 * _44-
		_12 * _21 * _33 * _44 + _11 * _22 * _33 * _44;
}

Matrix Matrix::Rotate(float angle, float x, float y, float z)
{
	float c = cos(angle);
	float s = sin(angle);

	float mag = sqrt(x*x + y*y + z*z);
	x /= mag; y /= mag; z /= mag;

	Matrix ret;
	ret._11 = x*x*(1-c)+c;
	ret._21 = y*x*(1-c)+z*s;
	ret._31 = x*z*(1-c)-y*s;
	ret._41 = 0;

	ret._12 = x*y*(1-c)-z*s;
	ret._22 = y*y*(1-c)+c;
	ret._32 = y*z*(1-c)+x*s;
	ret._42 = 0;

	ret._13 = x*z*(1-c)+y*s;
	ret._23 = y*z*(1-c)-x*s;
	ret._33 = z*z*(1-c)+c;
	ret._43 = 0;

	ret._14 = 0;
	ret._24 = 0;
	ret._34 = 0;
	ret._44 = 1;

	return ret;
}



Matrix Matrix::Rotate(const Vector3 & v1, const Vector3 & v2)
{
	static const float EPSILON = 0.00001f;
	Vector3 u1 = v1.normalize(), u2 = v2.normalize();
	float dot = u1 * u2;
	if (dot > 1) dot = 1;
	if (dot < -1) dot = -1;

	float angle = acos(dot);
	if (angle < EPSILON) return Identity();

	Vector3 axis = u1 ^ u2;
	return Rotate(angle, axis.x, axis.y, axis.z);
}


Matrix Matrix::LookAt(
	float eyeX, float eyeY, float eyeZ, 
	float centerX, float centerY, float centerZ, 
	float upX, float upY, float upZ)
{
	Matrix ret;
	Vector3 F(centerX - eyeX, centerY - eyeY, centerZ - eyeZ);
	Vector3 Up((float)upX, (float)upY, (float)upZ);
	
	F.makeNormal();
	Up.makeNormal();

	Vector3 s = F ^ Up;
	Vector3 u = s ^ F;

	float * v = &ret._11;
	for (int i = 0; i < 16; ++i) v[i] = 0;

	ret._11 = s.x;
	ret._21 = u.x;
	ret._31 = -F.x;

	ret._12 = s.y;
	ret._22 = u.y;
	ret._32 = -F.y;

	ret._13 = s.z;
	ret._23 = u.z;
	ret._33 = -F.z;

	ret._44 = 1;

	return ret * Matrix::Translate(-eyeX, -eyeY, -eyeZ);
	
}

Matrix Matrix::pickMatrix(
	float x, float y, float width, float height, const int viewport[4])
{
	Matrix M;
	float * m = M.vals();
	float sx, sy;
	float tx, ty;
	 
	sx = viewport[2] / width;
	sy = viewport[3] / height;
	tx = (viewport[2] + 2.0 * (viewport[0] - x)) / width;
	ty = (viewport[3] + 2.0 * (viewport[1] - y)) / height;
	 
	#define M(row,col) m[col*4+row]
	M(0,0) = sx; M(0,1) = 0.0; M(0,2) = 0.0; M(0,3) = tx;
	M(1,0) = 0.0; M(1,1) = sy; M(1,2) = 0.0; M(1,3) = ty;
	M(2,0) = 0.0; M(2,1) = 0.0; M(2,2) = 1.0; M(2,3) = 0.0;
	M(3,0) = 0.0; M(3,1) = 0.0; M(3,2) = 0.0; M(3,3) = 1.0;
	#undef M
 
	return M;
}

Matrix Matrix::normalTransformMatrix() const
{
	Matrix ret = *this;
	ret._14 = 0;
	ret._24 = 0;
	ret._34 = 0;
	ret =  ret.transpose().inverse();	
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
Plane Plane::fromPointNormal(const Vector3 & pt, const Vector3 & normal)
{
	Plane plane;
	Vector3 unitNormal = normal.normalize();
	plane.d = -(unitNormal*pt);
	plane.a = unitNormal.x;
	plane.b = unitNormal.y;
	plane.c = unitNormal.z;
	return plane;
}

Plane Plane::fromPoints(const Vector3 & pt1, const Vector3 & pt2, const Vector3 &pt3)
{
	Vector3 normal = (pt2 - pt1) ^ (pt3 - pt1);
	return fromPointNormal(pt1, normal);
}

bool Plane::intersect(
	Vector3 * out, const Ray & ray) const
{
	float t;
	return intersect(out, &t, ray);
}

bool Plane::intersectLine(Vector3 * out, float * tOut, const Vector3 & pointA, const Vector3 & pointB) const
{
	static const float BIAS = 0.00001f; 
	float t;	
	Vector3 dir = pointB - pointA;
	const Vector3 & origin = pointA;

	float h = -(a*dir.x + b*dir.y + c*dir.z);

	if(fabs(h) < BIAS)
	{		
		return false;
	}

	t = (d + a*origin.x + b*origin.y + c*origin.z)/h;

	*tOut = t;
	out->x = origin.x + t*dir.x;
	out->y = origin.y + t*dir.y;
	out->z = origin.z + t*dir.z;
	return true;
}

bool Plane::intersectLine(Vector3 * out, const Vector3 & pointA, const Vector3 & pointB) const
{
	float t;
	return intersectLine(out, &t, pointA, pointB);	
}

bool Plane::intersectSegment(Vector3 * out, float * tOut, const Vector3 & pointA, const Vector3 & pointB) const
{
	static const float BIAS = 0.00001f; 
	float t;	
	Vector3 dir = pointB - pointA;
	const Vector3 & origin = pointA;

	float h = -(a*dir.x + b*dir.y + c*dir.z);

	if(fabs(h) < BIAS)
	{		
		return false;
	}

	t = (d + a*origin.x + b*origin.y + c*origin.z)/h;

	if(t <= -BIAS || t >= 1 + BIAS)
	{
		return false;
	}

	out->x = origin.x + t*dir.x;
	out->y = origin.y + t*dir.y;
	out->z = origin.z + t*dir.z;
	*tOut = t;
	return true;
}

bool Plane::intersect(
	Vector3 * out, float * t, const Ray & ray) const
{
	static const float BIAS = 0.00001f; 

	float h = -(a*ray.dir.x + b*ray.dir.y + c*ray.dir.z);

	if(fabs(h) < BIAS)
	{		
		return false;
	}

	*t = (d + a*ray.origin.x + b*ray.origin.y + c*ray.origin.z)/h;

	if(*t <= -BIAS)
	{
		return false;
	}


	out->x = ray.origin.x + *t*ray.dir.x;
	out->y = ray.origin.y + *t*ray.dir.y;
	out->z = ray.origin.z + *t*ray.dir.z;
	return true;
}
////////////////////////////////////////////////////////////////////////////////

bool Sphere::intersectLine(Vector3 * intPt1, Vector3 * intPt2,
	const Vector3 & rA, const Vector3 & rB) const
{
	float A, B, C, D, t;

    Vector3 temp2;

	Vector3 dir = rB - rA;

    A = dir * dir;

    temp2 = rA - center_;

    B = 2.0f*(dir * temp2);

    C = (temp2 * temp2) - radius_ * radius_;

    //quadratic formula using A, B, C

    D = B*B - 4*A*C;

    if (D<0)
    {
        return false;
    }

    D = sqrt(D);

    t= (-B + D)/(2.0f*A);	

    *intPt2 = rA+t*dir;

    t= (-B - D)/(2.0f*A);

    *intPt1 = rA+t*dir;

    return true;
}

////////////////////////////////////////////////////////////////////////////////

float distancePointSegment(
	Vector2 * pOut, const Vector2 & p, 
	const Vector2 & a, const Vector2 & b)
{
	Vector2 s = b - a;
	float len = s.magnitude();
	s/=len;

	Vector2 v = p - a;

	float projDist = s*v;

	if (projDist > 0 && projDist < len)
	{
		s*=projDist;
		v -= s;
		len = v.magnitude();

		*pOut = a + s;        
		return len;
	}
	else
	{
		float d1, d2;

		v = p - a;
		d1 = v.magnitude();

		v = p - b;        
		d2 = v.magnitude();

		if (d1 < d2)
		{
			*pOut = a;
			return d1;
		}
		else
		{
			*pOut = b;
			return d2;
		}
	}
}


float distancePointSegment(
	Vector3 * pOut, const Vector3 & p, 
	const Vector3 & a, const Vector3 & b)
{
	Vector3 s = b - a;
	float len = s.magnitude();
	s/=len;

	Vector3 v = p - a;

	float projDist = s*v;

	if (projDist > 0 && projDist < len)
	{
		s*=projDist;
		v -= s;
		len = v.magnitude();

		*pOut = a + s;        
		return len;
	}
	else
	{
		float d1, d2;

		v = p - a;
		d1 = v.magnitude();

		v = p - b;        
		d2 = v.magnitude();

		if (d1 < d2)
		{
			*pOut = a;
			return d1;
		}
		else
		{
			*pOut = b;
			return d2;
		}
	}
}

Vector3 makeNormalTo(const Vector3 & v1, const Vector3 &v2)
{
    Vector3 t1, t2;
    float dot;
	t2 = v2; t2.makeNormal();
    dot = v1 * t2;

    t2*=dot;

	t1 = v1 - t2;    
    Vector3 pOut = t1;
	pOut.makeNormal();    
    return pOut;
}

/*///////////////////////////////////////////////////////////////////////////////
Vector3 Spline::BezierSpline(std::vector<Vector3>& points, GLfloat u)
{	
	Vector3 point;	
    unsigned int n= points.size();
    std::vector<int> factorials(n);
    // For efficiency, I put all factorials in an array instead
    // of recalculating them each time.    
		GLfloat c1= pow((double)(1.0-u),(double)(n-2)); 
    // This is (1-t)^(n-1)
		//GLfloat c2= 1.0;
    // This is t^i , I'll progressively multiply it for u.
    factorials[0]=1;
    for(unsigned int i=1; i<n;i++)
    {
        factorials[i]= factorials[i-1]*i;
    }
    point.x = points[0].x * (GLfloat)(n-1) * c1; // This is the first point
	point.y = points[0].y * (GLfloat)(n-1) * c1; // This is the first point
	point.z = points[0].z * (GLfloat)(n-1) * c1; // This is the first point
    for(unsigned int i=1; i<n; i++)
    {
		GLfloat c1= pow((double)(1.0-u),(double)(n-i));
		GLfloat c2= pow((double)u, (double)i);

        //c2*= u;
        GLfloat bCoeff= factorials[n-1]/(factorials[i]*factorials[n-i]);
            // bCoeff is the binomial coefficient: n! / ( i! * (n-1)! )
        point.x += points[i].x * bCoeff * c1 * c2;
		point.y += points[i].y * bCoeff * c1 * c2;
		point.z += points[i].z * bCoeff * c1 * c2;
    }
    return point;
}
/////////////////////////////////////////////////////////////////*/

enum Axis
{
	X,
	Y,
	Z
};

inline void removeDominantAxis(Vector2 * pOut, const Vector3 & in, Axis axis)
{
	if(axis == X)
	{
		pOut->x = in.y;
		pOut->y = in.z;
	}
	else if(axis == Y)
	{
		pOut->x = in.x;
		pOut->y = in.z;
	}
	else
	{
		pOut->x = in.x;
		pOut->y = in.y;
	}
}

//returns which of a or b has the greatest absolute value
template <typename T>
T maxABS(T a, T b) {return (fabs(a) > fabs(b)) ? a : b;}

bool triangleIntersectRay(
	Vector3 * pOut, 
	const Vector3 & v0, const Vector3 & v1, const Vector3 & v2, 
	const Ray & ray)
{
	float t;
	return triangleIntersectRay(pOut, &t, v0, v1, v2, ray);
}


bool triangleIntersectRay(
	Vector3 * pOut, float * t, 
	const Vector3 & v0, const Vector3 & v1, const Vector3 & v2, 
	const Ray & ray)
{
	Plane plane = Plane::fromPoints(v0, v1, v2);

	//check to see if ray intersects the triangle's plane
	if(!plane.intersect(pOut, t, ray))
		return false;

	float max = maxABS(plane.a, maxABS(plane.b, plane.c));

	Axis dominantAxis;
	if(max==plane.a)
		dominantAxis = X;
	else if(max==plane.b)
		dominantAxis = Y;
	else
		dominantAxis = Z;
	
	Vector2 A, B, C, b, c;
	removeDominantAxis(&A, v0, dominantAxis);
	removeDominantAxis(&B, v1, dominantAxis);
	removeDominantAxis(&C, v2, dominantAxis);

	b = B - A;
	c = C - A;

	float fu = 1.0f/(b.y*c.x - b.x*c.y);
	float fv = 1.0f/(c.y*b.x - c.x*b.y);	

	Vector2 pt; 

	removeDominantAxis(&pt, *pOut, dominantAxis);
	pt.x -= A.x;
	pt.y -= A.y;

	B.x -= A.x; B.y -= A.y;
	C.x -= A.x; C.y -= A.y;

	static const float BIAS = 0.00001f;

	float u = fu * (pt.y*C.x - pt.x*C.y);	
	if (u < -BIAS)
		return false;
	
	float v = fv * (pt.y*B.x - pt.x*B.y);
	if (v < -BIAS)
		return false;

	if(u + v > 1 + BIAS)
		return false;

	return true;
}


bool triangleIntersectSegment(
	Vector3 * pOut, float * t, 
	const Vector3 & v0, const Vector3 & v1, const Vector3 & v2, 
	const Vector3 & p0, const Vector3 & p1)
{
	Plane plane = Plane::fromPoints(v0, v1, v2);

	//check to see if ray intersects the triangle's plane
	if(!plane.intersectSegment(pOut, t, p0, p1))
		return false;

	float max = maxABS(plane.a, maxABS(plane.b, plane.c));

	Axis dominantAxis;
	if(max==plane.a)
		dominantAxis = X;
	else if(max==plane.b)
		dominantAxis = Y;
	else
		dominantAxis = Z;
	
	Vector2 A, B, C, b, c;
	removeDominantAxis(&A, v0, dominantAxis);
	removeDominantAxis(&B, v1, dominantAxis);
	removeDominantAxis(&C, v2, dominantAxis);

	b = B - A;
	c = C - A;

	float fu = 1.0f/(b.y*c.x - b.x*c.y);
	float fv = 1.0f/(c.y*b.x - c.x*b.y);	

	Vector2 pt; 

	removeDominantAxis(&pt, *pOut, dominantAxis);
	pt.x -= A.x;
	pt.y -= A.y;

	B.x -= A.x; B.y -= A.y;
	C.x -= A.x; C.y -= A.y;

	static const float BIAS = 0.00001f;

	float u = fu * (pt.y*C.x - pt.x*C.y);	
	if (u < -BIAS)
		return false;
	
	float v = fv * (pt.y*B.x - pt.x*B.y);
	if (v < -BIAS)
		return false;

	if(u + v > 1 + BIAS)
		return false;

	return true;
}

bool triangleIntersectLine(
	Vector3 * pOut, float * t, 
	const Vector3 & v0, const Vector3 & v1, const Vector3 & v2, 
	const Vector3 & p0, const Vector3 & p1)
{
	Plane plane = Plane::fromPoints(v0, v1, v2);
	if(!plane.intersectLine(pOut, t, p0, p1))
		return false;

	float max = maxABS(plane.a, maxABS(plane.b, plane.c));

	Axis dominantAxis;
	if(max==plane.a)
		dominantAxis = X;
	else if(max==plane.b)
		dominantAxis = Y;
	else
		dominantAxis = Z;

	Vector2 A, B, C, b, c;
	removeDominantAxis(&A, v0, dominantAxis);
	removeDominantAxis(&B, v1, dominantAxis);
	removeDominantAxis(&C, v2, dominantAxis);

	b = B - A;
	c = C - A;

	float fu = 1.0f/(b.y*c.x - b.x*c.y);
	float fv = 1.0f/(c.y*b.x - c.x*b.y);	

	Vector2 pt; 

	removeDominantAxis(&pt, *pOut, dominantAxis);
	pt.x -= A.x;
	pt.y -= A.y;

	B.x -= A.x; B.y -= A.y;
	C.x -= A.x; C.y -= A.y;

	static const float BIAS = 0.00001f;

	float u = fu * (pt.y*C.x - pt.x*C.y);	
	if (u < -BIAS)
		return false;

	float v = fv * (pt.y*B.x - pt.x*B.y);
	if (v < -BIAS)
		return false;

	if(u + v > 1 + BIAS)
		return false;

	return true;
}


float roundFloat(float r)
{
	return (r > 0.0f) ? floor(r + 0.5f) : ceil(r - 0.5f);
}



float randf()
{
	static boost::random::mt19937 gen(time(0));	
	static boost::random::uniform_real_distribution<float> dist;
	return dist(gen);	
}

float triangleArea(const Vector3 & p0, const Vector3 & p1, const Vector3 & p2)
{
	float a = p0.distanceFrom(p1);
	float b = p1.distanceFrom(p2);
	float c = p2.distanceFrom(p0);
	float s = 0.5f * (a + b + c);
	return sqrt(s * (s - a) * (s - b) * (s - c));
}

Vector3 triangleRandomPoint(const Vector3 & p0, const Vector3 & p1, const Vector3 & p2)
{
	//TODO: Is this really a uniform distribution?
	float b0 = randf();
	float b1 = (1.0f - b0) * randf();
	float b2 = 1 - b0 - b1;
	return b0 * p0 + b1 * p1 + b2 * p2;
}



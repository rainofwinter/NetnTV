#include "stdafx.h"
#include "Transform.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ScriptProcessor.h"


//-----------------------------------------------------------------------------

JSBool Transform_getTranslation(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * jsThisObj = JS_THIS_OBJECT(cx, vp);

	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Transform * transformObj = (Transform *)JS_GetPrivate(cx, jsThisObj);
	if (!JS_ConvertArguments(cx, argc, argv, ""))
        return JS_FALSE;

	Vector3 translation = transformObj->translation();
	JSObject * translationObject = translation.createScriptObject(s);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(translationObject));	
	return JS_TRUE;
};


JSBool Transform_setTranslation(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * jsThisObj = JS_THIS_OBJECT(cx, vp);

	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Transform * thisObj = (Transform *)JS_GetPrivate(cx, jsThisObj);

	JSObject * jsTranslation;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsTranslation))
		return JS_FALSE;

	if (!jsTranslation)
	{
		JS_ReportError(cx, "parameter not a valid object");
		return JS_FALSE;
	}

	Vector3 vector3;
	getPropertyFloat(cx, jsTranslation, "x", &vector3.x);
	getPropertyFloat(cx, jsTranslation, "y", &vector3.y);
	getPropertyFloat(cx, jsTranslation, "z", &vector3.z);
	

	thisObj->setTranslation(vector3);
	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);	
	return JS_TRUE;
};

JSBool Transform_getRotation(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * jsThisObj = JS_THIS_OBJECT(cx, vp);

	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Transform * transformObj = (Transform *)JS_GetPrivate(cx, jsThisObj);
	if (!JS_ConvertArguments(cx, argc, argv, ""))
        return JS_FALSE;

	Vector3 vec = transformObj->rotation();
	JSObject * jsVec = vec.createScriptObject(s);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsVec));	
	return JS_TRUE;
};


JSBool Transform_setRotation(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * jsThisObj = JS_THIS_OBJECT(cx, vp);

	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Transform * thisObj = (Transform *)JS_GetPrivate(cx, jsThisObj);

	JSObject * jsVec;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsVec))
		return JS_FALSE;

	if (!jsVec)
	{
		JS_ReportError(cx, "parameter not a valid object");
		return JS_FALSE;
	}

	Vector3 vector3;
	getPropertyFloat(cx, jsVec, "x", &vector3.x);
	getPropertyFloat(cx, jsVec, "y", &vector3.y);
	getPropertyFloat(cx, jsVec, "z", &vector3.z);

	thisObj->setRotation(vector3.x, vector3.y, vector3.z);
	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);	
	return JS_TRUE;
};


JSBool Transform_getScale(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * jsThisObj = JS_THIS_OBJECT(cx, vp);

	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Transform * transformObj = (Transform *)JS_GetPrivate(cx, jsThisObj);
	if (!JS_ConvertArguments(cx, argc, argv, ""))
        return JS_FALSE;

	Vector3 vec = transformObj->scaling();
	JSObject * jsVec = vec.createScriptObject(s);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsVec));	
	return JS_TRUE;
};


JSBool Transform_setScale(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * jsThisObj = JS_THIS_OBJECT(cx, vp);

	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Transform * thisObj = (Transform *)JS_GetPrivate(cx, jsThisObj);

	JSObject * jsVec;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsVec))
		return JS_FALSE;

	if (!jsVec)
	{
		JS_ReportError(cx, "parameter not a valid object");
		return JS_FALSE;
	}

	Vector3 vector3;
	getPropertyFloat(cx, jsVec, "x", &vector3.x);
	getPropertyFloat(cx, jsVec, "y", &vector3.y);
	getPropertyFloat(cx, jsVec, "z", &vector3.z);

	thisObj->setScaling(vector3.x, vector3.y, vector3.z);
	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);	
	return JS_TRUE;
};


JSBool Transform_getPivot(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * jsThisObj = JS_THIS_OBJECT(cx, vp);

	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Transform * transformObj = (Transform *)JS_GetPrivate(cx, jsThisObj);
	if (!JS_ConvertArguments(cx, argc, argv, ""))
        return JS_FALSE;

	Vector3 vec = transformObj->pivot();
	JSObject * jsVec = vec.createScriptObject(s);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsVec));	
	return JS_TRUE;
};


JSBool Transform_setPivot(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * jsThisObj = JS_THIS_OBJECT(cx, vp);

	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Transform * thisObj = (Transform *)JS_GetPrivate(cx, jsThisObj);

	JSObject * jsVec;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsVec))
		return JS_FALSE;

	if (!jsVec)
	{
		JS_ReportError(cx, "parameter not a valid object");
		return JS_FALSE;
	}

	Vector3 vector3;
	getPropertyFloat(cx, jsVec, "x", &vector3.x);
	getPropertyFloat(cx, jsVec, "y", &vector3.y);
	getPropertyFloat(cx, jsVec, "z", &vector3.z);

	thisObj->setPivot(vector3.x, vector3.y, vector3.z);
	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);	
	return JS_TRUE;
};

JSBool Transform_getMatrix(JSContext *cx, uintN argc, jsval *vp)
{/*
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * jsThisObj = JS_THIS_OBJECT(cx, vp);

	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Transform * transformObj = (Transform *)JS_GetPrivate(cx, jsThisObj);


	Vector3 vec = transformObj->pivot();
	JSObject * jsVec = vec.createScriptObject(s);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsVec));	*/
	return JS_TRUE;
};


JSFunctionSpec jsTransformFuncs[] = {
	JS_FS("getTranslation", Transform_getTranslation, 0, 0),
	JS_FS("setTranslation", Transform_setTranslation, 1, 0),
	
	JS_FS("getRotation", Transform_getRotation, 0, 0),
	JS_FS("setRotation", Transform_setRotation, 1, 0),
	
	JS_FS("getScale", Transform_getScale, 0, 0),
	JS_FS("setScale", Transform_setScale, 1, 0),

	JS_FS("getPivot", Transform_getPivot, 0, 0),
	JS_FS("setPivot", Transform_setPivot, 1, 0),

    JS_FS_END
};

///////////////////////////////////////////////////////////////////////////////

enum
{
	Transform_translation,
	Transform_rotation,
	Transform_scale,
	Transform_pivot
};


static JSBool Transform_getProperty(
	JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{	
	Transform * transform = (Transform *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	*vp = JSVAL_VOID;
	Vector3 retVec;
	switch (nID) {
		case Transform_translation: 
			retVec = transform->translation();
			JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retVec.createScriptObject(s)));
			break;	
		case Transform_rotation:
			retVec = transform->rotation();
			JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retVec.createScriptObject(s)));
			break;	
		case Transform_scale:
			retVec = transform->scaling();
			JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retVec.createScriptObject(s)));
			break;	
		case Transform_pivot:
			retVec = transform->pivot();
			JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retVec.createScriptObject(s)));
			break;	
		
	}
	return JS_TRUE;
}

JSBool Transform_setProperty(
	JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp)
{	
	Transform * transform = (Transform *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);
	switch (nID) {
		case Transform_translation:
			transform->trans_ = Vector3::fromScriptObject(s, JSVAL_TO_OBJECT(*vp));
			break;
		case Transform_rotation:
			transform->rot_ = Vector3::fromScriptObject(s, JSVAL_TO_OBJECT(*vp));
			break;
		case Transform_scale:
			transform->scale_ = Vector3::fromScriptObject(s, JSVAL_TO_OBJECT(*vp));
			break;
		case Transform_pivot:
			transform->pivot_ = Vector3::fromScriptObject(s, JSVAL_TO_OBJECT(*vp));
			break;
	}


	return JS_TRUE;
}

JSPropertySpec TransformProps[] = {
	
	{"translation",	Transform_translation, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Transform_getProperty, Transform_setProperty},

	{"rotation",	Transform_rotation, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Transform_getProperty, Transform_setProperty},

	{"scale",	Transform_scale, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Transform_getProperty, Transform_setProperty},

	{"pivot",	Transform_pivot, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Transform_getProperty, Transform_setProperty},	

	{0}
};



///////////////////////////////////////////////////////////////////////////////
void Transform_finalize(JSContext * cx, JSObject * obj)
{		
	Transform * privateData = (Transform *)JS_GetPrivate(cx, obj);
	delete privateData;
}

JSClass jsTransformClass = InitClass(
	"Transform", JSCLASS_HAS_PRIVATE, JS_PropertyStub, 
	JS_StrictPropertyStub, Transform_finalize, 0);


JSObject * Transform::scriptObjectProto(ScriptProcessor * s)
{
	JSObject * jsTransformProto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), 0, &jsTransformClass, 0, 0, 
		TransformProps, jsTransformFuncs, 
		0, 0); 
	s->rootJSObject(jsTransformProto);
	return jsTransformProto;	
}

JSObject * Transform::createScriptObject(ScriptProcessor * s) const
{	
	JSContext * cx = s->jsContext();
	JSObject * scriptObject = JS_NewObject(
		cx, &jsTransformClass, s->transformProto(), s->jsGlobal());

	Transform * newTransform = new Transform(*this);
	JS_SetPrivate(s->jsContext(), scriptObject, newTransform);
/*
	//Don't need this when we are using custom getters/setters for JS Transform class
	jsval val;
	val = OBJECT_TO_JSVAL(trans_.createScriptObject(s));
	JS_SetProperty(cx, scriptObject, "translation", &val);
	val = OBJECT_TO_JSVAL(rot_.createScriptObject(s));
	JS_SetProperty(cx, scriptObject, "rotation", &val);
	val = OBJECT_TO_JSVAL(scale_.createScriptObject(s));
	JS_SetProperty(cx, scriptObject, "scale", &val);
	val = OBJECT_TO_JSVAL(pivot_.createScriptObject(s));
	JS_SetProperty(cx, scriptObject, "pivot", &val);
*/	
	return scriptObject;
}

Transform Transform::fromJsonScriptObject(ScriptProcessor * s, JSObject * jsTransform)
{
	Transform ret;
	JSContext * cx = s->jsContext();
	jsval val;
	Vector3 vec;

	JS_GetProperty(cx, jsTransform, "translation", &val);	
	if (JSVAL_IS_OBJECT(val))
	{
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "x", &ret.trans_.x);
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "y", &ret.trans_.y);
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "z", &ret.trans_.z);
	}

	JS_GetProperty(cx, jsTransform, "rotation", &val);	
	if (JSVAL_IS_OBJECT(val))
	{
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "x", &ret.rot_.x);
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "y", &ret.rot_.y);
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "z", &ret.rot_.z);
	}

	JS_GetProperty(cx, jsTransform, "scale", &val);	
	if (JSVAL_IS_OBJECT(val))
	{
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "x", &ret.scale_.x);
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "y", &ret.scale_.y);
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "z", &ret.scale_.z);
	}

	JS_GetProperty(cx, jsTransform, "pivot", &val);	
	if (JSVAL_IS_OBJECT(val))
	{
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "x", &ret.pivot_.x);
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "y", &ret.pivot_.y);
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "z", &ret.pivot_.z);
	}

	return ret;
}

///default constructor, sets transformation to identity
Transform::Transform()
{
	rot_  = Vector3(0, 0, 0);
	trans_ = Vector3(0, 0, 0);
	scale_ = Vector3(1, 1, 1);
	shxy_ = shxz_ = shyz_ = 0;
	pivot_ = Vector3(0, 0, 0);
}

Transform::Transform(const Matrix & matrix)
{
	rot_  = Vector3(0, 0, 0);
	trans_ = Vector3(0, 0, 0);
	scale_ = Vector3(1, 1, 1);
	shxy_ = shxz_ = shyz_ = 0;
	pivot_ = Vector3(0, 0, 0);
	setMatrix(matrix, true);
}

bool Transform::isIdentity() const
{
	return
		rot_.x == 0 && rot_.y == 0 && rot_.z == 0 &&
		trans_.x == 0 && trans_.y == 0 && trans_.z == 0 &&
		scale_.x == 1 && scale_.y == 1 && scale_.z == 1 &&
		shxy_ == 0 && shxz_ == 0 && shyz_ == 0;
}

namespace
{

/**
	given three rotation angles (about each of the 3 axes)
	Calculate the corresponding rotation matrix
*/
void XYZToRotation(Matrix & pOut, float thetaX, float thetaY, float thetaZ)
{
	Matrix rot;
	pOut = Matrix::Rotate(thetaZ, 0, 0, 1);	
	rot = Matrix::Rotate(thetaY, 0, 1, 0);	
	pOut *= rot;
	rot = Matrix::Rotate(thetaX, 1, 0, 0);	
	pOut *= rot;
}


void RotationToXYZ(float & thetaX, float & thetaY, float & thetaZ, const Matrix & m)
{
    /*
    c - cos, s - sin, x - thetaX, y - thetaY, z - thetaZ
    cycz = m._11;
    cysz = m._21;
    -sy = m._31;
    sxcy = m._32
    cxcy = m._33;
    */   

    //2 sets of candidate solutions
    float thetaX1, thetaY1, thetaZ1;
    float thetaX2, thetaY2, thetaZ2;

    float sinThetaY = -m._31;

    //thetaY ~ +-pi/2 are special cases.
    //They correspond to gimbal lock.
    
    //if thetaY ~ pi/2
    if (sinThetaY > 0.9999f)
    {
        thetaY1 = thetaY2 = M_PI_2;
        thetaZ1 = 0;
        thetaX1 = atan2(m._12 / sinThetaY, m._13 / sinThetaY);

        thetaX2 = 0;
        thetaZ2 = atan2(-m._12, m._22);
    }
    //else if thetaY ~ -pi/2
    else if (sinThetaY < -0.9999f)
    {
        thetaY1 = thetaY2 = -M_PI_2;
        thetaZ1 = 0;
        thetaX1 = atan2(m._12 / sinThetaY, m._13 / sinThetaY);

        thetaX2 = 0;
        thetaZ2 = atan2(-m._12, m._22);
    }
    else
    {   
        thetaY1 = asin(sinThetaY); 
        if (thetaY1 > 0)
            thetaY2 = M_PI - thetaY1;
        else 
            thetaY2 = -M_PI - thetaY1;
        
        float cosThetaY1 = cos(thetaY1);
        float cosThetaY2 = cos(thetaY2);

        thetaZ1 = atan2(m._21/cosThetaY1, m._11/cosThetaY1);
        thetaX1 = atan2(m._32/cosThetaY1, m._33/cosThetaY1); 

        thetaZ2 = atan2(m._21/cosThetaY2, m._11/cosThetaY2);
        thetaX2 = atan2(m._32/cosThetaY2, m._33/cosThetaY2); 
    }

    //go with the more desirable of the two candidate solutions
    if(
        fabs(thetaX1) + fabs(thetaY1) + fabs(thetaZ1)
        <
        fabs(thetaX2) + fabs(thetaY2) + fabs(thetaZ2)
        )
    {
        thetaX = thetaX1; thetaY = thetaY1; thetaZ = thetaZ1;
    }
    else
    {
        thetaX = thetaX2; thetaY = thetaY2; thetaZ = thetaZ2;
    }      
    
}


/**
Do QR decomposition using householder reflections

Following the process given in
http://en.wikipedia.org/wiki/QR_decomposition

@param Q Orthogonal matrix output
@param R Upper triangular matrix output
@param A the original matrix input
*/
void QRDecomp(Matrix & Q, Matrix & R, const Matrix & A)
{	
	
	Matrix Qt;
	Matrix M = A;
	float a;
	float x1, x2, x3, x4;
	


	//zero out lower entries of 1st column	
	a = sqrt(M._11*M._11 + M._21*M._21 + M._31*M._31 + M._41*M._41);	
	if (M._11 > 0)a*=-1;
	x1=M._11, x2=M._21, x3=M._31, x4=M._41;		
	x1 = x1 - a;	
	a = sqrt(x1*x1 + x2*x2 + x3*x3 + x4*x4);	
	x1/=a, x2/=a, x3/=a, x4/=a;
	
	Qt._11 = 1 - 2*x1*x1;
	Qt._12 = Qt._21 = -2*x1*x2;
	Qt._13 = Qt._31 = -2*x1*x3;
	Qt._14 = Qt._41 = -2*x1*x4;
	
	Qt._22 = 1 - 2*x2*x2;
	Qt._23 = Qt._32 = -2*x2*x3;
	Qt._24 = Qt._42 = -2*x2*x4;

	Qt._33 = 1-2*x3*x3;
	Qt._34 = Qt._43 = -2*x3*x4;

	Qt._44 = 1-2*x4*x4;
	
	Q = Qt;
	M = Qt*M;

	//zero out lower entries of 2nd column
	
	a = sqrt(M._22*M._22 + M._32*M._32 + M._42*M._42);
	if (M._22 > 0)a*=-1;
	x2=M._22, x3=M._32, x4=M._42;
	x2 = x2 - a;
	a = sqrt(x2*x2 + x3*x3 + x4*x4);
	x2/=a, x3/=a, x4/=a;

	Qt._11 = 1;
	Qt._12 = Qt._21 = 0;
	Qt._13 = Qt._31 = 0;
	Qt._14 = Qt._41 = 0;
	
	Qt._22 = 1 - 2*x2*x2;
	Qt._23 = Qt._32 = -2*x2*x3;
	Qt._24 = Qt._42 = -2*x2*x4;

	Qt._33 = 1-2*x3*x3;
	Qt._34 = Qt._43 = -2*x3*x4;

	Qt._44 = 1-2*x4*x4;

	Q = Qt*Q;
	M = Qt*M;
	//zero out lower entries of 3rd column
	
	a = sqrt(M._33*M._33 + M._43*M._43);
	if (M._33 > 0)a*=-1;
	x3=M._33, x4=M._43;
	x3 = x3 - a;
	a = sqrt(x3*x3 + x4*x4);
	x3/=a, x4/=a;

	Qt._11 = 1;
	Qt._12 = Qt._21 = 0;
	Qt._13 = Qt._31 = 0;
	Qt._14 = Qt._41 = 0;
	
	Qt._22 = 1;
	Qt._23 = Qt._32 = 0;
	Qt._24 = Qt._42 = 0;

	Qt._33 = 1-2*x3*x3;
	Qt._34 = Qt._43 = -2*x3*x4;

	Qt._44 = 1-2*x4*x4;

	Q = Qt*Q;	
	R = Q*A;
	Q = Q.Transpose();	
}


}

/**
Get the rotation component of this transform in matrix form
*/
Matrix Transform::rotationMatrix() const
{
    Matrix matrix;
    XYZToRotation(matrix, rot_.x, rot_.y, rot_.z);
    return matrix;
}


/**
transformations are applied as follows:
translate pivot to origin -> scale -> shear -> rotate ->
reverse pivot translation -> translate
*/
Matrix Transform::computeMatrix() const
{    
    //translate pivot to origin    
	Matrix matrix = Matrix::Translate(-pivot_.x, -pivot_.y, -pivot_.z);

    //scale    
	Matrix temp = Matrix::Scale(scale_.x, scale_.y, scale_.z);
    matrix = temp * matrix;
	

    //shear matrix
	temp = Matrix::Identity();
    temp._12 = shxy_;
    temp._13 = shxz_;
    temp._23 = shyz_;	
    matrix = temp * matrix;
	
    //rotation matrix;
    XYZToRotation(temp, rot_.x, rot_.y, rot_.z);
    matrix = temp * matrix;

    //reverse pivot translation + translation
	temp = Matrix::Translate(
		pivot_.x + trans_.x, pivot_.y + trans_.y, pivot_.z + trans_.z);
    
	matrix = temp * matrix;
	return matrix;
}

/**
Sets the transform to be equivalent to the given (transformation) matrix
Keeps the same pivot. But appropriately changes all other transform fields
*/

void Transform::setMatrix(const Matrix & A, bool preserveScaling)
{
	Vector3 scale = this->scale_;
	
	Matrix Q, R;
	QRDecomp(Q, R, A);
	//A = QR


	Q = Q.Transpose();
	R = R.Transpose();
	//Now RQ = transpose(A). R has become lower triangular

	
	//Code intended to prevent scale sign flipping as much as possible.
	if (preserveScaling)
	{
		if (R._11 < 0 && scale.x > 0)
		{
			R._11 *= -1, R._21 *= -1, R._31 *= -1, R._41 *= -1;
			Q._11 *= -1, Q._12 *= -1, Q._13 *= -1, Q._14 *= -1;	
		}
		if (R._22 < 0 && scale.y > 0)
		{
			R._12 *= -1, R._22 *= -1, R._32 *= -1, R._42 *= -1;
			Q._21 *= -1, Q._22 *= -1, Q._23 *= -1, Q._24 *= -1;	
		}
		if (R._33 < 0 && scale.z > 0)
		{
			R._13 *= -1, R._23 *= -1, R._33 *= -1, R._43 *= -1;
			Q._31 *= -1, Q._32 *= -1, Q._33 *= -1, Q._34 *= -1;
		}
	}
	

	float det = Q.Determinant();
	if (det < 0)
	{
		//make Q into a proper rotation matrix if it isn't
		Q._11 *= -1;
		Q._12 *= -1;
		Q._13 *= -1;
		Q._14 *= -1;

		//also adjust R to compensate
		R._11 *= -1;
		R._21 *= -1;
		R._31 *= -1;
		R._41 *= -1;
	}
	//RQ still = transpose(A)
	
	
	/*
	Matrix DEBUGTrA = R * Q;
	float DEBUGdetQ = Q.Determinant();
	*/

	scale_.x = R._11;
	scale_.y = R._22;
	scale_.z = R._33;
	
	shxy_ = R._21 / scale_.y;
	shxz_ = R._31 / scale_.z; 
	shyz_ = R._32 / scale_.z;
	
	trans_.x = R._41, trans_.y = R._42, trans_.z = R._43;
	Matrix Qt = Q.Transpose();
	trans_ = Qt * trans_;
	
	RotationToXYZ(rot_.x, rot_.y, rot_.z, Qt);
	if (rot_.x != rot_.x) rot_.x = 0;
	if (rot_.y != rot_.y) rot_.y = 0;
	if (rot_.z != rot_.z) rot_.z = 0;

	/*
	Without the following, there will be a problem:
	matrix = transform.GetMatrix();
	transform.SetMatrix(matrix);
	can possibly lead to transform being changed (obviously it should be the
	same) if there was a non zero pivot.
	*/
	Vector3 pivotOrig = pivot_;
	pivot_ = Vector3(0, 0, 0);
	//This function will modify the translation so it will seem as if
	//the passed in matrix had this transform's pivot all along    
	setPivot(pivotOrig.x, pivotOrig.y, pivotOrig.z);
}

/**
Concatenate the specified rotation onto the existing rotation
*/
void Transform::concatRotation(const Matrix & concatRotMatrix)
{
    Matrix baseRotMatrix;

    XYZToRotation(baseRotMatrix, rot_.x, rot_.y, rot_.z);
    baseRotMatrix = concatRotMatrix * baseRotMatrix;
    RotationToXYZ(rot_.x, rot_.y, rot_.z, baseRotMatrix);

}

#if 0
/**
Concatenate the specified rotation onto the existing rotation
(rotation happens before translation)
*/
void Transform::ConcatRotation(const Vector3 & axis, float angle)
{
    Matrix concatRotMatrix;
    D3DXMatrixRotationAxis(&concatRotMatrix, &axis, angle);
    ConcatRotation(concatRotMatrix);
}

/**
Concatenate the specified rotation onto the existing rotation
*/
void Transform::ConcatRotation(const Matrix & concatRotMatrix)
{
    Matrix baseRotMatrix;

    XYZToRotation(baseRotMatrix, rot.x, rot.y, rot.z);
    baseRotMatrix *= concatRotMatrix;
    RotationToXYZ(rot.x, rot.y, rot.z, baseRotMatrix);

    UpdateMatrix();
}
#endif


void Transform::concatRotationPre(const Matrix & concatRotMatrix)
{
    Matrix baseRotMatrix;

    XYZToRotation(baseRotMatrix, rot_.x, rot_.y, rot_.z);
    baseRotMatrix = baseRotMatrix * concatRotMatrix;
    RotationToXYZ(rot_.x, rot_.y, rot_.z, baseRotMatrix);

}


void Transform::getCoordAxes(Vector3 & xAxis, Vector3 & yAxis, Vector3 & zAxis) const
{
    xAxis = Vector3(1, 0, 0);
    yAxis = Vector3(0, 1, 0);
    zAxis = Vector3(0, 0, 1);

    Matrix rotMatrix;
    XYZToRotation(rotMatrix, rot_.x, rot_.y, rot_.z);
	xAxis = rotMatrix * xAxis;
	yAxis = rotMatrix * yAxis;
	zAxis = rotMatrix * zAxis;
	xAxis.normalize();
	yAxis.normalize();
	zAxis.normalize();
     
    
}

void Transform::setTranslation(float tx, float ty, float tz)
{
	trans_.x = tx, trans_.y = ty, trans_.z = tz;
}

void Transform::setRotation(float rx, float ry, float rz)
{
	rot_.x = rx, rot_.y = ry, rot_.z = rz;
}

void Transform::setRotation(const Vector3 & axis, float angle)
{
    Matrix rotMatrix = Matrix::Rotate(angle, axis.x, axis.y, axis.z);
    
    RotationToXYZ(rot_.x, rot_.y, rot_.z, rotMatrix);

}

void Transform::setRotation(const Matrix & rotOnlyMatrix)
{
	RotationToXYZ(rot_.x, rot_.y, rot_.z, rotOnlyMatrix);
}

void Transform::setScaling(float sx, float sy, float sz)
{
	scale_.x = sx, scale_.y = sy, scale_.z = sz;
}

/**

*/
void Transform::setPivot(float px, float py, float pz)
{        
    Vector3 newPivot(px, py, pz);     

    Matrix T = computeMatrix(); //transform without any pivot translations or regular translations    
    T._14 = 0; T._24 = 0; T._34 = 0;

    Vector3 tn; //new translation    
    tn = newPivot - pivot_;   
	tn = T*tn;
    
    tn += pivot_ + trans_ - newPivot;

    //modify 
    trans_ = tn;
    pivot_ = newPivot;
}

void Transform::setShear(float xy, float xz, float yz)
{
	shxy_ = xy, shxz_ = xz, shyz_ = yz;
}


void Transform::concatenate(const Transform & transform)
{
	Matrix matrix = transform.computeMatrix();
	concatenate(matrix);
}


void Transform::concatenate(const Matrix & matrix, bool preserveScaling)
{	
	setMatrix(matrix * computeMatrix(), preserveScaling);
}

Vector3 Transform::globalPivot() const
{
	return computeMatrix() * pivot_;    
}

Transform Transform::lerp(const Transform & rhs, float t) const
{
	Transform ret;
	ret.rot_ = rot_ + t*(rhs.rot_ - rot_);
	ret.trans_ = trans_ + t*(rhs.trans_ - trans_);
	ret.scale_ = scale_ + t*(rhs.scale_ - scale_);
	ret.pivot_ = pivot_ + t*(rhs.pivot_ - pivot_);
	ret.shxy_ = shxy_ + t*(rhs.shxy_ - shxy_);
	ret.shxz_ = shxz_ + t*(rhs.shxz_ - shxz_);
	ret.shyz_ = shyz_ + t*(rhs.shyz_ - shyz_);
	return ret;
}

bool Transform::operator == (const Transform & rhs) const
{
	return rot_ == rhs.rot_ && trans_ == rhs.trans_ && scale_ == rhs.scale_ &&
		pivot_ == rhs.pivot_ && shxy_ == rhs.shxy_ && shxz_ == rhs.shxz_ && 
		shyz_ == rhs.shyz_;
}

bool Transform::operator != (const Transform & rhs) const
{
	return !(*this == rhs);
}



void Transform::write(Writer & writer) const
{
	writer.write(rot_, "rot");
	writer.write(trans_, "trans");
	writer.write(scale_, "scale");
	writer.write(pivot_, "pivot");
	writer.write(shxy_, "shxy");
	writer.write(shxz_, "shxz");
	writer.write(shyz_, "shyz");	
}

void Transform::writeXml(XmlWriter & w) const
{
	w.writeTag("Rotation", rot_);
	w.writeTag("Translation", trans_);
	w.writeTag("Scale", scale_);
	w.writeTag("Pivot", pivot_);
	w.writeTag("ShearXY", shxy_);
	w.writeTag("ShearXZ", shxz_);
	w.writeTag("ShearYZ", shyz_);
}

void Transform::readXml(XmlReader & r, xmlNode * parent) 
{
	for (xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(trans_, curNode, "Translation"));
		else if (r.getNodeContentIfName(scale_, curNode, "Scale"));
		else if (r.getNodeContentIfName(pivot_, curNode, "Pivot"));
		else if (r.getNodeContentIfName(shxy_, curNode, "ShearXY"));
		else if (r.getNodeContentIfName(shxz_, curNode, "ShearXZ"));
		else if (r.getNodeContentIfName(shyz_, curNode, "ShearYZ"));
	}
}

void Transform::read(Reader & reader, unsigned char version)
{
	reader.read(rot_);
	reader.read(trans_);
	reader.read(scale_);
	reader.read(pivot_);
	reader.read(shxy_);
	reader.read(shxz_);
	reader.read(shyz_);	
}


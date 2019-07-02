#include "stdafx.h"
#include "PixelBuffer.h"
#include "ScriptProcessor.h"
#include "Image.h"
#include "TextureRenderer.h"

using namespace std;

void PixelBuffer_finalize(JSContext * cx, JSObject * obj)
{		
	PixelBuffer * privateData = (PixelBuffer *)JS_GetPrivate(cx, obj);
	delete privateData;
}

JSClass jsPixelBufferClass = InitClass(
	"PixelBuffer", JSCLASS_HAS_PRIVATE, JS_PropertyStub, 
	JS_StrictPropertyStub, PixelBuffer_finalize, 0);

JSBool PixelBuffer_constructor(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	jsdouble jsWidth, jsHeight;
	if (!JS_ConvertArguments(cx, argc, argv, "dd", &jsWidth, &jsHeight)) return JS_FALSE;

	PixelBuffer * pixelBuffer = new PixelBuffer((int)jsWidth, (int)jsHeight);

	JSObject * newObject = JS_NewObject(cx, &jsPixelBufferClass, s->pixelBufferProto(), 0);	
	JS_SetPrivate(cx, newObject, pixelBuffer);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(newObject));
	return JS_TRUE;
};

JSBool PixelBuffer_setFillColor(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsColor;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsColor)) return JS_FALSE;	
	pixelBuffer->setFillColor(Color::fromScriptObject(s, jsColor));
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};


JSBool PixelBuffer_fill(JSContext *cx, uintN argc, jsval *vp)
{		
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));	
	pixelBuffer->fill();
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool PixelBuffer_getPixel(JSContext *cx, uintN argc, jsval *vp)
{		
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	unsigned int x, y;
	if (!JS_ConvertArguments(cx, argc, argv, "uu", &x, &y)) return JS_FALSE;	
	Color color;
	bool ret = pixelBuffer->getPixel((int)x, (int)y, &color);
	if (!ret)
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	else
	{
		JSObject * jsColor = color.createScriptObject(s);
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsColor));
	}

	return JS_TRUE;
}

JSBool PixelBuffer_getBitsFromImage(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsImage;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsImage)) return JS_FALSE;	
	Image * image = NULL;
	if (isSceneObject(s, jsImage))
	{
		SceneObject * sceneObject = (SceneObject *)JS_GetPrivate(cx, jsImage);
		if (sceneObject->type() == Image().type())
			image = (Image *)sceneObject;
	}
	if (!image)
	{
		JS_ReportError(cx, "PixelBuffer::getBitsFromImage - parameter must be an Image");
		return JS_FALSE;
	}

	bool ret = pixelBuffer->getBitsFromImage(image);
	if (ret) JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	else JS_SET_RVAL(cx, vp, JSVAL_FALSE);
	return JS_TRUE;
}

JSBool PixelBuffer_fillRect(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	unsigned int x, y, w, h;
	if (!JS_ConvertArguments(cx, argc, argv, "uuuu", &x, &y, &w, &h)) return JS_FALSE;	
	pixelBuffer->fillRect(x, y, w, h);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool PixelBuffer_fillCircle(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	unsigned int x, y, r;
	if (!JS_ConvertArguments(cx, argc, argv, "uuu", &x, &y, &r)) return JS_FALSE;	
	pixelBuffer->fillCircle((int)x, (int)y, (int)r);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool PixelBuffer_copyFrom(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsRhs;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsRhs)) return JS_FALSE;	

	//TODO check if this is indeed a PixelBuffer
	PixelBuffer * rhs = (PixelBuffer *)JS_GetPrivate(cx, jsRhs);
	bool ret = pixelBuffer->copyFrom(0, 0, *rhs, 0, 0, pixelBuffer->width(), pixelBuffer->height(), PixelBuffer::NORMAL);
	if (ret) JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	else JS_SET_RVAL(cx, vp, JSVAL_FALSE);
	return JS_TRUE;
}

JSBool PixelBuffer_copyFromRegion(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsRhs;
	jsint lx, ly, x, y, w, h, mode;
	if (!JS_ConvertArguments(cx, argc, argv, "iioiiiii", &lx, &ly, &jsRhs, &x, &y, &w, &h, &mode)) return JS_FALSE;	

	//TODO check if this is indeed a PixelBuffer
	PixelBuffer * rhs = (PixelBuffer *)JS_GetPrivate(cx, jsRhs);
	bool ret = pixelBuffer->copyFrom(lx, ly, *rhs, x, y, w, h, (PixelBuffer::FillMode)mode);
	if (ret) JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	else JS_SET_RVAL(cx, vp, JSVAL_FALSE);
	return JS_TRUE;
}

JSBool PixelBuffer_invert(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	pixelBuffer->invert();
	return JS_TRUE;
}

JSBool PixelBuffer_add(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsRhs;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsRhs)) return JS_FALSE;	

	//TODO check if this is indeed a PixelBuffer
	PixelBuffer * rhs = (PixelBuffer *)JS_GetPrivate(cx, jsRhs);
	bool ret = pixelBuffer->add(*rhs);
	if (ret) JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	else JS_SET_RVAL(cx, vp, JSVAL_FALSE);
	return JS_TRUE;
}

JSBool PixelBuffer_countPixels(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsColor;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsColor)) return JS_FALSE;	

	//TODO check if this is indeed a Color
	Color color = Color::fromScriptObject(s, jsColor);	
	int count = pixelBuffer->countPixels(color, 0, 0, pixelBuffer->width(), pixelBuffer->height());
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(count));
	return JS_TRUE;
}

JSBool PixelBuffer_countPixelsInRegion(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsColor;
	jsint x, y, w, h;
	if (!JS_ConvertArguments(cx, argc, argv, "oiiii", &jsColor, &x, &y, &w, &h)) return JS_FALSE;	

	//TODO check if this is indeed a Color
	Color color = Color::fromScriptObject(s, jsColor);	
	int count = pixelBuffer->countPixels(color, x, y, w, h);
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(count));
	return JS_TRUE;
}

JSBool PixelBuffer_countPixelsWithAlpha(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsAlpha;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsAlpha)) return JS_FALSE;			
	int count = pixelBuffer->countPixelsWithAlpha((float)jsAlpha);
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(count));
	return JS_TRUE;
}

JSBool PixelBuffer_replacePixels(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsOldColor, * jsNewColor;
	if (!JS_ConvertArguments(cx, argc, argv, "oo", &jsOldColor, &jsNewColor)) return JS_FALSE;	

	//TODO check if this is indeed a Color
	Color oldColor = Color::fromScriptObject(s, jsOldColor);
	Color newColor = Color::fromScriptObject(s, jsNewColor);
	pixelBuffer->replacePixels(oldColor, newColor);
	return JS_TRUE;
}

JSBool PixelBuffer_delete(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	pixelBuffer->deleteBits();
	return JS_TRUE;
}

JSBool PixelBuffer_fillCircleCrayon(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	unsigned int x, y, r;
	if (!JS_ConvertArguments(cx, argc, argv, "uuu", &x, &y, &r)) return JS_FALSE;	
	pixelBuffer->fillCircleCrayon((int)x, (int)y, (int)r);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool PixelBuffer_fillCircleBrush(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	unsigned int x, y, r;
	if (!JS_ConvertArguments(cx, argc, argv, "uuu", &x, &y, &r)) return JS_FALSE;	
	pixelBuffer->fillCircleBrush((int)x, (int)y, (int)r);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};


JSFunctionSpec PixelBufferFuncs[] = {
	JS_FS("setFillColor", PixelBuffer_setFillColor, 1, 0),

	JS_FS("fillRect", PixelBuffer_fillRect, 4, 0),
	JS_FS("fill", PixelBuffer_fill, 0, 0),

	JS_FS("getPixel", PixelBuffer_getPixel, 2, 0),

	JS_FS("copyFrom", PixelBuffer_copyFrom, 1, 0),
	JS_FS("copyFromRegion", PixelBuffer_copyFromRegion, 8, 0),	
	JS_FS("invert", PixelBuffer_invert, 0, 0),
	JS_FS("add", PixelBuffer_add, 1, 0),
	JS_FS("countPixels", PixelBuffer_countPixels, 1, 0),
	JS_FS("countPixelsInRegion", PixelBuffer_countPixelsInRegion, 5, 0),
	JS_FS("countPixelsWithAlpha", PixelBuffer_countPixelsWithAlpha, 1, 0),
	JS_FS("replacePixels", PixelBuffer_replacePixels, 2, 0),

	JS_FS("getBitsFromImage", PixelBuffer_getBitsFromImage, 1, 0),

	JS_FS("fillCircle", PixelBuffer_fillCircle, 3, 0),
	JS_FS("fillCircleCrayon", PixelBuffer_fillCircleCrayon, 3, 0),
	JS_FS("fillCircleBrush", PixelBuffer_fillCircleBrush, 3, 0),

	JS_FS("delete", PixelBuffer_delete, 0, 0),

	JS_FS_END
};

///////////////////////////////////////////////////////////////////////////////

JSObject * PixelBuffer::createScriptObjectProto(ScriptProcessor * s, JSObject * global)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), global, 0, &jsPixelBufferClass,
		PixelBuffer_constructor, 2, 0, PixelBufferFuncs, 0, 0);

	return proto;
}

JSObject * PixelBuffer::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsPixelBufferClass, s->pixelBufferProto(), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);
	}

	return scriptObject_;
}

PixelBuffer::PixelBuffer(int width, int height)
{
	width_ = width;
	height_ = height;

	bits_ = new unsigned char[width*height*4];
	fillColor_ = Color(0.0f, 0.0f, 0.0f, 1.0f);
}

PixelBuffer::PixelBuffer()
{
	width_ = 0;
	height_ = 0;
	bits_ = NULL;
}

PixelBuffer::~PixelBuffer()
{
	delete [] bits_;
}

void PixelBuffer::setFillColor(const Color & color)
{
	fillColor_ = color;
}

void PixelBuffer::setPenColor(const Color & color)
{
	penColor_ = color;
}


void PixelBuffer::fill()
{
	unsigned char * ptr = bits_;
	unsigned char r = (unsigned char)(255*fillColor_.r);
	unsigned char g = (unsigned char)(255*fillColor_.g);
	unsigned char b = (unsigned char)(255*fillColor_.b);
	unsigned char a = (unsigned char)(255*fillColor_.a);

	for (int i = 0; i < width_ * height_; ++i)
	{
		setPixelPtr(ptr, r, g, b, a);	
	}
}

void PixelBuffer::fillRect(int x, int y, int width, int height)
{
	unsigned char r = (unsigned char)(255*fillColor_.r);
	unsigned char g = (unsigned char)(255*fillColor_.g);
	unsigned char b = (unsigned char)(255*fillColor_.b);
	unsigned char a = (unsigned char)(255*fillColor_.a);


	fitWithinBuffer(x, y, width, height);
	int maxX = x + width;
	int maxY = y + height;

	unsigned char * ptr = bits_ + 4*(width_*y + x);
	for (int j = y; j < maxY; ++j)
	{
		for (int i = x; i < maxX; ++i)
		{
			setPixelPtr(ptr, r, g, b, a);			
		}
		ptr += 4*(width_ - (maxX - x));
	}
}

void PixelBuffer::fillCircle(int x0, int y0, int radius)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	unsigned char r = (unsigned char)(255*fillColor_.r);
	unsigned char g = (unsigned char)(255*fillColor_.g);
	unsigned char b = (unsigned char)(255*fillColor_.b);
	unsigned char a = (unsigned char)(255*fillColor_.a);

	setPixel(x0, y0 + radius, r, g, b, a);
	setPixel(x0, y0 - radius, r, g, b, a);
	for (int i = x0 - radius; i <= x0 + radius; ++i)
		setPixel(i, y0,  r, g, b, a);

	while(x < y)
	{
		if(f >= 0) 
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;    

		for (int i = x0 - x; i <= x0 + x; ++i)
		{
			setPixel(i, y0 - y,  r, g, b, a);
			setPixel(i, y0 + y,  r, g, b, a);
		}

		for (int i = x0 - y; i <= x0 + y; ++i)
		{
			setPixel(i, y0 + x,  r, g, b, a);
			setPixel(i, y0 - x,  r, g, b, a);
		}
	}
}

void PixelBuffer::fillCircleCrayon(int x0, int y0, int radius)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	unsigned char r = (unsigned char)(255*fillColor_.r);
	unsigned char g = (unsigned char)(255*fillColor_.g);
	unsigned char b = (unsigned char)(255*fillColor_.b);
	unsigned char a;

	float fa = fillColor_.a;
	float origA = fa;
	float delta = -0.04;
	int prob = 70;
	//float correction = 0.5;
	Color gc;

	setPixel(x0, y0 + radius, fillColor_);
	setPixel(x0, y0 - radius, fillColor_);
	for (int i = x0 - radius; i <= x0 + radius; ++i)
	{
		if (rand()%100 < prob)
			fa = fa + delta;
		if (fa < origA - 0.4 && delta < 0)
		{
			fa = 0;
			delta = -delta;
		}
		else if (fa > 0.3 && delta > 0)
		{
			fa = origA;
			delta = -delta;
		}
		if (fa < 0 || fa > 1) fa = 1;
		a = (unsigned char)(255*fa);
		setPixel(i, y0,  r, g, b, a);
	}

	while(x < y)
	{
		if(f >= 0) 
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;    

		for (int i = x0 - x; i <= x0 + x; ++i)
		{
			if (rand()%100 < prob)
				fa = fa + delta;
			if (fa < origA - 0.4 && delta < 0)
			{
				fa = 0;
				delta = -delta;
			}
			else if (fa > 0.3 && delta > 0)
			{
				fa = origA;
				delta = -delta;
			}
			if (fa < 0 || fa > 1) fa = 1;
			a = (unsigned char)(255*fa);
			setPixel(i, y0 - y,  r, g, b, a);
			if (rand()%100 < prob)
				fa = fa + delta;
			if (fa < origA - 0.4 && delta < 0)
			{
				fa = 0;
				delta = -delta;
			}
			else if (fa > 0.3 && delta > 0)
			{
				fa = origA;
				delta = -delta;
			}
			if (fa < 0 || fa > 1) fa = 1;
			a = (unsigned char)(255*fa);
			setPixel(i, y0 + y,  r, g, b, a);
		}

		for (int i = x0 - y; i <= x0 + y; ++i)
		{
			if (rand()%100 < prob)
				fa = fa + delta;
			if (fa < origA - 0.4 && delta < 0)
			{
				fa = 0;
				delta = -delta;
			}
			else if (fa > 0.3 && delta > 0)
			{
				fa = origA;
				delta = -delta;
			}
			if (fa < 0 || fa > 1) fa = 1;
			a = (unsigned char)(255*fa);
			setPixel(i, y0 + x,  r, g, b, a);
			if (rand()%100 < prob)
				fa = fa + delta;
			if (fa < origA - 0.4 && delta < 0)
			{
				fa = 0;
				delta = -delta;
			}
			else if (fa > 0.3 && delta > 0)
			{
				fa = origA;
				delta = -delta;
			}
			if (fa < 0 || fa > 1) fa = 1;
			a = (unsigned char)(255*fa);
			setPixel(i, y0 - x,  r, g, b, a);
		}
	}
	fillColor_.a = fa;
}

void PixelBuffer::fillCircleBrush(int x0, int y0, int radius)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	Color gc;

	unsigned char r = (unsigned char)(255*fillColor_.r);
	unsigned char g = (unsigned char)(255*fillColor_.g);
	unsigned char b = (unsigned char)(255*fillColor_.b);
	unsigned char a = (unsigned char)(255*fillColor_.a);

	for (int i = x0 - radius; i <= x0 + radius; ++i)
	{
		float delta = fabs(1.0/(x0 - i));
		if (x0 == i ) delta = 1;
		getPixel(i, y0,  &gc);
		float fa = (255*fillColor_.a*delta) + (255*gc.a);
		if (fa > 255) fa = 255;
		unsigned char deltaA = (unsigned char)(fa);
		setPixel(i, y0,  r, g, b, deltaA);
	}


	while(x < y)
	{
		if(f >= 0) 
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		for (int i = x0 - y; i <= x0 + y; ++i)
		{
			float deltax = fabs(1.0/(x0 - i));
			if (x0 == i ) deltax = 1;
			float deltay = 1;

			getPixel(i, y0 + x,  &gc);
			float fa = (255*fillColor_.a*deltax*deltay) + (255*gc.a);
			if (fa > 255) fa = 255;
			unsigned char deltaA = (unsigned char)(fa);
			setPixel(i, y0 + x,  r, g, b, deltaA);

			getPixel(i, y0 - x,  &gc);
			fa = (255*fillColor_.a*deltax*deltay) + (255*gc.a);
			if (fa > 255) fa = 255;
			deltaA = (unsigned char)(fa);
			setPixel(i, y0 - x,  r, g, b, deltaA);
		}
	}
}

void PixelBuffer::deleteBits()
{
	delete [] bits_;
	width_ = 0;
	height_ = 0;
	bits_ = NULL;
}

void PixelBuffer::allocateBits(int width, int height)
{
	bits_ = new unsigned char[width * height * 4];
	width_ = width;
	height_ = height;
}

bool PixelBuffer::getBitsFromImage(Image * image)
{
	if (!image->texture()->isLoaded()) return false;

	Texture * texture = const_cast<Texture *>(image->texture());
	TextureRenderer renderer;
	renderer.init(image->renderer(), texture->width(), texture->height(), false);	
	renderer.setTexture(texture);	
	deleteBits();
	bits_ = renderer.textureBits();
	width_ = texture->width();
	height_ = texture->height();

	renderer.uninit();
	return true;
}

void PixelBuffer::setBits(unsigned char * bits)
{
	memcpy(bits_, bits, width_*height_*4);
}

bool PixelBuffer::setPixel(int x, int y, const Color & color)
{
	if (x < 0 || x >= width_) return false;
	if (y < 0 || y >= height_) return false;

	unsigned char * ptr = bits_ + 4*(y*width_ + x);

	unsigned char r = (unsigned char)(255*fillColor_.r);
	unsigned char g = (unsigned char)(255*fillColor_.g);
	unsigned char b = (unsigned char)(255*fillColor_.b);
	unsigned char a = (unsigned char)(255*fillColor_.a);

	setPixelPtr(ptr, r, g, b, a);

	return true;
}

bool PixelBuffer::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if (x < 0 || x >= width_) return false;
	if (y < 0 || y >= height_) return false;
	unsigned char * ptr = bits_ + 4*(y*width_ + x);

	setPixelPtr(ptr, r, g, b, a);	

	return true;
}

bool PixelBuffer::getPixel(int x, int y, Color * color)
{
	if (x < 0 || x >= width_) return false;
	if (y < 0 || y >= height_) return false;

	unsigned char * ptr = bits_ + 4*(y*width_ + x);

	color->r = *(ptr++) / 255.0f;
	color->g = *(ptr++) / 255.0f;
	color->b = *(ptr++) / 255.0f;
	color->a = *(ptr++) / 255.0f;	

	return true;
}


bool PixelBuffer::add(const PixelBuffer & rhs)
{
	if (rhs.width() != width() || rhs.height() != height()) return false;

	for (int i = 0; i < 4 * width_ * height_; ++i)
	{
		unsigned short sum = (unsigned short)(bits_[i] + rhs.bits_[i]);
		if (sum > 255) bits_[i] = 255;
		else bits_[i] = (unsigned char)sum;		
	}
	return true;
}

void PixelBuffer::invert()
{
	for (int i = 0; i < 4 * width_ * height_; ++i)
	{
		bits_[i] = 255 - bits_[i];
	}
}

void PixelBuffer::fitWithinBuffer(int & rx, int & ry, int & rw, int & rh) const
{
	if (rw < 0)
	{
		rx += rw;
		rw *= -1;
	}

	if (rh < 0)
	{
		ry += rh;
		rh *= -1;
	}

	if (rx < 0) 
	{
		rw += rx;
		if (rw < 0) rw = 0;
		rx = 0;
	}

	if (ry < 0)
	{
		rh += ry;
		if (rh < 0) rh = 0;
		ry = 0;
	}

	if (rx >= width_)
	{
		rx = width_ - 1;
	}

	if (ry >= height_)
	{
		ry = height_ - 1;
	}

	if (rx + rw > width_) rw = width_ - rx;
	if (ry + rh > height_) rh = height_ - ry;
}

bool PixelBuffer::copyFrom(int lx, int ly, const PixelBuffer & rhs, int rx, int ry, int rw, int rh, PixelBuffer::FillMode fillMode)
{	
	int lw = rw;
	int lh = rh;
	rhs.fitWithinBuffer(rx, ry, rw, rh);	
	fitWithinBuffer(lx, ly, lw, lh);
	if (rw < lw) lw = rw;
	if (rh < lh) lh = rh;

	unsigned char * src = rhs.bits_ + 4*(ry*rhs.width_ + rx);
	unsigned char * dst = bits_ + 4*(ly*width_ + lx);

	switch(fillMode)
	{
	case NORMAL:
		for (int j = 0; j < lh; ++j)
		{
			memcpy(dst, src, 4*lw);
			src += 4*rhs.width_;
			dst += 4*width_;
		}
		break;
	case XOR:
		for (int j = 0; j < lh; ++j)
		{
			for (int i = 0; i < lw; ++i)
			{
				*dst = *dst ^ *(src++); dst++;
				*dst = *dst ^ *(src++); dst++;
				*dst = *dst ^ *(src++); dst++;
				//TODO : maybe don't touch the alpha component during XOR
				*dst = *dst ^ *(src++); dst++;				
			}
			src += 4*(rhs.width_ - lw);
			dst += 4*(width_ - lw);
		}
		break;
	case ALPHABLEND:
		for (int j = 0; j < lh; ++j)
		{
			for (int i = 0; i < lw; ++i)
			{
				float alpha = src[3] / 255.0f;
				*dst = (unsigned char)(*dst + alpha*(*src - *dst)); dst++; src++;
				*dst = (unsigned char)(*dst + alpha*(*src - *dst)); dst++; src++;
				*dst = (unsigned char)(*dst + alpha*(*src - *dst)); dst++; src++;
				//preserve destination alpha
				dst++; src++;
				//*dst = unsigned char(*dst + alpha*(*src - *dst)); dst++; src++;				

			}
			src += 4*(rhs.width_ - lw);
			dst += 4*(width_ - lw);
		}	
		break;
	case ALPHACHANNEL:
		for (int j = 0; j < lh; ++j)
		{
			for (int i = 0; i < lw; ++i)
			{
				dst++; src++;
				dst++; src++;
				dst++; src++;
				*(dst++) = *(src++);				
			}
			src += 4*(rhs.width_ - lw);
			dst += 4*(width_ - lw);
		}	
		break;
/*
	case MULTIPLY:
		for (int j = 0; j < lh; ++j)
		{
			for (int i = 0; i < lw; ++i)
			{
				*dst = (unsigned char)(*src + (*src / 255.0f)*(*dst - *src)); dst++; src++;
				*dst = (unsigned char)(*src + (*src / 255.0f)*(*dst - *src)); dst++; src++;
				*dst = (unsigned char)(*src + (*src / 255.0f)*(*dst - *src)); dst++; src++;
				*dst = (unsigned char)(*src + (*src / 255.0f)*(*dst - *src)); dst++; src++;			

			}
			src += 4*(rhs.width_ - lw);
			dst += 4*(width_ - lw);
		}	
		break;
*/
	}


	return true;
}


int PixelBuffer::countPixels(const Color & color, int x, int y, int w, int h) const
{
	fitWithinBuffer(x, y, w, h);

	unsigned char r = (unsigned char)(255*color.r);
	unsigned char g = (unsigned char)(255*color.g);
	unsigned char b = (unsigned char)(255*color.b);
	unsigned char a = (unsigned char)(255*color.a);
	int count = 0;

	unsigned char * ptr = bits_ + 4*(y*width_ + x);

	for (int j = 0; j < h; ++j)
	{
		for (int i = 0; i < w; ++i)
		{
			if (ptr[0] == r && ptr[1] == g && ptr[2] == b && ptr[3] == a) 
				++count;	
			ptr += 4;
		}
		ptr += 4*(width_ - w);
	}

	return count;
}

int PixelBuffer::countPixelsWithAlpha(float alpha) const
{
	unsigned char a = (unsigned char)(255*alpha);
	int count = 0;
	unsigned char * ptr = bits_;
	for (int i = 0; i < width_ * height_; ++i)
	{
		ptr += 3;
		if (*(ptr++) == a) ++count;
	}

	return count;
}

void PixelBuffer::replacePixels(const Color & oldColor, const Color & newColor)
{
	unsigned char newr = (unsigned char)(255*newColor.r);
	unsigned char newg = (unsigned char)(255*newColor.g);
	unsigned char newb = (unsigned char)(255*newColor.b);
	unsigned char newa = (unsigned char)(255*newColor.a);

	unsigned char oldr = (unsigned char)(255*oldColor.r);
	unsigned char oldg = (unsigned char)(255*oldColor.g);
	unsigned char oldb = (unsigned char)(255*oldColor.b);
	unsigned char olda = (unsigned char)(255*oldColor.a);

	unsigned char * ptr = bits_;
	for (int i = 0; i < width_ * height_; ++i)
	{
		if (ptr[0] == oldr && ptr[1] == oldg && ptr[2] == oldb && ptr[3] == olda)
		{
			ptr[0] = newr;
			ptr[1] = newg;
			ptr[2] = newb;
			ptr[3] = newa;
		}
		ptr += 4;
	}
}
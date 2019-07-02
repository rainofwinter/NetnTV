#include "stdafx.h"
#include "Scene.h"
#include "Document.h"
#include "DocumentTemplate.h"
#include "Image.h"
#include "Mesh.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Reader.h"
#include "FileUtils.h"
#include "Global.h"
#include "ScriptProcessor.h"
#include "Color.h"
#include "PixelBuffer.h"
#include "TextureRenderer.h"
#include "Xml.h"

using namespace std;

extern JSClass jsPixelBufferClass;

JSClass jsImageClass = InitClass(
	"Image", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);

JSBool Image_setWidth(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Image * thisObj = (Image *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsDouble;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsDouble)) return JS_FALSE;
	thisObj->setWidth((int)jsDouble);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Image_setHeight(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Image * thisObj = (Image *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsdouble jsDouble;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsDouble)) return JS_FALSE;
	thisObj->setHeight((int)jsDouble);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Image_getWidth(JSContext *cx, uintN argc, jsval *vp)
{
	Image * thisObj = (Image *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(thisObj->width()));
	return JS_TRUE;
};

JSBool Image_getHeight(JSContext *cx, uintN argc, jsval *vp)
{
	Image * thisObj = (Image *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(thisObj->height()));
	return JS_TRUE;
};

JSBool Image_setFileName(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);
	Image * thisObj = (Image *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jschar * jsStr;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &jsStr)) return JS_FALSE;
	std::string fileName = s->getActualFileName(jsCharStrToStdString(jsStr));
	thisObj->setFileName(fileName);	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Image_getFileName(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Image * thisObj = (Image *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "")) return JS_FALSE;
	
	const char * idStr = thisObj->fileName().c_str();
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, idStr)));
	return JS_TRUE;
};

JSBool Image_setFormat(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Image * thisObj = (Image *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jschar * jsStr;
	uint32 jsFormat;
	if (!JS_ConvertArguments(cx, argc, argv, "u", &jsFormat)) return JS_FALSE;
	std::string fileName = jsCharStrToStdString(jsStr);
	
	bool validFormat = false;
	Texture::Format format;

	switch(jsFormat)
	{
	case (uint32)Texture::UncompressedRgb16: 
	case (uint32)Texture::UncompressedRgba16: 
	case (uint32)Texture::UncompressedRgba32: 
	case (uint32)Texture::UncompressedA8: 
		validFormat = true;
		format = (Texture::Format)jsFormat;
		break;
		
	}
	thisObj->setFormat(format);
		
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Image_getBitsFromPixelBuffer(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);
	JS_SET_RVAL(cx, vp, JSVAL_FALSE);
	Image * thisObj = (Image *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;

	JSObject * proto = JS_GetPrototype(cx, jsObj);
	if (proto != s->pixelBufferProto())
	{
		JS_ReportError(cx, "Image::getBitsFromPixelBuffer - parameter must be a PixelBuffer");
		return JS_FALSE;
	}

	PixelBuffer * pixelBuffer = (PixelBuffer *)JS_GetPrivate(cx, jsObj);
	bool ret = thisObj->getBitsFromPixelBuffer(pixelBuffer);

	if (ret)
		JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	else
		JS_SET_RVAL(cx, vp, JSVAL_FALSE);
	return JS_TRUE;
};

JSBool Image_createPixelBufferFromImage(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Image * thisObj = (Image *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	PixelBuffer * pixelBuffer = thisObj->createPixelBufferFromImage();
	if (pixelBuffer)
	{
		JSObject * newObject = JS_NewObject(cx, &jsPixelBufferClass, s->pixelBufferProto(), 0);	
		JS_SetPrivate(cx, newObject, pixelBuffer);
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(newObject));
	}
	else
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	return JS_TRUE;
}


JSFunctionSpec ImageFuncs[] = {
	JS_FS("setWidth", Image_setWidth, 1, 0),
	JS_FS("setHeight", Image_setHeight, 1, 0),

	JS_FS("getWidth", Image_getWidth, 0, 0),
	JS_FS("getHeight", Image_getHeight, 0, 0),	

	JS_FS("setFileName", Image_setFileName, 1, 0),
	JS_FS("getFileName", Image_getFileName, 0, 0),

	JS_FS("setFormat", Image_setFormat, 0, 0),

	JS_FS("getBitsFromPixelBuffer", Image_getBitsFromPixelBuffer, 1, 0),
	JS_FS("createPixelBufferFromImage", Image_createPixelBufferFromImage, 0, 0),

    JS_FS_END
};


///////////////////////////////////////////////////////////////////////////////
JSObject * Image::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsImageClass,
		0, 0, 0, ImageFuncs, 0, 0);

	return proto;
}

JSObject * Image::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsImageClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void Image::create()
{
	texture_ = new Texture;
	width_ = 0;
	height_ = 0;
	format_ = Texture::UncompressedRgba32;

	pixelDraw_ = false;
}

Image::Image()
{
	create();
	setId("Image");
}

Image::Image(const Image & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();
	fileName_ = rhs.fileName_;
	visualAttrib_ = rhs.visualAttrib_;
	width_ = rhs.width_;
	height_ = rhs.height_;
	pixelDraw_ = rhs.pixelDraw_;
}

Image::~Image()
{	
	delete texture_;	
}

void Image::swap(Image * other)
{
	Texture * tempTexture = texture_;
	string tempFileName = fileName_;
	int tempWidth = width_;
	int tempHeight = height_;
	bool tempPixelDraw = pixelDraw_;

	fileName_ = other->fileName_;
	texture_ = other->texture_;
	width_ = other->width_;
	height_ = other->height_;
	pixelDraw_ = other->pixelDraw_;

	other->fileName_ = tempFileName;
	other->texture_ = tempTexture;
	other->width_ = tempWidth;
	other->height_ = tempHeight;
	other->pixelDraw_ = tempPixelDraw;

}

void Image::referencedFiles(std::vector<std::string> * refFiles) const
{
	SceneObject::referencedFiles(refFiles);
	refFiles->push_back(fileName_);
}

int Image::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);
	fileName_ = refFiles[index++];
	return index;
}


void Image::setFileName(const std::string & fileName, bool resetDims)
{
	fileName_ = fileName;
	if (resetDims)
	{
		width_ = -1;
		height_ = -1;
	}
}

void Image::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
	texture_->init(gl, fileName_, format_);	
	//TODO remove this when older versions are phased out
	if (texture_->isLoaded())
	{
		if (width_ < 0) width_ = texture_->width();
		if (height_ < 0) height_ = texture_->height();
	}
}

void Image::uninit()
{
	texture_->uninit();	
	SceneObject::uninit();
}


bool Image::update(float sceneTime)
{	
	bool needRedraw = SceneObject::update(sceneTime);
	//TODO right now this always returns true while image is loading
	//it should return true right at the point image loading completes
	needRedraw |= !texture_->isFullyLoaded();
	
	return needRedraw;
}

bool Image::asyncLoadUpdate()
{
	bool everythingLoaded = SceneObject::asyncLoadUpdate();
	if (!texture_->isFullyLoaded())
	{
		texture_->asyncLoadUpdate();				
	}
	
	if (texture_->isLoaded())
	{		
		//TODO remove this when older versions are phased out
		if (width_ < 0) width_ = texture_->width();
		if (height_ < 0) height_ = texture_->height();
	}

	everythingLoaded &= texture_->isLoaded();
	handleLoadedEventDispatch(everythingLoaded);
	return everythingLoaded & texture_->isLoaded();	
}

bool Image::isLoaded() const
{
	return texture_->isLoaded();
}

void Image::drawObject(GfxRenderer * gl) const
{
	float fOpacity = totalOpacity();
	if (fOpacity == 0) return;

	if (!texture_->isLoaded()) return;
	if (pixelDraw_)
	{
		Transform t;
		//pixel = Global::instance().pixelDraw();
        Vector3 scenescale = parentScene()->currentScale();
		float dpiScale = parentScene()->parentDocument()->documentTemplate()->dpiScale();
		float mag = 1.0 / scenescale.x * Global::instance().magnification() * dpiScale;
		float movingX = (width_ / mag - width_)/2 * mag;
		float movingY = (height_ / mag - height_)/2 * mag;
		t.setTranslation(movingX, movingY, 0);
		t.setScaling(mag, mag,1);
		gl->pushMatrix();
		gl->multMatrix(t.computeMatrix());	
	}

	if (texture_->needSeparateAlpha())
	{
		gl->useTextureAlphaProgram();
		gl->setTextureAlphaProgramOpacity(fOpacity);		
	}
	else
	{
		gl->useTextureProgram();
		gl->setTextureProgramOpacity(fOpacity);				
	}

	gl->use(texture_);
	
	gl->drawRect(0, 0, width(), height());
	if (pixelDraw_)
		gl->popMatrix();
}


bool Image::getBitsFromPixelBuffer(PixelBuffer * pixelBuffer)
{
	if (texture_) return texture_->getBitsFromPixelBuffer(pixelBuffer);		
	return false;
}

PixelBuffer * Image::createPixelBufferFromImage()
{
	if (!texture_) return NULL;
	if (!gl_) return NULL;
	
	unsigned char * bits = (unsigned char *)malloc(texture_->width() * texture_->height() * 4);
	GLuint frameBuffer;

	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_->glTexture(), 0);
	glReadPixels(0, 0, texture_->width(), texture_->height(), GL_RGBA, GL_UNSIGNED_BYTE, bits);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &frameBuffer);

	PixelBuffer * pixelBuffer = new PixelBuffer(texture_->width(), texture_->height());
	pixelBuffer->setBits(bits);

	free(bits);
	return pixelBuffer;	
}

BoundingBox Image::extents() const
{
	return BoundingBox(
		Vector3(0, 0, 0), 
		Vector3((float)width(), (float)height(), 0));
}

void Image::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");	
	
	writer.write(fileName_, "fileName");

	writer.write(format_, "format");

	writer.write(width_, "width");
	writer.write(height_, "height");
	
	writer.write(pixelDraw_, "pixelDraw");
}

void Image::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);	
	reader.read(fileName_);
	
	if (version >= 4)
	{
		reader.read(format_);
	}
	if (version >= 3)
	{
		reader.read(width_);
		reader.read(height_);
	}
	else
	{
		width_ = -1;
		height_ = -1;
		if (version == 2)
		{
			float overrideWidth, overrideHeight;
			reader.read(overrideWidth);
			reader.read(overrideHeight);
			
			if (overrideWidth > 0) width_ = overrideWidth;
			if (overrideHeight > 0) height_ = overrideHeight;		
		}		
	}
	if (version >= 5)
	{
		reader.read(pixelDraw_);
	}
}

void Image::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();
	
	w.writeTag("FileName", fileName_);
	w.writeTag("Format", (unsigned int)format_);
	w.writeTag("Width", width_);
	w.writeTag("Height", height_);
	w.writeTag("PixelDraw", pixelDraw_);	
}

void Image::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);

	unsigned int val;

	for (xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
		}
		else if (r.getNodeContentIfName(fileName_, curNode, "FileName"));
		else if (r.getNodeContentIfName(val, curNode, "Format"))
		{			
			format_ = (Texture::Format)val;
		}
		else if (r.getNodeContentIfName(width_, curNode, "Width"));
		else if (r.getNodeContentIfName(height_, curNode, "Height"));
		else if (r.getNodeContentIfName(pixelDraw_, curNode, "PixelDraw"));
	}
}

SceneObject * Image::intersect(Vector3 * out, const Ray & ray)
{
	if (!visualAttrib_.isVisible()) return 0;
	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();

	float movingX = 0;
	float movingY = 0;
	float mag = 1.0;

	float w = width();
	float h = height();
	if (pixelDraw_)
	{
		float dpiScale = parentScene()->parentDocument()->documentTemplate()->dpiScale();
        Vector3 scenescale = parentScene()->currentScale();
		mag = 1.0 / scenescale.x * Global::instance().magnification() * dpiScale;
		movingX = (width_ / mag - width_)/4;
		movingY = (height_ / mag - height_)/4;
	}

	Vector3 a = trans * Vector3(movingX, movingY, 0);
	Vector3 b = trans * Vector3(movingX, h * mag + movingY, 0);
	Vector3 c = trans * Vector3(w * mag + movingX, h * mag + movingY, 0);
	Vector3 d = trans * Vector3(w * mag + movingX, movingY, 0);

	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return this;
	}
	else return 0;
}
void Image::requestLocalSaveObject()
{
	if(fileName_ != "")
		texture_->requestLocalSaveObject(fileName_);
}

void Image::requestZoom()
{
	if (!pixelDraw_)
		texture_->requestZoom();
}

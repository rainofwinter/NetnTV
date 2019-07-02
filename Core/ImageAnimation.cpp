#include "stdafx.h"
#include "Subscene.h"
#include "ImageAnimation.h"
#include "Image.h"
#include "Texture.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Xml.h"
#include "Scene.h"
#include "Reader.h"
#include "Camera.h"
#include "GLUtils.h"
#include "ElementMapping.h"
#include "FileUtils.h"
#include "Global.h"
#include "Document.h"
#include "LoadingPage.h"
#include "PageChangedEvent.h"
#include "ScriptProcessor.h"

using namespace std;

JSClass jsImageAnimationClass = InitClass(
	"ImageAnimation", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);


JSBool ImageAnimation_play(JSContext *cx, uintN argc, jsval *vp)
{
	ImageAnimation * thisObj = (ImageAnimation *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	thisObj->play();
	return JS_TRUE;
};

JSBool ImageAnimation_stop(JSContext *cx, uintN argc, jsval *vp)
{
	ImageAnimation * thisObj = (ImageAnimation *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	thisObj->stop();
	return JS_TRUE;
};

JSBool ImageAnimation_reset(JSContext *cx, uintN argc, jsval *vp)
{
	ImageAnimation * thisObj = (ImageAnimation *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	thisObj->reset();
	return JS_TRUE;
};

JSFunctionSpec ImageAnimationFuncs[] = {
	JS_FS("play", ImageAnimation_play, 0, 0),
	JS_FS("reset", ImageAnimation_reset, 0, 0),
	JS_FS("stop", ImageAnimation_stop, 0, 0),
	JS_FS_END
};

///////////////////////////////////////////////////////////////////////////////

JSObject * ImageAnimation::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsImageAnimationClass,
		0, 0, 0, ImageAnimationFuncs, 0, 0);

	return proto;
}

JSObject * ImageAnimation::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsImageAnimationClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void ImageAnimation::create()
{
	width_ = 256;
	height_ = 256;		
	fps_ = 10.0f;

	startTime_ = -1.0f;
	curFrameIndex_ = -1;

	repeat_ = false;
	format_ = Texture::UncompressedRgba32;
	maxMemFrames_ = 24;

	pause_ = false;
	puaseGapTime_ = -1;
}

ImageAnimation::ImageAnimation()
{
	create();
	setId("ImageAnimation");
}

ImageAnimation::ImageAnimation(
	const ImageAnimation & rhs, ElementMapping * elementMapping)
	: SceneObject(rhs, elementMapping)
{
	create();
	visualAttrib_ = rhs.visualAttrib_;
	width_ = rhs.width_;
	height_ = rhs.height_;
	fps_ = rhs.fps_;
	repeat_ = rhs.repeat_;
	maxMemFrames_ = rhs.maxMemFrames_;
	format_ = rhs.format_;

	BOOST_FOREACH(string fileName, rhs.fileNames_)
		fileNames_.push_back(fileName);			
}

ImageAnimation::~ImageAnimation()
{
	uninit();
}

void ImageAnimation::setFileNames(const std::vector<std::string> & fileNames)
{	
	fileNames_ = fileNames;	
	init(gl_);
}

void ImageAnimation::play()
{
	pause_ = false;
	startTime_ = Global::currentTime();
}

void ImageAnimation::stop()
{
	pause_ = false;
	startTime_ = -1.0f;
}

void ImageAnimation::reset()
{
	if (startTime_ >= 0.0f) startTime_ = Global::currentTime();
	setFrame(0);	
	if (parentScene_)
	{
		Document * doc = parentScene_->parentDocument();
		if (doc) doc->triggerRedraw();
	}
}

void ImageAnimation::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
	float startTime = Global::currentTime();

	initBuffers();
	for (int i = 0; i < (int)textures_.size(); ++i)
		if (refCount_[i] && !textures_[i]->isInit()) textures_[i]->init(gl, format_);
}

void ImageAnimation::uninit()
{		
	if (!gl_) return;

	BOOST_FOREACH(TextureSPtr & texture, textures_) texture->uninit();
	BOOST_FOREACH(unsigned char & count, refCount_) count = 0;
	
	SceneObject::uninit();
}

bool ImageAnimation::update(float sceneTime)
{
	bool needRedraw = false;
	needRedraw |= SceneObject::update(sceneTime);

	if (pause_) return false;
	
	//Note: sceneTime may not be fully current, it's possible for sceenTime < 
	//startTime_. Compute curTime again instead.
	
	float curTime = Global::currentTime();

	if (startTime_ >= 0.0f)
	{
		float animTime = curTime - startTime_;
		int frameIndex = (int)(animTime * fps_);

		int numFiles = (int)fileNames_.size();		
		if (!repeat_ && frameIndex >= numFiles) frameIndex = numFiles - 1;
		needRedraw |= setFrame(frameIndex);
	}
	
	return needRedraw;
}

void ImageAnimation::initBuffers()
{			

	int numFiles = (int)fileNames_.size();
	textures_.resize(numFiles);
	refCount_.resize(numFiles);
	
	for (int i = 0; i < numFiles; ++i)
	{
		textures_[i].reset(new Texture);
		textures_[i]->setFileName(fileNames_[i]);
		refCount_[i] = 0;
	}

	curFrameIndex_ = -1;
	curDisplayIndex_ = -1;

	setFrame(0);
}

bool ImageAnimation::setFrame(int pframeIndex)
{	
	int numFiles = (int)fileNames_.size();

	
	int newFrameIndex = pframeIndex % numFiles;

	int upperBound = maxMemFrames_;
	if (maxMemFrames_ > numFiles) upperBound = numFiles;
	
	for (int i = newFrameIndex; i < newFrameIndex + upperBound; ++i) refCount_[i % numFiles]++;
	
	int prevFrameIndex = curFrameIndex_;

	if (prevFrameIndex >= 0)
	{
		for (int i = prevFrameIndex; i < prevFrameIndex + upperBound; ++i) 
		{
			int index = i % numFiles;
			if (refCount_[index]) refCount_[index]--;
		}

		for (int i = prevFrameIndex; i < prevFrameIndex + upperBound; ++i)
		{			
			int index = i % numFiles;
			if (refCount_[index] == 0 && curDisplayIndex_ != index) 
			{
				if(!textures_.empty()) textures_[index]->uninit();	
			}
		}
	}
	
	for (int i = newFrameIndex; i < newFrameIndex + upperBound; ++i)
	{
		int index = i % numFiles;
		if(!refCount_.empty() && !textures_.empty())
		{
			if (refCount_[index] && !textures_[index]->isInit()) 
				textures_[index]->init(gl_);	
		}
	}


	curFrameIndex_ = newFrameIndex;
	return true;
}

bool ImageAnimation::asyncLoadUpdate() 
{
	bool everythingLoaded = SceneObject::asyncLoadUpdate();

	for (int i = 0; i < (int)textures_.size(); ++i)
	{
		if (refCount_[i]) 
		{
			textures_[i]->asyncLoadUpdate();
			everythingLoaded &= textures_[i]->isLoaded();
		}
	}
	if(!textures_.empty())
	{
		if (textures_[curFrameIndex_]->isLoaded())
		{
			curDisplayIndex_ = curFrameIndex_;
		}
	}
	return everythingLoaded;
}

bool ImageAnimation::isLoaded() const
{
	if (!gl_) return false;
	bool ret = true;

	for (int i = 0; i < (int)textures_.size(); ++i)
	{
		if (refCount_[i]) ret &= textures_[i]->isLoaded();			
	}
	
	return ret;
}

void ImageAnimation::drawObject(GfxRenderer * gl) const
{
	if (textures_.empty()) return;
	if (textures_[0]->needSeparateAlpha())
	{
		gl->useTextureAlphaProgram();
		gl->setTextureAlphaProgramOpacity(visualAttrib_.opacity());		
	}
	else
	{
		gl->useTextureProgram();
		gl->setTextureProgramOpacity(visualAttrib_.opacity());				
	}	

	if (curDisplayIndex_ >= 0)
	{
		gl->use(textures_[curDisplayIndex_].get());
		gl->drawRect(0, 0, width_, height_);	
	}
	
}

void ImageAnimation::start(float docTime)
{
	SceneObject::start(docTime);	
}

void ImageAnimation::preStart(float docTime)
{
	SceneObject::preStart(docTime);
}

BoundingBox ImageAnimation::extents() const
{
	return BoundingBox(Vector3(0, 0, 0), Vector3(width_, height_, 0));
}


SceneObject * ImageAnimation::intersect(Vector3 * intPt, const Ray & ray)
{
	if (!visualAttrib_.isVisible()) return 0;
	if (intersectRect(intPt, ray)) return this;
	else return 0;
}

bool ImageAnimation::intersectRect(Vector3 * out, const Ray & ray)
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

void ImageAnimation::remapReferences(const ElementMapping & elementMapping)
{
	SceneObject::remapReferences(elementMapping);
}


void ImageAnimation::referencedFiles(std::vector<std::string> * refFiles) const
{	
	SceneObject::referencedFiles(refFiles);

	BOOST_FOREACH(std::string fileName, fileNames_)
		refFiles->push_back(fileName);
}

int ImageAnimation::setReferencedFiles(
	const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);

	BOOST_FOREACH(std::string & fileName, fileNames_)
		fileName = refFiles[index++];

	return index;
}

bool ImageAnimation::dependsOn(Scene * scene) const
{
	return false;
}


void ImageAnimation::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");	
	writer.write(width_, "width");
	writer.write(height_, "height");
	writer.write(fileNames_, "fileNames");
	writer.write(fps_);
	writer.write(repeat_);
	writer.write(maxMemFrames_);
	writer.write(format_);
}

void ImageAnimation::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);
	reader.read(width_);
	reader.read(height_);
	
	reader.read(fileNames_);
	refCount_.resize(fileNames_.size());
	for (int i = 0; i < (int)fileNames_.size(); ++i) refCount_[i] = 0;

	reader.read(fps_);
	reader.read(repeat_);
	if (version >= 1)
	{
		reader.read(maxMemFrames_);
		reader.read(format_);
		format_ = Texture::UncompressedRgba32;
	}
}


void ImageAnimation::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();

	w.writeTag("Width", width_);
	w.writeTag("Height", height_);
	for (unsigned int i = 0; i < (unsigned int)fileNames_.size(); ++i)
		w.writeTag("FileName", fileNames_[i]);
	
	w.writeTag("Fps", fps_);
	w.writeTag("Repeat", repeat_);
	w.writeTag("MaxMemFrames", maxMemFrames_);
	int temp = format_;
	w.writeTag("Format", temp);
}

void ImageAnimation::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);
	int temp = 0;
	std::string filename;
	fileNames_.clear();

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{
		if (curNode->type != XML_ELEMENT_NODE) continue;		
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(width_, curNode, "Width"));
		else if (r.getNodeContentIfName(height_, curNode, "Height"));
		else if (r.getNodeContentIfName(filename, curNode, "FileName")) fileNames_.push_back(filename);
		else if (r.getNodeContentIfName(fps_, curNode, "Fps"));
		else if (r.getNodeContentIfName(repeat_, curNode, "Repeat"));
		else if (r.getNodeContentIfName(maxMemFrames_, curNode, "MaxMemFrames"));
		else if (r.getNodeContentIfName(temp, curNode, "Format")) format_ = (Texture::Format)temp;
	}
}

void ImageAnimation::pause()
{
	pause_ = true;
	if (puaseGapTime_ < 0)
		puaseGapTime_ = Global::currentTime() - startTime_;
}

void ImageAnimation::resume()
{
	pause_ = false;
	if (puaseGapTime_ > 0)
	{
		startTime_ = Global::currentTime() - puaseGapTime_;
		puaseGapTime_ = -1;
	}
}
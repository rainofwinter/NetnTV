#include "stdafx.h"
#include "VideoObject.h"
#include "ScriptProcessor.h"

#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "Global.h"
#include "FileUtils.h"
#include "GfxRenderer.h"

#include "EventListener.h"
#include "Event.h"
#include "VideoPlayer.h"
#include "VideoObjectPlayedEvent.h"
#include "VideoObjectStoppedEvent.h"
#include "VideoPlayTimeEvent.h"

#include "Document.h"
#include "Scene.h"

using namespace std;

JSClass jsVideoClass = InitClass(
	"Video", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);

JSBool Video_play(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	VideoObject * thisObj = (VideoObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));	
	thisObj->play(0.0f);
	return JS_TRUE;
}

JSBool Video_stop(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	VideoObject * thisObj = (VideoObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));	
	thisObj->stop();
	return JS_TRUE;
}

JSBool Video_setPlayTime(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	VideoObject * thisObj = (VideoObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsTime;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsTime)) return JS_FALSE;
	thisObj->play((float)jsTime);
	return JS_TRUE;
}

JSBool Video_getPlayTime(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	VideoObject * thisObj = (VideoObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	double time =(double)thisObj->currentTime();
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(time));
	return JS_TRUE;
}

JSFunctionSpec VideoFuncs[] = {
	JS_FS("play", Video_play, 0, 0),
	JS_FS("stop", Video_stop, 0, 0),
	JS_FS("setPlayTime", Video_setPlayTime, 1, 0),
	JS_FS("getPlayTime", Video_getPlayTime, 0, 0),
    JS_FS_END
};


///////////////////////////////////////////////////////////////////////////////

JSObject * VideoObject::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsVideoClass,
		0, 0, 0, VideoFuncs, 0, 0);

	return proto;
}

JSObject * VideoObject::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsVideoClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void VideoObject::create()
{
	isPlaying_ = false;
	x_ = 0;
	y_ = 0;
	width_ = 320;
	height_ = 200;

	sourceType_ = File;
	playInRegion_ = true;

	showPlaybackControls_ = false;

	startTime_ = -1.0f;
}

VideoObject::VideoObject()
{
	create();
	setId("Video");
}

VideoObject::VideoObject(const VideoObject & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();

	source_ = rhs.source_;
	sourceType_ = rhs.sourceType_;
	playInRegion_ = rhs.playInRegion_;
	showPlaybackControls_ = rhs.showPlaybackControls_;
	x_ = rhs.x_;
	y_ = rhs.y_;
	width_ = rhs.width_;
	height_ = rhs.height_;
}


VideoObject::~VideoObject()
{
	Global::instance().unregisterPtr(this);			
}

void VideoObject::referencedFiles(std::vector<std::string> * refFiles) const
{
	SceneObject::referencedFiles(refFiles);
	if (sourceType_ == File) refFiles->push_back(source_);
}

int VideoObject::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);
	if (sourceType_ == File) source_ = refFiles[index++];
	return index;
}


void VideoObject::setSource(const std::string & source)
{
	source_ = source;
}

void VideoObject::setSourceNames(const std::vector<std::string> & sourceNames)
{
	sourceName_ = sourceNames;
}

void VideoObject::setSourceMode(const std::vector<int> & sourceMode)
{
	sourceMode_ = sourceMode;
}

void VideoObject::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
}

void VideoObject::uninit()
{
	SceneObject::uninit();
}


bool VideoObject::update(float sceneTime)
{	
	bool needRedraw = SceneObject::update(sceneTime);
	static boost::uuids::uuid playTimeEventType = VideoPlayTimeEvent().type();

	if (isPlaying_)
	{
		float curPlayTime = -FLT_MAX;
		BOOST_FOREACH(EventListenerSPtr listener, eventListeners_)
		{			
			if (listener->event()->type() == playTimeEventType)
			{
				if (curPlayTime == -FLT_MAX)
					curPlayTime = Global::instance().videoPlayer()->currentTime();

				VideoPlayTimeEvent * playTimeEvent = (VideoPlayTimeEvent *)listener->event();
				
				if (firedPlayEventTimes_.find(playTimeEvent->time()) == firedPlayEventTimes_.end())
				{
					if (curPlayTime >= playTimeEvent->time() && startTime_ <= playTimeEvent->time())
					{
						needRedraw |= handleEvent(playTimeEvent, Global::currentTime());
						firedPlayEventTimes_.insert(playTimeEvent->time());
					}
				}
				
			}
		}
	}

	return needRedraw;
}

void VideoObject::notifyStopped()
{
	if (!isPlaying_) return;
	isPlaying_ = false;
	VideoObjectStoppedEvent event;
	event.target = this;
	if (handleEvent(&event, Global::currentTime()))
		parentScene_->parentDocument()->triggerRedraw();
	parentScene_->stopSyncedAnimations(this);	
}

void VideoObject::notifyPaused()
{
	if (!isPlaying_) return;
	isPlaying_ = false;
	parentScene_->stopSyncedAnimations(this);
	
}

void VideoObject::notifyResumed()
{
	if (isPlaying_) return;
	isPlaying_ = true;
	parentScene_->resumeSyncedAnimations(this);
}

void VideoObject::notifyPlayed()
{
	if (isPlaying_) return;
	isPlaying_ = true;
	firedPlayEventTimes_.clear();
	VideoObjectPlayedEvent event;
	event.target = this;
	if (handleEvent(&event, Global::currentTime()))
		parentScene_->parentDocument()->triggerRedraw();
	parentScene_->playSyncedAnimations(this);
}

void VideoObject::notifySeeked(float time)
{
	parentScene_->seekSyncedAnimations(this, time);

	static boost::uuids::uuid playTimeEventType = VideoPlayTimeEvent().type();
	firedPlayEventTimes_.clear();
	BOOST_FOREACH(EventListenerSPtr listener, eventListeners_)
	{			
		if (listener->event()->type() == playTimeEventType)
		{
			VideoPlayTimeEvent * playTimeEvent = (VideoPlayTimeEvent *)listener->event();

			if (playTimeEvent->time() < time) 
				firedPlayEventTimes_.insert(playTimeEvent->time());		
		}
	}
}

float VideoObject::currentTime()
{
	currentTime_ =  Global::instance().videoPlayer()->currentTime();
	return currentTime_;
}


bool VideoObject::asyncLoadUpdate()
{
	return SceneObject::asyncLoadUpdate();
}

bool VideoObject::isLoaded() const
{
	return SceneObject::isLoaded();
}

void VideoObject::drawObject(GfxRenderer * gl) const
{
	gl->useTextureProgram();
	gl->setTextureProgramFactorST(1, 1);
	SceneObject::drawObject(gl);
}


BoundingBox VideoObject::extents() const
{/*
	const float cEpsilon = 0.0001f;
	return BoundingBox(Vector3(-cEpsilon, -cEpsilon, -cEpsilon), Vector3(cEpsilon, cEpsilon, cEpsilon));
	*/
	return BoundingBox();
}

void VideoObject::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(sourceName_);
	writer.write(sourceMode_);
	writer.write(source_);
	writer.write(sourceType_);
	writer.write(playInRegion_);
	writer.write(x_);
	writer.write(y_);
	writer.write(width_);
	writer.write(height_);
	writer.write(showPlaybackControls_);	
}

void VideoObject::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	for (unsigned int i = 0; i < (unsigned int)sourceName_.size(); ++i)
		w.writeTag("SourceName", sourceName_[i]);
	for (unsigned int i = 0; i < (unsigned int)sourceMode_.size(); ++i)
		w.writeTag("SourceMode", sourceMode_[i]);
	w.writeTag("Source", source_);
	w.writeTag("SourceType", (unsigned int)sourceType_);
	w.writeTag("PlayInRegion", playInRegion_);
	w.writeTag("X", x_);
	w.writeTag("Y", y_);
	w.writeTag("Width", width_);
	w.writeTag("Height", height_);
	w.writeTag("ShowPlaybackControls", showPlaybackControls_);
}

void VideoObject::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);
	std::string str;
	int temp;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(str, curNode, "SourceName")) sourceName_.push_back(str);
		else if (r.getNodeContentIfName(temp, curNode, "SourceMode")) sourceMode_.push_back(temp);
		else if (r.getNodeContentIfName(source_, curNode, "Source"));
		else if (r.getNodeContentIfName(temp, curNode, "SourceType")) sourceType_ = (SourceType)temp;
		else if (r.getNodeContentIfName(x_, curNode, "X"));
		else if (r.getNodeContentIfName(y_, curNode, "Y"));
		else if (r.getNodeContentIfName(width_, curNode, "Width"));
		else if (r.getNodeContentIfName(height_, curNode, "Height"));
		else if (r.getNodeContentIfName(showPlaybackControls_, curNode, "ShowPlaybackControls"));
	}
}

void VideoObject::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	if (version >= 1)
	{
		reader.read(sourceName_);
		reader.read(sourceMode_);
	}
	reader.read(source_);
	reader.read(sourceType_);
	reader.read(playInRegion_);
	reader.read(x_);
	reader.read(y_);
	reader.read(width_);
	reader.read(height_);
	reader.read(showPlaybackControls_);
}


SceneObject * VideoObject::intersect(Vector3 * out, const Ray & ray)
{
	if (isPlaying_)
	{
		Matrix trans = parentTransform();//visualAttrib_.transformMatrix();

		Vector3 a = trans * Vector3(x_, y_, 0);
		Vector3 b = trans * Vector3(x_, height_ + y_, 0);
		Vector3 c = trans * Vector3(width_ + x_, height_ + y_, 0);
		Vector3 d = trans * Vector3(width_ + x_, y_, 0);

		if (triangleIntersectRay(out, a, b, c, ray) ||
			triangleIntersectRay(out, a, c, d, ray))
		{
			return this;
		}
		else
			return 0;
	}
	else
		return 0;
		//return SceneObject::intersect(out, ray);
}


void VideoObject::play(float startTime)
{
	Scene * scene = parentScene();
	Document * document = scene->parentDocument();
	if (!scene->isCurrentScene()) return;

	ContainerObjects objs;
	scene->getContainerObjects(&objs);

	if (sourceType_ == Url)
	{
		for (int i = 0 ; i < sourceMode_.size() ; i ++)
		{
#if defined ANDROID
			if (sourceMode_[i] == 1)
				source_ = sourceName_[i];
#elif defined IOS
			if (sourceMode_[i]== 2)
				source_ = sourceName_[i];
#elif defined WIN32
			if (sourceMode_[i] == 3)
				source_ = sourceName_[i];
#endif
			if (sourceMode_[i] == 0)
				source_ = sourceName_[i];
		}
	}

	if (playInRegion_)
	{
		document->playVideo(this, objs, source_,
			x_, y_, width_, height_, sourceType_,
			showPlaybackControls_, startTime);
	}
	else
	{
		document->playVideoFullscreen(this, objs, source_, 
			sourceType_, showPlaybackControls_, startTime);
	}

	startTime_ = startTime;

}

void VideoObject::stop()
{
	Global::instance().videoPlayer()->deactivate();
}
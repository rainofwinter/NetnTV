#include "stdafx.h"
#include "AudioObject.h"
#include "ScriptProcessor.h"

#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "Global.h"
#include "FileUtils.h"

#include "EventListener.h"
#include "Event.h"
#include "AudioPlayer.h"
#include "AudioPlayTimeEvent.h"
#include "AudioPlayedEvent.h"
#include "AudioStoppedEvent.h"
#include "AudioResumedEvent.h"
#include "AudioPausedEvent.h"

#include "Document.h"
#include "Scene.h"

using namespace std;

JSClass jsAudioClass = InitClass(
	"Audio", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);

JSBool Audio_setVolume(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	AudioObject * thisObj = (AudioObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsVolume;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsVolume)) return JS_FALSE;
	thisObj->setVolume((float)jsVolume);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Audio_getVolume(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	AudioObject * thisObj = (AudioObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));	
	double volume = (double)thisObj->volume();
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(volume));
	return JS_TRUE;
}

JSBool Audio_play(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	AudioObject * thisObj = (AudioObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));	
	thisObj->play(0.0f);
	return JS_TRUE;
}

JSBool Audio_stop(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	AudioObject * thisObj = (AudioObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));	
	thisObj->stop();
	return JS_TRUE;
}

JSBool Audio_pause(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	AudioObject * thisObj = (AudioObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));	
	thisObj->pause();
	return JS_TRUE;
}

JSBool Audio_resume(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	AudioObject * thisObj = (AudioObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));	
	thisObj->resume();
	return JS_TRUE;
}

JSBool Audio_setPlayTime(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	AudioObject * thisObj = (AudioObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsTime;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsTime)) return JS_FALSE;
	thisObj->play((float)jsTime);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Audio_getPlayTime(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	AudioObject * thisObj = (AudioObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	double time =(double)thisObj->currentTime();
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(time));
	return JS_TRUE;
}


JSFunctionSpec AudioFuncs[] = {
	JS_FS("setVolume", Audio_setVolume, 1, 0),
	JS_FS("getVolume", Audio_getVolume, 0, 0),
	JS_FS("play", Audio_play, 0, 0),
	JS_FS("stop", Audio_stop, 0, 0),
	JS_FS("pause", Audio_pause, 0, 0),
	JS_FS("resume", Audio_resume, 0, 0),
	JS_FS("setPlayTime", Audio_setPlayTime, 1, 0),
	JS_FS("getPlayTime", Audio_getPlayTime, 0, 0),
    JS_FS_END
};


///////////////////////////////////////////////////////////////////////////////

JSObject * AudioObject::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsAudioClass,
		0, 0, 0, AudioFuncs, 0, 0);

	return proto;
}

JSObject * AudioObject::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsAudioClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void AudioObject::create()
{
	isPlaying_ = false;
	repeat_ = false;
	sourceType_ = File;
	volume_ = 1.0f;
}

AudioObject::AudioObject()
{
	create();
	setId("Audio");
}

AudioObject::AudioObject(const AudioObject & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();
	fileName_ = rhs.fileName_;
	repeat_ = rhs.repeat_;
}


AudioObject::~AudioObject()
{
	Global::instance().unregisterPtr(this);		
}

void AudioObject::referencedFiles(std::vector<std::string> * refFiles) const
{
	SceneObject::referencedFiles(refFiles);
	refFiles->push_back(fileName_);
}

int AudioObject::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);
	fileName_ = refFiles[index++];
	return index;
}



void AudioObject::setFileName(const std::string & fileName)
{
	fileName_ = fileName;
}

void AudioObject::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
}

void AudioObject::uninit()
{
	SceneObject::uninit();
}


bool AudioObject::update(float sceneTime)
{	
	bool needRedraw = SceneObject::update(sceneTime);
	static boost::uuids::uuid playTimeEventType = AudioPlayTimeEvent().type();

	if (isPlaying_)
	{
		float curPlayTime = -FLT_MAX;
		BOOST_FOREACH(EventListenerSPtr listener, eventListeners_)
		{			
			if (listener->event()->type() == playTimeEventType)
			{
				if (curPlayTime == -FLT_MAX)
					curPlayTime = Global::instance().audioPlayer()->currentTime(Global::instance().ptrToId((void *)this));

				AudioPlayTimeEvent * playTimeEvent = (AudioPlayTimeEvent *)listener->event();

				//////////////////////////////////////////////////////////////
				if (firedPlayEventTimes_.find(playTimeEvent->time()) == firedPlayEventTimes_.end())
				{
					if (curPlayTime >= playTimeEvent->time() && playTimeEvent->time() >= playTime_)
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

void AudioObject::setVolume(float volume)
{
	volume_ = volume;
	Global::instance().audioPlayer()->setVolume(Global::instance().ptrToId(this), volume);
}

void AudioObject::play(float startTime)
{
	AudioPlayer * audioPlayer = Global::instance().audioPlayer();
	if (sourceType_ == AudioObject::File)
	{
		string absFileName = getAbsFileName(Global::instance().readDirectory(), fileName_);
		audioPlayer->play(Global::instance().ptrToId((void *)this), absFileName, repeat_, startTime);
	}
	else if (sourceType_ == AudioObject::Recorded)
		audioPlayer->playRecorded(Global::instance().ptrToId((void *)this), repeat_);
}

void AudioObject::stop()
{
	Global::instance().audioPlayer()->stop(Global::instance().ptrToId((void *)this));
}

void AudioObject::pause()
{
	Global::instance().audioPlayer()->pause(Global::instance().ptrToId((void *)this));
}

void AudioObject::resume()
{
	Global::instance().audioPlayer()->resume(Global::instance().ptrToId((void *)this));
}

void AudioObject::notifyStopped()
{
	if (!isPlaying_) return;
	isPlaying_ = false;
	AudioStoppedEvent event;
	event.target = this;
	if (handleEvent(&event, Global::currentTime()))
		parentScene_->parentDocument()->triggerRedraw();
	parentScene_->stopSyncedAnimations(this);	
}

void AudioObject::notifyPaused()
{
	if (!isPlaying_) return;
	isPlaying_ = false;
	AudioPausedEvent event;
	event.target = this;
	if (handleEvent(&event, Global::currentTime()))
		parentScene_->parentDocument()->triggerRedraw();
	parentScene_->stopSyncedAnimations(this);
}

void AudioObject::notifyResumed()
{
	if (isPlaying_) return;
	isPlaying_ = true;
	AudioResumedEvent event;
	event.target = this;
	if (handleEvent(&event, Global::currentTime()))
		parentScene_->parentDocument()->triggerRedraw();
	parentScene_->resumeSyncedAnimations(this);
}

void AudioObject::notifySeeked(float time)
{
	parentScene_->seekSyncedAnimations(this, time);
}

void AudioObject::notifyPlayed()
{  
	playTime_ = this->currentTime();
//	if (isPlaying_) return;
	isPlaying_ = true;
	firedPlayEventTimes_.clear();
	AudioPlayedEvent event;
	event.target = this;
	if (handleEvent(&event, Global::currentTime()))
		parentScene_->parentDocument()->triggerRedraw();
	parentScene_->playSyncedAnimations(this);
}


bool AudioObject::asyncLoadUpdate()
{
	return SceneObject::asyncLoadUpdate();
}

bool AudioObject::isLoaded() const
{
	return SceneObject::isLoaded();
}

void AudioObject::drawObject(GfxRenderer * gl) const
{
	SceneObject::drawObject(gl);
}

float AudioObject::currentTime()
{
	currnetTime_ = Global::instance().audioPlayer()->currentTime(Global::instance().ptrToId((void *)this));
	return currnetTime_;
}

BoundingBox AudioObject::extents() const
{/*
	const float cEpsilon = 0.0001f;
	return BoundingBox(Vector3(-cEpsilon, -cEpsilon, -cEpsilon), Vector3(cEpsilon, cEpsilon, cEpsilon));
	*/
	return BoundingBox();
}

void AudioObject::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(sourceType_);
	writer.write(fileName_);
	writer.write(repeat_);
	writer.write(volume_);	
}

void AudioObject::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.writeTag("SourceType", (unsigned int)sourceType_);
	w.writeTag("FileName", fileName_);
	w.writeTag("Repeat", repeat_);
	w.writeTag("Volume", volume_);
}

void AudioObject::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	if (version >= 2)	
		reader.read(sourceType_);
	else
		sourceType_ = File;
	reader.read(fileName_);
	if (version < 1) repeat_ = false;	
	else
		reader.read(repeat_);	

	if (version >= 3) 
		reader.read(volume_);
}

void AudioObject::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);

	unsigned int val;
	
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
	
		if (r.getNodeContentIfName(val, curNode, "SourceType"))
		{
			sourceType_ = (SourceType)val;
		}
		else if (r.getNodeContentIfName(fileName_, curNode, "FileName"));
		else if (r.getNodeContentIfName(repeat_, curNode, "Repeat"));
		else if (r.getNodeContentIfName(volume_, curNode, "Volume"));
	}
}

SceneObject * AudioObject::intersect(Vector3 * out, const Ray & ray)
{
	return SceneObject::intersect(out, ray);
}

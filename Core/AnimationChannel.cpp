#include "stdafx.h"
#include "AnimationChannel.h"
#include "Attrib.h"
#include "SceneObject.h"
#include "Subscene.h"
#include "SceneChanger.h"
#include "ElementMapping.h"
#include "ScriptProcessor.h"
#include "Scene.h"

unsigned char AnimationChannel::version() const
{
	return 0;
}

void AnimationChannel::write(Writer & writer) const
{
	writer.write(object_);
}

void AnimationChannel::read(Reader & reader, unsigned char)
{
	reader.read(object_);
}

void AnimationChannel::setObject(SceneObject * object)
{
	object_ = object;
}

void AnimationChannel::remapReferences(const ElementMapping & mapping)
{
	object_ = mapping.mapObject(object_);
}

JSObject * AnimationChannel::createScriptObject(
	ScriptProcessor * s, const std::vector<jsval> & keyFrameValues) const
{
	JSContext * cx = s->jsContext();
	JSObject * obj = JS_NewObject(s->jsContext(), 0, 0, 0);	

	jsval val;
	val = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, name()));
	JS_SetProperty(cx, obj, "type", &val);

	val = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, object_->id().c_str()));
	JS_SetProperty(cx, obj, "object", &val);

	JSObject * jsKeyFrames = JS_NewArrayObject(cx, (jsint)numKeyFrames(), 0);

	int index = 0;
	BOOST_FOREACH(const jsval & keyFrameValue, keyFrameValues)
	{
		JSObject * jsKeyFrame = JS_NewObject(s->jsContext(), 0, 0, 0);
		val = DOUBLE_TO_JSVAL((double)keyFrame(index)->time());
		JS_SetProperty(cx, jsKeyFrame, "time", &val);
		val = keyFrameValue;
		JS_SetProperty(cx, jsKeyFrame, "value", &val);

		std::string interp = keyFrame(index)->interpolator()->name();
		val = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, interp.c_str()));
		JS_SetProperty(cx, jsKeyFrame, "interp", &val);

		val = OBJECT_TO_JSVAL(jsKeyFrame);
		JS_SetElement(cx, jsKeyFrames, (jsint)index, &val);
		++index;
	}

	val = OBJECT_TO_JSVAL(jsKeyFrames);
	JS_SetProperty(cx, obj, "keys", &val);

	return obj;
}

AnimationChannel * AnimationChannel::fromScriptObject(
	ScriptProcessor * s, Scene * scene, 
	AnimationChannel * channel, JSObject * jsChannel) const
{
	JSContext * cx = s->jsContext();

	std::string objId;
	getPropertyString(cx, jsChannel, "object", &objId);
	channel->object_ = scene->findObject(objId).get();

	jsval val;
	JS_GetProperty(cx, jsChannel, "keys", &val);
	JSObject * jsKeys = JSVAL_TO_OBJECT(val);

	if (jsKeys)
	{
		jsuint jsLen;
		JS_GetArrayLength(cx, jsKeys, &jsLen);

		for (int i = 0; i < (int)jsLen; ++i)
		{		
			JS_GetElement(cx, jsKeys, (jsint)i, &val);
			JSObject * jsKey = JSVAL_TO_OBJECT(val);
			KeyFrame * keyFrame = keyFrameFromScriptObject(s, jsKey);
			channel->insertKeyFrame(keyFrame);		
		}
	}

	return channel;
}


////////////////////////////////////////////////////////////////////////////////
JSObject * TransformChannel::createScriptObject(ScriptProcessor * s) const
{
	std::vector<jsval> keyFrameValues;
	BOOST_FOREACH(const ChannelKeyFrame * keyFrame, this->keyFrames_)
	{
		jsval val = OBJECT_TO_JSVAL(keyFrame->value.createScriptObject(s));
		keyFrameValues.push_back(val);
	}

	return AnimationChannel::createScriptObject(s, keyFrameValues);
}

Transform TransformChannel::keyFrameValueFromScriptObject(
	ScriptProcessor * s, jsval value) const
{
	if (JSVAL_IS_OBJECT(value))
		return Transform::fromJsonScriptObject(s, JSVAL_TO_OBJECT(value));
	return Transform();
}

Vector3 TransformChannel::PosFromValue(Transform & tr) const
{
	Vector3 pos = tr.translation();	
		
	return pos;
}


void TransformChannel::setValue(
	const Transform & a, const Transform & b, float t)
{
	VisualAttrib * attr = object_->visualAttrib();
	attr->setTransform(a.lerp(b, t));
}

Transform TransformChannel::getValue()
{
	return object_->visualAttrib()->transform();
}

bool TransformChannel::supportsObject(SceneObject * obj) const
{
	return obj->visualAttrib() != 0;
}

////////////////////////////////////////////////////////////////////////////////
JSObject * OpacityChannel::createScriptObject(ScriptProcessor * s) const
{
	std::vector<jsval> keyFrameValues;
	BOOST_FOREACH(const ChannelKeyFrame * keyFrame, this->keyFrames_)
	{		
		jsval val = DOUBLE_TO_JSVAL((float)keyFrame->value);
		keyFrameValues.push_back(val);
	}

	return AnimationChannel::createScriptObject(s, keyFrameValues);
}

float OpacityChannel::keyFrameValueFromScriptObject(
	ScriptProcessor * s, jsval value) const
{
	if (JSVAL_IS_DOUBLE(value))	
		return (float)JSVAL_TO_DOUBLE(value);
	else if (JSVAL_IS_INT(value))
		return (float)JSVAL_TO_INT(value);

	return 1.0f;
}
  
Vector3 OpacityChannel::PosFromValue(float & a) const
{	
	return object_->transform().translation();
}

void OpacityChannel::setValue(
	const float & a, const float & b, float t)
{
	VisualAttrib * attr = object_->visualAttrib();
	
	attr->setOpacity(a + t*(b - a));
}

float OpacityChannel::getValue()
{
	return object_->visualAttrib()->opacity();
}

bool OpacityChannel::supportsObject(SceneObject * obj) const
{
	static boost::uuids::uuid sceneChangerType = SceneChanger().type();
	static boost::uuids::uuid subSceneType = Subscene().type();
	return 
		obj->visualAttrib() != 0 && 
		obj->type() != sceneChangerType &&
		obj->type() != subSceneType;
}

////////////////////////////////////////////////////////////////////////////////
JSObject * VisibleChannel::createScriptObject(ScriptProcessor * s) const
{
	std::vector<jsval> keyFrameValues;
	BOOST_FOREACH(const ChannelKeyFrame * keyFrame, this->keyFrames_)
	{		
		jsval val = BOOLEAN_TO_JSVAL(keyFrame->value);
		keyFrameValues.push_back(val);
	}

	return AnimationChannel::createScriptObject(s, keyFrameValues);
}

bool VisibleChannel::keyFrameValueFromScriptObject(
	ScriptProcessor * s, jsval value) const
{
	if (JSVAL_IS_BOOLEAN(value))
		return (bool)JSVAL_TO_BOOLEAN(value);

	return true;
}

Vector3 VisibleChannel::PosFromValue(bool & a) const
{		
	return object_->transform().translation();
}

void VisibleChannel::setValue(
	const bool & a, const bool & b, float t)
{
	VisualAttrib * attr = object_->visualAttrib();
	attr->setVisible((t > 0.5f)?b:a);
}

bool VisibleChannel::getValue()
{
	return object_->visualAttrib()->isVisible();
}

bool VisibleChannel::supportsObject(SceneObject * obj) const
{
	return obj->visualAttrib() != 0;
}

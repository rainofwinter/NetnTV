#include "stdafx.h"
#include "Animation.h"
#include "AnimationChannel.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "SceneObject.h"
#include "Scene.h"
#include "Global.h"

#include "ElementMapping.h"
#include "ScriptProcessor.h"

using namespace std;

JSClass jsAnimationClass = InitClass("Animation", JSCLASS_HAS_PRIVATE, JS_PropertyStub, 
	JS_StrictPropertyStub, 0, 0);

JSBool Animation_setData(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * jsJsonAnim;
	jsval * argv = JS_ARGV(cx, vp);
	Animation * thisObj = (Animation *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsJsonAnim))
        return JS_FALSE;

	thisObj->fromJsonScriptObject(s, jsJsonAnim);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Animation_getData(JSContext *cx, uintN argc, jsval *vp)
{
	Animation * thisObj = (Animation *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsJsonAnim = thisObj->createJsonScriptObject(s);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsJsonAnim));
	return JS_TRUE;
};


JSBool Animation_getId(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Animation * thisObj = (Animation *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "")) return JS_FALSE;
	
	const char * idStr = thisObj->name().c_str();
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, idStr)));
	return JS_TRUE;
};

JSBool Animation_isPlaying(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Animation * thisObj = (Animation *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "")) return JS_FALSE;

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(false));
	Scene * scene = thisObj->parentScene();
	if (scene)
	{
		bool ret = scene->isPlaying(thisObj);
		JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(ret));
	}
	return JS_TRUE;
};

JSBool Animation_setId(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Animation * thisObj = (Animation *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	jschar * jsId;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &jsId)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	std::string id = jsCharStrToStdString(jsId);		
	thisObj->setName(id);
	
	return JS_TRUE;
};

JSBool Animation_addChannel(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * jsJsonChannel;
	jsval * argv = JS_ARGV(cx, vp);
	Animation * thisObj = (Animation *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsJsonChannel))
        return JS_FALSE;

	string type;
	getPropertyString(cx, jsJsonChannel, "type", &type);

	AnimationChannel * temp = Global::instance().createAnimationChannel(type);

	if (temp)
	{
		thisObj->addChannel(AnimationChannelSPtr(
			temp->fromScriptObject(s, thisObj->parentScene(), jsJsonChannel)));
	}

	delete temp;

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSFunctionSpec AnimationFuncs[] = {			
	JS_FS("getData", Animation_getData, 0, 0),
	JS_FS("setData", Animation_setData, 1, 0),

	JS_FS("setId", Animation_setId, 1, 0),
	JS_FS("getId", Animation_getId, 0, 0),

	JS_FS("addChannel", Animation_addChannel, 1, 0),

	JS_FS("isPlaying", Animation_isPlaying, 0, 0),

    JS_FS_END
};

///////////////////////////////////////////////////////////////////////////////
JSObject * Animation::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), 0, &jsAnimationClass,
		0, 0, 0, AnimationFuncs, 0, 0);
	s->rootJSObject(proto);
	return proto;
}

JSObject * Animation::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsAnimationClass, s->animationProto(), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);
	}

	return scriptObject_;
}

JSObject * Animation::createJsonScriptObject(ScriptProcessor * s) const
{	
	JSContext * cx = s->jsContext();
	JSObject * obj = JS_NewObject(cx, 0, 0, 0);

	jsval val;
	val = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, name_.c_str()));
	JS_SetProperty(cx, obj, "id", &val);
	
	val = BOOLEAN_TO_JSVAL(repeat_);
	JS_SetProperty(cx, obj, "repeat", &val);

	int numChannels = 0;
	BOOST_FOREACH(const ObjAnimChannels & c, objChannels_)
	{
		BOOST_FOREACH(const AnimationChannelSPtr & channel, c.channels)
		{
			numChannels++;
		}
	}

	JSObject * jsChannels = JS_NewArrayObject(cx, (jsint)numChannels, 0);

	int index = 0;
	BOOST_FOREACH(const ObjAnimChannels & c, objChannels_)
	{
		BOOST_FOREACH(const AnimationChannelSPtr & channel, c.channels)
		{
			JSObject * jsChannel = JS_NewObject(cx, 0, 0, 0);

			val = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, c.object->id().c_str()));	
			JS_SetProperty(cx, jsChannel, "object", &val);

			val = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, channel->name()));
			JS_SetProperty(cx, jsChannel, "type", &val);

			val = OBJECT_TO_JSVAL(channel->createScriptObject(s));
			JS_SetProperty(cx, jsChannel, "keys", &val);
	
			val = OBJECT_TO_JSVAL(jsChannel);
			JS_SetElement(cx, jsChannels, (jsint)index, &val);
			++index;			
		}
	}

	val = OBJECT_TO_JSVAL(jsChannels);
	JS_SetProperty(cx, obj, "channels", &val);

	return obj;
}


void Animation::fromJsonScriptObject(ScriptProcessor * s, JSObject * obj)
{
	JSContext * cx = s->jsContext();
	objChannels_.clear();

	map<string, SceneObject *> objs;
	map<SceneObject *, vector<AnimationChannelSPtr> > objChannels;


	getPropertyString(cx, obj, "id", &name_);
	getPropertyBool(cx, obj, "repeat", &repeat_);

	jsval val;
	JS_GetProperty(cx, obj, "channels", &val);
	
	if (JSVAL_IS_OBJECT(val) && JS_IsArrayObject(cx, JSVAL_TO_OBJECT(val)))
	{
		JSObject * jsChannels = JSVAL_TO_OBJECT(val);
		
		jsuint jslen;
		JS_GetArrayLength(cx, jsChannels, &jslen);

		for (int i = 0; i < (int)jslen; ++i)
		{
			JS_GetElement(cx, jsChannels, i, &val);
			if (!JSVAL_IS_OBJECT(val)) continue;

			JSObject * jsChannel = JSVAL_TO_OBJECT(val);
			string objId;
			string type;
			getPropertyString(cx, jsChannel, "object", &objId);
			getPropertyString(cx, jsChannel, "type", &type);
			AnimationChannel * temp = 
				Global::instance().createAnimationChannel(type);

			if (objs.find(objId) == objs.end())
			{
				SceneObjectSPtr objSPtr = parentScene_->findObject(objId);
				if (!objSPtr) continue;

				objs[objId] = objSPtr.get();
			}
			SceneObject * animObj = objs[objId];
			objChannels[animObj].push_back(AnimationChannelSPtr(
				temp->fromScriptObject(s, parentScene_, jsChannel)));
			delete temp;
		}
	}


	map<SceneObject *, vector<AnimationChannelSPtr> >::iterator iter;
	for (iter = objChannels.begin(); iter != objChannels.end(); ++iter)
	{
		ObjAnimChannels c;
		c.object = (*iter).first;
		c.channels = (*iter).second;
		objChannels_.push_back(c);
	}

}

Animation::Animation()
{
	setName("Animation");
	repeat_ = false;
	scriptObject_ = 0;
	lastUpdateTime_ = 0;
}

Animation::Animation(const Animation & rhs, ElementMapping * elementMapping)
{
	if (elementMapping)
		elementMapping->addAnimationMapping(const_cast<Animation *>(&rhs), this);

	objChannels_.resize(rhs.objChannels_.size());
	
	for (int i = 0; i < (int)objChannels_.size(); ++i)
	{
		vector<AnimationChannelSPtr> & channels = objChannels_[i].channels;
		const vector<AnimationChannelSPtr> & rhsChannels = rhs.objChannels_[i].channels;

		objChannels_[i].object = rhs.objChannels_[i].object;
		channels.resize(rhsChannels.size());

		for (int j = 0; j < (int)channels.size(); ++j)
		{
			channels[j].reset(rhsChannels[j]->clone());
		}
	}


	repeat_ = rhs.repeat_;
	name_ = rhs.name_;
	lastUpdateTime_ = 0;
}

Animation::~Animation()
{
}


bool Animation::hasObject(SceneObject * obj) const
{	
	BOOST_FOREACH(ObjAnimChannels c, objChannels_)
	{
		if (c.object == obj && !c.channels.empty()) return true;		
	}
	return false;
}

void Animation::remapReferences(const ElementMapping & elementMapping)
{
	vector<ObjAnimChannels>::iterator iter;
	for (iter = objChannels_.begin(); iter != objChannels_.end();)	
	{
		ObjAnimChannels & perObj = *iter;
		SceneObject * newObj = elementMapping.mapObject(perObj.object);
		if (!newObj)			
		{
			iter = objChannels_.erase(iter);
		}
		else
		{
			perObj.object = newObj;
			BOOST_FOREACH(AnimationChannelSPtr channel, perObj.channels)
			{
				channel->remapReferences(elementMapping);
			}
			++iter;
		}
	}


}

float Animation::duration() const
{
	float duration = 0;
	BOOST_FOREACH(const ObjAnimChannels & perObj, objChannels_)
	{
		BOOST_FOREACH(const AnimationChannelSPtr & channel, perObj.channels)
		{
			float chanDuration = channel->duration();
			if (chanDuration > duration) duration = chanDuration;
		}
	}
	return duration;
}

ErrorCode Animation::addChannel(const AnimationChannelSPtr & channel)
{
	BOOST_FOREACH(ObjAnimChannels & o, objChannels_)
	{
		if (o.object == channel->object())
		{
			o.channels.push_back(channel);
			return Ok;
		}
	}

	ObjAnimChannels o;
	o.object = channel->object();
	o.channels.push_back(channel);
	objChannels_.push_back(o);
	return Ok;
}



ErrorCode Animation::deleteChannel(AnimationChannel * channel)
{
	std::vector<ObjAnimChannels>::iterator oiter = objChannels_.begin();

	for(oiter = objChannels_.begin(); oiter != objChannels_.end(); oiter++)
	{
		ObjAnimChannels & o = *oiter;
		if (o.object == channel->object())
		{			
			vector<AnimationChannelSPtr>::iterator iter = o.channels.begin();
			for (;iter != o.channels.end(); ++iter)
			{
				if ((*iter).get() == channel)
				{
					o.channels.erase(iter);					
					break;
				}
			}

			//remove the ObjAnimChannels object itself if there are no more
			//channels left
			if (o.channels.empty())
			{
				objChannels_.erase(oiter);
				break;
			}

		}
	}

	return ChannelNotFound;
}

void Animation::objects(std::vector<SceneObject *> * objects) const
{
	objects->clear();
	BOOST_FOREACH(const ObjAnimChannels & o, objChannels_)
	{
		objects->push_back(o.object);
	}
}


const std::vector<AnimationChannelSPtr> * Animation::channels(SceneObject * obj) const
{
	BOOST_FOREACH(const ObjAnimChannels & o, objChannels_)
	{
		if (o.object == obj) return &o.channels;
	}
	return 0;
}

std::vector<AnimationChannelSPtr> * Animation::channelsToModify(SceneObject * obj)
{
	BOOST_FOREACH(ObjAnimChannels & o, objChannels_)
	{
		if (o.object == obj) return &o.channels;
	}
	return 0;
}


bool Animation::update(float time)
{
	if (repeat_)
	{
		float duration = 0;
		BOOST_FOREACH(ObjAnimChannels & o, objChannels_)
		{
			BOOST_FOREACH(const AnimationChannelSPtr & channel, o.channels)
			{
				float channelDuration = channel->duration();
				if (channelDuration > duration) duration = channelDuration;
			}
		}

		time -= duration * (int)(time / duration);
	}
	
	
	BOOST_FOREACH(ObjAnimChannels & o, objChannels_)		
		BOOST_FOREACH(const AnimationChannelSPtr & channel, o.channels)			
			channel->apply(time);	
	
	lastUpdateTime_ = time;
	
	return true;
}

void Animation::insertBefore(
	const AnimationChannelSPtr & newObj, AnimationChannel * refObj)
{
	vector<AnimationChannelSPtr>::iterator insertIter, removeIter, iter;
	if (newObj.get() == refObj) return;

	if (refObj && refObj->object() != newObj->object())
		throw Exception(InvalidArguments);

	vector<AnimationChannelSPtr> * channels = 
		this->channelsToModify(newObj->object());
	if (!channels) throw Exception(NoTargetObject);

	removeIter = channels->end();
	insertIter = channels->end();	
	for (iter = channels->begin(); iter != channels->end(); ++iter)	
	{
		if ((*iter) == newObj) removeIter = iter;	
		if ((*iter).get() == refObj) insertIter = iter;
	}

	if (removeIter == channels->end()) throw Exception(InvalidArguments);
	if (insertIter == channels->end() && refObj) throw Exception(InvalidArguments);

	channels->erase(removeIter);

	insertIter = channels->end();	
	for (iter = channels->begin(); iter != channels->end(); ++iter)	
		if ((*iter).get() == refObj) insertIter = iter;	

	channels->insert(insertIter, newObj);

}

void Animation::setChannels(
	SceneObject * object, const std::vector<AnimationChannelSPtr> & channels)
{
	if (channels.empty())
	{
		vector<ObjAnimChannels>::iterator iter = objChannels_.begin();
		for (; iter != objChannels_.end(); ++iter)
		{
			if ((*iter).object == object)
			{
				objChannels_.erase(iter);
				break;
			}
		}
		return;
	}

	BOOST_FOREACH(ObjAnimChannels & o, objChannels_)
	{
		if (o.object == object)
		{			
			o.channels = channels;
			return;
		}
	}

	ObjAnimChannels o;
	o.object = object;
	o.channels = channels;
	objChannels_.push_back(o);
	return;
}

AnimationChannelSPtr Animation::findChannel(AnimationChannel * channelToFind) const
{
	BOOST_FOREACH(const ObjAnimChannels & o, objChannels_)
	{
		BOOST_FOREACH(const AnimationChannelSPtr & channel, o.channels)		
			if (channel.get() == channelToFind) return channel;
		
	}

	return AnimationChannelSPtr();
}

void Animation::write(Writer & writer) const
{	
	writer.write(name_, "name");
	writer.write(repeat_, "repeat");

	
	vector<ObjAnimChannels> objChannels = objChannels_;
	
	//Make sure that no objChannels references non existent object.
	//This results in memory leaks when opening the resulting file
	//TODO: fully understand how such a situation could arise

	vector<ObjAnimChannels>::iterator iter;
	for (iter = objChannels.begin(); iter != objChannels.end();)
	{
		const ObjAnimChannels & c = *iter;
		if (!c.object->parentScene())
		{
			iter = objChannels.erase(iter);
		}
		else
		{
			++iter;
		}
	}	


	writer.write(objChannels, "objAnimChannels");	
}

void Animation::writeXml(XmlWriter & w) const
{
	w.writeTag("Name", name_);
	w.writeTag("Repeat", repeat_);
	
	w.startTag("Channels");
	for (unsigned int i = 0; i < (unsigned int)objChannels_.size(); ++i)
	{
		w.startTag("ObjectChannels");
		objChannels_[i].writeXml(w);
		w.endTag();
	}
	w.endTag();	
}

void Animation::readXml(XmlReader & r, xmlNode * parent)
{
	unsigned int id;
	r.getNodeAttribute(id, parent, "Id");
	if (!id)
		throw XmlException(parent, "No Id");

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(name_, curNode, "Name"));
		else if (r.getNodeContentIfName(repeat_, curNode, "Repeat"));

		else if (r.isNodeName(curNode, "Channels"))
		{
			for(xmlNode * channelsNode = curNode->children; channelsNode; channelsNode = channelsNode->next)
			{		
				if (channelsNode->type != XML_ELEMENT_NODE) continue;
				if (r.isNodeName(channelsNode, "ObjectChannels"))
				{
					ObjAnimChannels c;
					c.readXml(r, channelsNode);
					objChannels_.push_back(c);
				}
			}
		}
	}	

	r.idMapping().setId(this, id);
}

void Animation::read(Reader & reader, unsigned char)
{
	reader.read(name_);
	reader.read(repeat_);
	reader.read(objChannels_);
}

void Animation::ObjAnimChannels::write(Writer & writer) const
{
	writer.write(object, "object");
	writer.write(channels, "channels");
}

void Animation::ObjAnimChannels::writeXml(XmlWriter & w) const
{
	w.writeTag("Object", object);
	w.startTag("Channels");
	for (unsigned i = 0; i < (unsigned)channels.size(); ++i)
	{
		AnimationChannel * channel = channels[i].get();
		w.startTag(channel->xmlTag());
		channel->writeXml(w);
		w.endTag();

	}
	w.endTag();
}

void Animation::ObjAnimChannels::readXml(XmlReader & r, xmlNode * parent)
{
	unsigned objId = 0;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(objId, curNode, "Object"))
		{
			object = (SceneObject *)objId;
		}
	}
	if (!objId) throw XmlException(parent, "No valid object id");


	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(curNode, "Channels"))
		{

			for(xmlNode * cNode = curNode->children; cNode; cNode = cNode->next)
			{		
				if (cNode->type != XML_ELEMENT_NODE) continue;
				AnimationChannelSPtr a;
				if (r.getAnimChannel(a, cNode))
				{
					a->setObject(object);
					channels.push_back(a);
				}
			}					
		}
	}
		
}

void Animation::ObjAnimChannels::read(Reader & reader, unsigned char)
{
	reader.read(object);
	reader.read(channels);
}
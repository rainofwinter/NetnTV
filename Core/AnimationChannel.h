#ifndef AnimationChannel_h__
#define AnimationChannel_h__
#include "Transform.h"
#include "Interpolator.h"
#include "KeyFrame.h"
#include "Error.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ScriptProcessor.h"

class KeyFrame;
class KeyFrameAccess;
class XmlWriter;
class SceneObject;
class ElementMapping;
struct JSObject;

class AnimationChannel
{
	friend class KeyFrameAccess;
public:	
	virtual JSObject * createScriptObject(ScriptProcessor * s) const = 0;
	virtual AnimationChannel * fromScriptObject(
		ScriptProcessor * s, Scene * scene, JSObject * obj) const = 0;

	AnimationChannel() {object_ = 0;}
	virtual ~AnimationChannel() {}
	void setObject(SceneObject * object);
	SceneObject * object() const {return object_;}
	
	virtual ErrorCode setKeyFrame(float time) = 0;
	virtual void apply(float time) = 0;
	virtual float duration() const = 0;

	
	virtual int numKeyFrames() const = 0;
	
	virtual Vector3 PosFromKeyFrameIndex(int index) const = 0;


	virtual AnimationChannel * clone() const = 0;
	virtual AnimationChannel & equals(const AnimationChannel & rhs) = 0;
	
	virtual boost::uuids::uuid type() const = 0;
	virtual const char * name() const = 0;

	virtual bool supportsObject(SceneObject * obj) const = 0;

	bool operator == (const AnimationChannel & rhs) const
	{
		return type() == rhs.type() && object_ == rhs.object_;
	}

	virtual KeyFrame * keyFrame(float time) const = 0;
	virtual KeyFrame * keyFrame(int index) const = 0;	

	virtual ErrorCode removeKeyFrame(KeyFrame * keyFrame) = 0;
	virtual ErrorCode insertKeyFrame(KeyFrame * keyFrame) = 0;

	virtual unsigned char version() const = 0;
	virtual void write(Writer & writer) const = 0;
	virtual void read(Reader & reader, unsigned char) = 0;

	virtual const char * xmlTag() const = 0;
	virtual void writeXml(XmlWriter & w) = 0;
	virtual void readXml(XmlReader & r, xmlNode * node) = 0;
	void remapReferences(const ElementMapping & mapping);
protected:
	JSObject * createScriptObject(ScriptProcessor * s, 
		const std::vector<jsval> & keyFrameValues) const;

	AnimationChannel * fromScriptObject(
		ScriptProcessor * s, Scene * scene, 
		AnimationChannel * channel, JSObject * jsChannel) const;
//////////////
	AnimationChannel * ChannelfromKeyFrame(
		int index, float time) const;
//////////////
	virtual KeyFrame * keyFrameFromScriptObject(
		ScriptProcessor * s, JSObject * jsKey) const = 0;

	virtual void sortKeyFrames() = 0;
	SceneObject * object_;

};



///////////////////////////////////////////////////////////////////////////////
template <typename ChannelType, typename Value>
class AnimationChannelTemplate : public AnimationChannel
{
public:
	class ChannelKeyFrame : public KeyFrame
	{	
		friend class AnimationChannelTemplate;
	public:

		ChannelKeyFrame()
		{
			this->time_ = 0;
			interpolator_ = 0;
		}

		ChannelKeyFrame(float time, const Value & value) 
		{	
			this->time_ = time;
			this->value = value;
			interpolator_ = &gLinearInterpolator;
		}				
		
		bool supportsChannel(const AnimationChannel * channel) const
		{
			return channel->type() == ChannelType().type();
		}

		virtual unsigned char version() const {return 0;}

		void write(Writer & writer) const
		{
			writer.writeParent<KeyFrame>(this);
			writer.write(value, "value");
		}

		void read(Reader & reader, unsigned char version)
		{			
			reader.readParent<KeyFrame>(this);
			reader.read(value);
		}
		Value value;	
	};

	struct KeyFramePtrCompare
	{
		bool operator ()(
			const ChannelKeyFrame * lhs, const ChannelKeyFrame * rhs) const
		{return lhs->time_ < rhs->time_;}
	};

public:

	virtual AnimationChannel * fromScriptObject(
		ScriptProcessor * s, Scene * scene, JSObject * jsChannel) const
	{
		AnimationChannel * ret = new ChannelType;
		return AnimationChannel::fromScriptObject(s, scene, ret, jsChannel);
	}

	virtual Value keyFrameValueFromScriptObject(
		ScriptProcessor * s, jsval value) const = 0;


	virtual Vector3 PosFromValue(Value & value) const = 0;	


	virtual KeyFrame * keyFrameFromScriptObject(
		ScriptProcessor * s, JSObject * jsKey) const
	{
		ChannelKeyFrame * keyFrame = new ChannelKeyFrame;
		JSContext * cx = s->jsContext();
		float time;
		getPropertyFloat(cx, jsKey, "time", &time);
		keyFrame->setTime(time);
		
		jsval val;
		JS_GetProperty(cx, jsKey, "value", &val);		
		keyFrame->value = keyFrameValueFromScriptObject(s, val);

		std::string interp;
		getPropertyString(cx, jsKey, "interp", &interp);

		//TODO Make this more OO
		if (interp == gStepInterpolator.name())
			keyFrame->setInterpolator(&gStepInterpolator);
		else if (interp == gEaseInInterpolator.name())
			keyFrame->setInterpolator(&gEaseInInterpolator);
		else if (interp == gEaseOutInterpolator.name())
			keyFrame->setInterpolator(&gEaseOutInterpolator);
		else
			keyFrame->setInterpolator(&gLinearInterpolator);

		return keyFrame;
	}


	AnimationChannelTemplate()
	{
		index_ = 0;
	}


	AnimationChannelTemplate(const AnimationChannelTemplate & rhs) : 
	AnimationChannel(rhs)
	{
		index_ = rhs.index_;
		keyFrames_.reserve(rhs.keyFrames_.size());
		BOOST_FOREACH(ChannelKeyFrame * key, rhs.keyFrames_)
		{
			keyFrames_.push_back(new ChannelKeyFrame(*key));
		}
	}

	AnimationChannelTemplate & operator = (const AnimationChannelTemplate & rhs)
	{
		if (this == &rhs) return *this;
		index_ = rhs.index_;
		BOOST_FOREACH(ChannelKeyFrame * key, keyFrames_) delete key;
		keyFrames_.clear();
		keyFrames_.reserve(rhs.keyFrames_.size());
		BOOST_FOREACH(ChannelKeyFrame * key, rhs.keyFrames_)
		{
			keyFrames_.push_back(new ChannelKeyFrame(*key));
		}
		return *this;
	}

	~AnimationChannelTemplate()
	{
		BOOST_FOREACH(ChannelKeyFrame * key, keyFrames_) delete key;
	}

	virtual AnimationChannel * clone() const
	{
		return new ChannelType(*(ChannelType *)this);
	}

	virtual AnimationChannel & equals(const AnimationChannel & rhs)
	{
		*(ChannelType *)this = (ChannelType &)rhs; 
		return *this;
	}

	virtual void setValue(const Value & valueA, const Value & valueB, float t) = 0;
	virtual Value getValue() = 0;


	virtual int numKeyFrames() const {return keyFrames_.size();}

	virtual ChannelKeyFrame * keyFrame(int index) const
	{
		if (index < 0 || index >= (int)keyFrames_.size()) return 0;
		return keyFrames_[index];
	}

	virtual ChannelKeyFrame * keyFrame(float time) const
	{
		for (int i = 0; i < (int)keyFrames_.size(); ++i)
		{
			if (keyFrames_[i]->time_ == time)
				return keyFrames_[i];
		}
		return 0;
	}
	
	virtual ErrorCode setKeyFrame(float time)
	{		
		if (!object_) return NoTargetObject;
				
		for (int i = 0; i < (int)keyFrames_.size(); ++i)
		{
			if (keyFrames_[i]->time_ == time) return KeyFrameExists;
		}
		
		keyFrames_.push_back(new ChannelKeyFrame(time, getValue()));
		sortKeyFrames();

		index_ = 0;	

		return Ok;
	}

	virtual float duration() const
	{
		if (keyFrames_.empty()) return 0;
		return keyFrames_.back()->time_;
	}


	virtual Vector3 PosFromKeyFrameIndex(int index) const
	{		
		return PosFromValue(keyFrames_[index]->value);
	}	


	virtual void apply(float time)
	{				
		int numKeyFrames = (int)keyFrames_.size();
		if (!object_ || numKeyFrames == 0) return;

		if (index_ > numKeyFrames - 1) index_ = 0;
		else if (keyFrames_[index_]->time_ > time) index_ = 0;
		
		int leftIndex = -1;
		int i;	
		for (i = index_; i < numKeyFrames; ++i)
		{
			if (keyFrames_[i]->time_ <= time) leftIndex = i;
			else break;			
		}	
		int rightIndex = leftIndex + 1;

		//if this is the last keyframe, just use its value without interpolating
		if (rightIndex >= numKeyFrames)
		{
			setValue(
				keyFrames_[leftIndex]->value, keyFrames_[leftIndex]->value, 0);
		}
		else if (rightIndex == 0)
		{
			setValue(
				keyFrames_[0]->value, keyFrames_[0]->value, 0);
		}
		else
		{
		
			float startTime = keyFrames_[leftIndex]->time_;
			float endTime = keyFrames_[leftIndex + 1]->time_;
			float timeDelta = endTime - startTime;
			float t = (time - startTime) / (endTime - startTime);

			t = keyFrames_[rightIndex]->interpolator_->interpolate(t);

			//linear interpolation with the adjusted t
			
			setValue(
				keyFrames_[leftIndex]->value, keyFrames_[rightIndex]->value, t);
		}
	}

	virtual unsigned char version() const {return 0;}
	void write(Writer & writer) const
	{		
		writer.writeParent<AnimationChannel>(this);

		writer.write((unsigned int)keyFrames_.size(), "num Keyframes");
		for (unsigned int i = 0; i < (unsigned int)keyFrames_.size(); ++i)
		{
			writer.write(keyFrames_[i]->time_, "time");
			writer.write(keyFrames_[i]->interpolator_->type(), "interpolator type");
			writer.write(keyFrames_[i]->value, "value");
		}
	}

	virtual const char * xmlTag() const = 0;
	virtual void writeXml(XmlWriter & w) 
	{
		for (unsigned i = 0; i < (unsigned)keyFrames_.size(); ++i)
		{
			w.startTag("KeyFrame");
			w.writeTag("Time", keyFrames_[i]->time());
			w.writeTag("Value", keyFrames_[i]->value);
			w.writeTag("Interp", (unsigned int)keyFrames_[i]->interpolator()->type());
			w.endTag();
		}
	}

	virtual void readXml(XmlReader & r, xmlNode * pNode)
	{
		for(xmlNode * node = pNode->children; node; node = node->next)
		{
			if (node->type != XML_ELEMENT_NODE) continue;
			if (!r.isNodeName(node, "KeyFrame")) continue;

			ChannelKeyFrame kf;	
			unsigned int val = 0;
			InterpolatorType interpolatorType;
			bool hasInterp = false;
			bool hasTime = false;
			bool hasValue = false;

			for(xmlNode * curNode = node->children; curNode; curNode = curNode->next)
			{		
				if (curNode->type != XML_ELEMENT_NODE) continue;

				if (r.getNodeContentIfName(kf.time_, curNode, "Time")) hasTime = true;
				else if (r.getNodeContentIfName(kf.value, curNode, "Value")) hasValue = true;
				else if (r.getNodeContentIfName(val, curNode, "Interp"))
				{
					interpolatorType = (InterpolatorType)val;
					hasInterp = true;
				}				
			}
			
			if (hasInterp && hasTime && hasValue)
			{
				ChannelKeyFrame * kfN = new ChannelKeyFrame;
				kfN->time_ = kf.time_;
				kfN->value = kf.value;
				if (interpolatorType == gLinearInterpolator.type())
					kfN->interpolator_ = &gLinearInterpolator;
				else if (interpolatorType == gStepInterpolator.type())
					kfN->interpolator_ = &gStepInterpolator;
				else if (interpolatorType == gEaseInInterpolator.type())
					kfN->interpolator_ = &gEaseInInterpolator;
				else if (interpolatorType == gEaseOutInterpolator.type())
					kfN->interpolator_ = &gEaseOutInterpolator;

				keyFrames_.push_back(kfN);
			}
		}

		sortKeyFrames();
	}

	void read(Reader & reader, unsigned char version)
	{		
		reader.readParent<AnimationChannel>(this);
		unsigned int numKeyFrames;
		reader.read(numKeyFrames);
		keyFrames_.resize(numKeyFrames);
		for (unsigned int i = 0; i < (unsigned int)keyFrames_.size(); ++i)
		{
			keyFrames_[i] = new ChannelKeyFrame;
			reader.read(keyFrames_[i]->time_);
			InterpolatorType interpolatorType;
			reader.read(interpolatorType);

			if (interpolatorType == gLinearInterpolator.type())
				keyFrames_[i]->interpolator_ = &gLinearInterpolator;
			else if (interpolatorType == gStepInterpolator.type())
				keyFrames_[i]->interpolator_ = &gStepInterpolator;
			else if (interpolatorType == gEaseInInterpolator.type())
				keyFrames_[i]->interpolator_ = &gEaseInInterpolator;
			else if (interpolatorType == gEaseOutInterpolator.type())
				keyFrames_[i]->interpolator_ = &gEaseOutInterpolator;

			reader.read(keyFrames_[i]->value);

		}
	}
protected:

	virtual ErrorCode removeKeyFrame(KeyFrame * keyFrame)
	{
		typename std::vector<ChannelKeyFrame *>::iterator iter = keyFrames_.begin();
		for (;iter != keyFrames_.end(); ++iter)
		{
			if (*iter == keyFrame) 
			{
				keyFrames_.erase(iter);
				return Ok;
			}
		}
		return KeyFrameNotFound;
	}

	virtual ErrorCode insertKeyFrame(KeyFrame * keyFrame)
	{
		if (!keyFrame->supportsChannel(this)) return WrongKeyFrameType;
		for (int i = 0; i < (int)keyFrames_.size(); ++i)
		{
			if (keyFrames_[i]->time_ == keyFrame->time_) return KeyFrameExists;
		}

		keyFrames_.push_back((ChannelKeyFrame *)keyFrame);
		sortKeyFrames();

		return Ok;
	}


	virtual void sortKeyFrames()
	{
		std::sort(keyFrames_.begin(), keyFrames_.end(), KeyFramePtrCompare());
	}

	int index_;
	std::vector<ChannelKeyFrame *> keyFrames_;	
};

///////////////////////////////////////////////////////////////////////////////
class TransformChannel 
:
public AnimationChannelTemplate<TransformChannel, Transform>
{
public:
	virtual JSObject * createScriptObject(ScriptProcessor * s) const;
	virtual Transform keyFrameValueFromScriptObject(
		ScriptProcessor * s, jsval value) const;

	const char * xmlTag() const {return "TransformChannel";}

	virtual Vector3 PosFromValue(Transform & tr) const;

	virtual const char * name() const {return "transform";}
	virtual void setValue(
		const Transform & a, const Transform & b, float t);

	virtual Transform getValue();

	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("FD7679BA-B04B-BFD6-F35C-E9EDB2938564");
	}

	virtual bool supportsObject(SceneObject * obj) const;
};

////////////////////////////////////////////////////////////////////////////////
class OpacityChannel 
: 
public AnimationChannelTemplate<OpacityChannel, float>
{
public:

	virtual JSObject * createScriptObject(ScriptProcessor * s) const;
	virtual float keyFrameValueFromScriptObject(
		ScriptProcessor * s, jsval value) const;

	const char * xmlTag() const {return "OpacityChannel";}

////////temporary... need correct
	virtual Vector3 PosFromValue(float & a) const;
////////

	virtual const char * name() const {return "opacity";}
	virtual void setValue(
		const float & a, const float & b, float t);

	virtual float getValue();

	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("099D45D0-A111-EE1B-92D5-18A2E91F568F");
	}

	virtual bool supportsObject(SceneObject * obj) const;
};

////////////////////////////////////////////////////////////////////////////////
class VisibleChannel 
: 
public AnimationChannelTemplate<VisibleChannel, bool>
{
public:
	virtual JSObject * createScriptObject(ScriptProcessor * s) const;
	virtual bool keyFrameValueFromScriptObject(
		ScriptProcessor * s, jsval value) const;	

	const char * xmlTag() const {return "VisibleChannel";}
////////temporary... need correct
	virtual Vector3 PosFromValue(bool & a) const;
////////

	virtual const char * name() const {return "visible";}
	virtual void setValue(
		const bool & a, const bool & b, float t);

	virtual bool getValue();

	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("DACE35D0-A111-EE1B-92D5-18A2E91F568F");
	}

	virtual bool supportsObject(SceneObject * obj) const;
};


#endif // AnimationChannel_h__
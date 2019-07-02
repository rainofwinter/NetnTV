#ifndef KeyFrame_h__
#define KeyFrame_h__
#include "Error.h"

class Interpolator;
class AnimationChannel;
class Writer;

class KeyFrame
{
	friend class KeyFrameAccess;
	template <typename ChannelType, typename ValueType>
	friend class AnimationChannelTemplate;
public:
	bool operator < (const KeyFrame & rhs) const {return time_ < rhs.time_;}
	virtual bool supportsChannel(const AnimationChannel * channel) const = 0;

	float time() const {return time_;}
	void setTime(float time) {time_ = time;}
	Interpolator * interpolator() const {return interpolator_;}
	void setInterpolator(Interpolator * interpolator)
	{
		interpolator_ = interpolator;
	}

	virtual unsigned char version() const = 0;	
	virtual void write(Writer & writer) const = 0;
protected:		
	float time_;
	Interpolator * interpolator_;
};

/**
This is a way to reference keyframes by channel and time instead of using
direct pointers. This is because the actual pointer addresses can change 
unexpectedly.
*/
class KeyFrameData
{
public:
	KeyFrameData()
	{
		time = 0;
		channel = 0;
	}
	KeyFrameData(AnimationChannel * channel, float time)
	{
		this->time = time;
		this->channel = channel;
	}

	bool operator == (const KeyFrameData & rhs) const 
	{
		return channel == rhs.channel && time == rhs.time;
	}

	///retrieve the pointer to the keyframe that corresponds to this->channel and
	///this->time. return 0 if there is no such keyframe
	::KeyFrame * ptr() const;
	AnimationChannel * channel;
	float time;
};

/**

A safe way to modify data in a keyframe.

For example, calling KeyFrame::setTime directly should be avoided.
This is because, care has to be taken so that the ordering of keyframes 
(by time) in the channel is maintained and that there are no keyframes in the
channel with the same times.

Any instance of this class should never be saved for an extend time.
It's too easy for the internal KeyFrame pointer to get invalidated.
*/
class KeyFrameAccess
{
	friend class KeyFrame;
public:
	KeyFrameAccess(AnimationChannel * channel, int index);
	KeyFrameAccess(const KeyFrameData & data);
	~KeyFrameAccess();
	ErrorCode time(float * time) const;
	ErrorCode setTime(float time);

	ErrorCode interpolator(Interpolator ** interp) const;
	ErrorCode setInterpolator(Interpolator * interp);

	AnimationChannel * channel() const {return channel_;}

	bool operator == (const KeyFrameAccess & rhs) const 
	{
		return keyFrame_ == rhs.keyFrame_;
	}

private:
	AnimationChannel * channel_;
	KeyFrame * keyFrame_;
};


#endif // KeyFrame_h__
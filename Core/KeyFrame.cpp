#include "stdafx.h"
#include "AnimationChannel.h"
#include "KeyFrame.h"
#include "Writer.h"

unsigned char KeyFrame::version() const
{
	return 0;
}

void KeyFrame::write(Writer & writer) const
{
	writer.write(time_, "time");
	writer.write(interpolator_->type(), "interpolator type");
}

///////////////////////////////////////////////////////////////////////////////

KeyFrameAccess::KeyFrameAccess (const KeyFrameData & data)
{
	channel_ = data.channel;
	keyFrame_ = data.ptr();
}

KeyFrameAccess::KeyFrameAccess (AnimationChannel * channel, int index)
{
	channel_ = channel;
	keyFrame_ = channel->keyFrame(index);
}

KeyFrameAccess::~KeyFrameAccess()
{
}

ErrorCode KeyFrameAccess::time(float * time) const
{
	if (!keyFrame_) return InvalidState;
	*time = keyFrame_->time_;
	return Ok;
}

ErrorCode KeyFrameAccess::interpolator(Interpolator ** interp) const
{
	if (!keyFrame_) return InvalidState;
	*interp = keyFrame_->interpolator_;
	return Ok;
}

ErrorCode KeyFrameAccess::setInterpolator(Interpolator * interp)
{
	if (!keyFrame_) return InvalidState;
	keyFrame_->interpolator_ = interp;
	return Ok;	
}

ErrorCode KeyFrameAccess::setTime(float time)
{
	if (!keyFrame_) return InvalidState;
	if (channel_->keyFrame(time)) return KeyFrameExists;
	keyFrame_->time_ = time;
	channel_->sortKeyFrames();
	return Ok;
}

KeyFrame * KeyFrameData::ptr() const 
{
	if (!channel) return 0;
	return channel->keyFrame(time);
}

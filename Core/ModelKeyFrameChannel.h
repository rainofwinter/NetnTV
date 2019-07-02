#pragma once
#include "ModelKeyFrame.h"
#include "Reader.h"
#include "Writer.h"

/**
Represents a keyframe group / animation track
*/
template <typename T>
class ModelKeyFrameChannel
{  

public:

	void setKeyFrames(const std::vector<ModelKeyFrame<T> > frames) 
	{this->frames = frames;}

	void addKeyFrame(float time, const T & value);
	
	ModelKeyFrameChannel(unsigned numKeyFrames);

	ModelKeyFrameChannel();

	ModelKeyFrameChannel(const std::vector<ModelKeyFrame<T> > & frames);
	
	void setNumKeyFrames(unsigned numKeyFrames);	

	void setTargetObjectIndex(int index) {targetObjectIndex_ = index;}
	int targetObjectIndex() const {return targetObjectIndex_;}

	/**
	set the current time of this keyframe group to newTime
	Note, this function may set curTime to something not exactly equal to 
	newTime curTime is set to the time of the nearest keyframe whose time is <= 
	newTime curFrameIndex is also appropriately modified

	@param newTime requested time
	@return whether the time was indeed updated
	*/
	bool setTime(float newTime);

	const unsigned numFrames() const {return frames.size();}
	
	///get the value (matrix, weight, etc) at the current time
	const T & value() const;

	
	unsigned char version() const {return 0;}
	
	void write(Writer & writer) const
	{
		writer.write(targetObjectIndex_);
		writer.write(frames);
	}
	
	void read(Reader & reader, unsigned char)
	{
		reader.read(targetObjectIndex_);
		reader.read(frames);
	}



private:
	///index of joint/mesh/etc. that the keyframes apply to
	unsigned int targetObjectIndex_;

	///the list of frames
	std::vector< ModelKeyFrame<T> > frames;	
		
	///current time of the animation track
	float curTime;

	///current index number (corresponding to current time) into frames list
	unsigned curFrameIndex;	
};



/**
if possible use other constructor (that takes in #keyframes)
or be sure to call SetNumKeyFrames afterward
(don't want frames vector too large)
*/
template<typename T>
ModelKeyFrameChannel<T>::ModelKeyFrameChannel()
{
	curFrameIndex = 0;
	curTime = 0;	
}

///constructor
template<typename T>
ModelKeyFrameChannel<T>::ModelKeyFrameChannel(unsigned numKeyFrames)
{
	frames.reserve(numKeyFrames);
	curFrameIndex = 0;
	curTime = 0;
}

template<typename T>
ModelKeyFrameChannel<T>::ModelKeyFrameChannel(const std::vector<ModelKeyFrame<T> > & frames) :
frames(frames)
{
	curFrameIndex = 0;
	curTime = 0;
}

/**
do a vector::reserve

(to try not to make vector too much larger than required)
*/
template<typename T>
void ModelKeyFrameChannel<T>::setNumKeyFrames(unsigned numKeyFrames)
{
	frames.reserve(numKeyFrames);
}

template<typename T>
const T & ModelKeyFrameChannel<T>::value() const 
{
	assert(frames.size() > 0);
	return frames[curFrameIndex].value;
}

///add the specified keyframe into keyframes list
template<typename T>
void ModelKeyFrameChannel<T>::addKeyFrame(float time, const T & value)
{
	frames.push_back(ModelKeyFrame<T>(time, value));
	//if this is the first added frame
	if (frames.size() == 1)
	{
		curTime = frames[0].time;
	}
}


template<typename T>
bool ModelKeyFrameChannel<T>::setTime(float newTime)
{
	size_t numFrames = frames.size();
	float prevTime = curTime;
	if (newTime > curTime)
	{
		//increment curFrameIndex until the time of corresponding frame ise >= newTime
		//or curFrameIndex == numFrames - 1
		while(
			newTime > curTime && 
			curFrameIndex < numFrames-1 &&
			
			//this condition is important, the current keyframe cannot have time 
			//> than newTime. ex: there could be a keyframe at 0 and 100, and 
			//newTime could be 10. ClearlyS the 0 keyframe should be used.
			frames[curFrameIndex+1].time <= newTime
			)
		{
			++curFrameIndex;
			curTime = frames[curFrameIndex].time;
		}

	}
	else if (newTime < curTime)
	{
		//decrement curFrameIndex until the time of corresponding frame >= newTime
		//or curFrameIndex == 0
		while(newTime < curTime && curFrameIndex > 0)
		{
			--curFrameIndex;
			curTime = frames[curFrameIndex].time;
		}
	}
	
	return curTime != prevTime;
}


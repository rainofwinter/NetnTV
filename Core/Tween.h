#pragma once

template<typename T>
class Tween
{
public:
	Tween()
	{
		startTime_ = 0;
		duration_ = 0.0f;
	}

	void start(float startTime, const T & startState, const T & endState)
	{
		if (startState == endState) return;
		startTime_ = startTime;
		duration_ = 0.3f;
		startState_ = startState;
		endState_ = endState;
	}

	bool isHappening() const {return duration_ > 0.0f;}

	void reset()
	{
		duration_ = 0.0f;
	}

	T update(float time) 
	{
		float t = (time - startTime_) / duration_;		
		if (t > 1) t = 1;		
		t = 1 - (t - 1)*(t - 1);

		T curState =
			startState_ + t * (endState_ - startState_);	

		if (t == 1) reset();

		return curState;
	}

private:


	float startTime_;
	float duration_;
	T startState_;
	T endState_;
};
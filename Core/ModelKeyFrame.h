#pragma once

template<typename T>
class ModelKeyFrame
{
public:
	
	float time;
	T value;

	ModelKeyFrame(float time, const T & value)
	{
		this->time = time;
		this->value = value;
	}

	ModelKeyFrame()
	{}


};


#pragma once

class PressEventData
{
public:
	PressEventData() 
	{
		create();
	}

	PressEventData(const Vector2 & startPt)
	{
		create();
		this->startPt = startPt;
	}

    Vector2 curPt;
    Vector2 worldPressPt;
	Vector2 startPt;
	Vector2 pressPt;
	enum
	{
		NotPressed = 0,
		Pressed = 1,		
		PassedToChild = 2,
		PassedToParent = 3,
		StrayPress = 4
	}pressed;
	bool startedMoving;
	float startMovingTime;	
	bool propagate;
private:
	void create()
	{
		pressed = NotPressed;
		startedMoving = false;
		startMovingTime = 0.0f;
		propagate = false;
	}
};
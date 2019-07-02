#pragma once
#include "MathStuff.h"

class DragVelocityHandler
{
public:
	DragVelocityHandler();
	void clearMoveHistory();
	void addMoveHistory(const Vector2 & pos, float time);
	void calcVelocityFromHistory(const Vector2 & curPos, float curTime);

	const Vector2 & dragVelocity() const {return dragVelocity_;}
	const Vector2 & unitDragVelocity() const {return unitDragVelocity_;}
	const float & dragAccel() const {return dragAccel_;}
	
	const Vector2 & moveDelta() const {return moveDelta_;}

	void update(float time);
	bool isZeroVelocity() const {return dragVelocity_.x == 0.0f && dragVelocity_.y == 0.0f;}
	void hasHitBoundary(bool horz, bool vert);
private:
	//Drag velocity handling
	static const int cNumPrevPos = 2;
	std::vector<float> prevTimeList_;
	std::vector<Vector2> prevPosList_;

	float lastTime_;
	Vector2 dragVelocity_;
	float dragAccel_;
	Vector2 unitDragVelocity_;
	Vector2 moveDelta_;
	bool hasHitBoundaryHorz_, hasHitBoundaryVert_;

};
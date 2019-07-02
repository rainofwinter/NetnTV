#include "stdafx.h"
#include "DragVelocityHandler.h"
#include "Global.h"

DragVelocityHandler::DragVelocityHandler()
{
	prevTimeList_.reserve(cNumPrevPos);
	prevPosList_.reserve(cNumPrevPos);
	dragAccel_ = 1.0f; //changed later
	dragVelocity_ = Vector2(0.0f, 0.0f);
	lastTime_ = 0.0f;
	hasHitBoundaryHorz_ = false;
	hasHitBoundaryVert_ = false;
}

void DragVelocityHandler::clearMoveHistory()
{
	prevTimeList_.clear();
	prevPosList_.clear();
	dragVelocity_ = Vector2(0.0f, 0.0f);
}

void DragVelocityHandler::addMoveHistory(const Vector2 & pos, float time)
{	
	const static float minTimeStep = 0.01f;
	bool notFilled = (int)prevPosList_.size() < cNumPrevPos;
	bool enoughTimePassed = 
		prevTimeList_.empty() || time - prevTimeList_.back() > minTimeStep;

	if (!enoughTimePassed) return;

	if (notFilled)
	{
		prevPosList_.push_back(pos);
		prevTimeList_.push_back(time);
	}
	else
	{
		for (int i = 0; i < cNumPrevPos - 1; ++i)
		{
			prevPosList_[i] = prevPosList_[i + 1];
			prevTimeList_[i] = prevTimeList_[i + 1];
		}

		prevPosList_.back() = pos;
		prevTimeList_.back() = time;
	}
}

void DragVelocityHandler::calcVelocityFromHistory(const Vector2 & curPos, float curTime)
{	
	if ((int)prevPosList_.size() < cNumPrevPos)
	{
		dragVelocity_ = unitDragVelocity_ = Vector2(0, 0);
		return;
	}

	if (prevPosList_.back() == curPos)
	{
		curTime = prevTimeList_.back();
	}

	float timeDelta = curTime - prevTimeList_[0];
	dragVelocity_ = (1/timeDelta) * (curPos - prevPosList_[0]);
	unitDragVelocity_ = dragVelocity_.normalize();
	lastTime_ = curTime;
	dragAccel_ = 4000.0f;
	hasHitBoundaryHorz_ = false;
	hasHitBoundaryVert_ = false;
	
}

void DragVelocityHandler::update(float time)
{
	float timeDelta = time - lastTime_;
	float speed = dragVelocity_.magnitude();
	speed -= timeDelta*dragAccel_;
	if (speed <= 1.0f) speed = 0;		
	dragVelocity_ = speed * unitDragVelocity_;

	moveDelta_ = timeDelta * dragVelocity_;
	lastTime_ = time;
}


void DragVelocityHandler::hasHitBoundary(bool horz, bool vert)
{
	if ((!horz || hasHitBoundaryHorz_) && (!vert || hasHitBoundaryVert_)) return;


	float boundaryStopFactor = 0.25f;
	float speed = dragVelocity_.magnitude();
	dragAccel_ = std::max(dragAccel_, speed/0.75f);
	if (horz && !hasHitBoundaryHorz_)
	{		
		dragVelocity_.x = speed * boundaryStopFactor * unitDragVelocity_.x;
		hasHitBoundaryHorz_ = true;
	}
	if (vert && !hasHitBoundaryVert_)
	{
		dragVelocity_.y = speed * boundaryStopFactor * unitDragVelocity_.y;
		hasHitBoundaryVert_ = true;
	}
	
}
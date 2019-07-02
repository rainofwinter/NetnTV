#pragma once
#include "MathStuff.h"
class Transform;
class ScriptProcessor;

class OBoundingBox;

class BoundingBox
{
public:

	JSObject * createJsonScriptObject(ScriptProcessor * s) const;

	BoundingBox()
	{
		minPt = Vector3(0, 0, 0);
		maxPt = Vector3(0, 0, 0);
	}
	BoundingBox(const Vector3 & minPt, const Vector3 & maxPt)
	{
		this->minPt = minPt;
		this->maxPt = maxPt;
	}

	Vector3 midPt() const
	{
		return 0.5f*(maxPt + minPt);
	}

	void applyTranslation(float x, float y, float z); 
	void applyScaling(float sx, float sy, float sz);

	BoundingBox transform(const Matrix & matrix) const;
	BoundingBox unite(const BoundingBox & other) const;
	BoundingBox intersect(const BoundingBox & other) const;
	bool intersect(float * tOut, const Ray & ray) const;
	bool doesIntersect(const BoundingBox & other) const;
	/**
	local scaling
	*/
	BoundingBox scale(const Transform & transform) const;

	OBoundingBox toObb(const Matrix & transform) const;

	float lengthX() const;
	float lengthY() const;
	float lengthZ() const;

	bool isEmpty() const;
public:
	Vector3 minPt, maxPt;
};




///////////////////////////////////////////////////////////////////////////////

class OBoundingBox
{
public:
	static OBoundingBox fromScriptObject(ScriptProcessor *, JSObject *);
	Vector3 xAxis, yAxis, zAxis;
	Vector3 origin;

	Vector3 center() const;

	///intersect oriented bounding box
	bool intersect(const OBoundingBox & other) const;

	/**
	intersect triangle
	*/
	bool intersect(const Vector3 & p0, const Vector3 & p1, const Vector3 & p2) const;

	bool intersect(const BoundingBox & aabb) const;
};
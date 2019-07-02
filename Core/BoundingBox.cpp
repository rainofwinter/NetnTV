#include "stdafx.h"
#include "BoundingBox.h"
#include "Transform.h"
#include "ScriptProcessor.h"

using namespace std;


JSObject * BoundingBox::createJsonScriptObject(ScriptProcessor * s) const
{
	JSContext * cx = s->jsContext();
	JSObject * jsBb = JS_NewObject(cx, 0, 0, 0);

	jsval val;
	val = OBJECT_TO_JSVAL(minPt.createScriptObject(s));
	JS_SetProperty(cx, jsBb, "minPt", &val);

	val = OBJECT_TO_JSVAL(maxPt.createScriptObject(s));
	JS_SetProperty(cx, jsBb, "maxPt", &val);

	return jsBb;
}

BoundingBox BoundingBox::transform(const Matrix & matrix) const
{
	Vector3 corners[8];
	Vector3 newMinPt(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3 newMaxPt(FLT_MIN, FLT_MIN, FLT_MIN);


	corners[0] = matrix * Vector3(minPt.x, maxPt.y, minPt.z);
	corners[1] = matrix * Vector3(maxPt.x, minPt.y, minPt.z);
	corners[2] = matrix * Vector3(maxPt.x, maxPt.y, minPt.z);
	corners[3] = matrix * Vector3(minPt.x, minPt.y, minPt.z);

	corners[4] = matrix * Vector3(minPt.x, maxPt.y, maxPt.z);
	corners[5] = matrix * Vector3(maxPt.x, minPt.y, maxPt.z);
	corners[6] = matrix * Vector3(maxPt.x, maxPt.y, maxPt.z);
	corners[7] = matrix * Vector3(minPt.x, minPt.y, maxPt.z);


	for (int i = 0; i < 8; ++i)
	{
		if (corners[i].x < newMinPt.x) newMinPt.x = corners[i].x;
		if (corners[i].y < newMinPt.y) newMinPt.y = corners[i].y;
		if (corners[i].z < newMinPt.z) newMinPt.z = corners[i].z;

		if (corners[i].x > newMaxPt.x) newMaxPt.x = corners[i].x;
		if (corners[i].y > newMaxPt.y) newMaxPt.y = corners[i].y;
		if (corners[i].z > newMaxPt.z) newMaxPt.z = corners[i].z;
	}

	return BoundingBox(newMinPt, newMaxPt);
}

void BoundingBox::applyTranslation(float x, float y, float z)
{
	minPt.x += x;
	minPt.y += y;
	minPt.z += z;

	maxPt.x += x;
	maxPt.y += y;
	maxPt.z += z;	
}

void BoundingBox::applyScaling(float sx, float sy, float sz)
{
	minPt.x *= sx;
	minPt.y *= sy;
	minPt.z *= sz;

	maxPt.x *= sx;
	maxPt.y *= sy;
	maxPt.z *= sz;
}

BoundingBox BoundingBox::unite(const BoundingBox & other) const
{
	if (isEmpty()) return other;
	if (other.isEmpty()) return *this;

	Vector3 newMinPt, newMaxPt;

	newMinPt.x = min(minPt.x, other.minPt.x);
	newMinPt.y = min(minPt.y, other.minPt.y);
	newMinPt.z = min(minPt.z, other.minPt.z);

	newMaxPt.x = max(maxPt.x, other.maxPt.x);
	newMaxPt.y = max(maxPt.y, other.maxPt.y);
	newMaxPt.z = max(maxPt.z, other.maxPt.z);

	return BoundingBox(newMinPt, newMaxPt);
}

bool BoundingBox::doesIntersect(const BoundingBox & other) const
{
	if (other.maxPt.x < minPt.x || maxPt.x < other.minPt.x) return false;
	if (other.maxPt.y < minPt.y || maxPt.y < other.minPt.y) return false;
	if (other.maxPt.z < minPt.z || maxPt.z < other.minPt.z) return false;

	return true;
}

bool BoundingBox::intersect(float * tOut, const Ray & ray) const
{
	Vector3 a = ray.origin;
	Vector3 b = ray.origin + ray.dir;

	float minT = FLT_MAX;
	float t;
	float px, py, pz;


	//intersect plane x = mintPt.x
	t = (minPt.x - ray.origin.x) / ray.dir.x;
	py = ray.origin.y + t * ray.dir.y;
	pz = ray.origin.z + t * ray.dir.z;

	if (0.0f < py && py < 1.0f && 0.0f < pz && pz < 1.0f)
	{
		if (t < minT) minT = t;	
	}


	//intersect plane x = 1
	t = (maxPt.x - ray.origin.x) / ray.dir.x;
	py = ray.origin.y + t * ray.dir.y;
	pz = ray.origin.z + t * ray.dir.z;

	if (0.0f < py && py < 1.0f && 0.0f < pz && pz < 1.0f)
	{
		if (0.0f < t && t < minT) minT = t;	
	}


	//intersect plane y = 0
	t = (minPt.y - ray.origin.y) / ray.dir.y;
	px = ray.origin.x + t * ray.dir.x;
	pz = ray.origin.z + t * ray.dir.z;

	if (0.0f < px && px < 1.0f && 0.0f < pz && pz < 1.0f)
	{
		if (0.0f < t && t < minT) minT = t;	
	}


	//intersect plane y = 1
	t = (maxPt.y - ray.origin.y) / ray.dir.y;
	px = ray.origin.x + t * ray.dir.x;
	pz = ray.origin.z + t * ray.dir.z;

	if (0.0f < px && px < 1.0f && 0.0f < pz && pz < 1.0f)
	{
		if (0.0f < t && t < minT) minT = t;	
	}

	//intersect plane z = 0
	t = (minPt.z - ray.origin.z) / ray.dir.z;
	px = ray.origin.x + t * ray.dir.x;
	py = ray.origin.y + t * ray.dir.y;

	if (0.0f < px && px < 1.0f && 0.0f < py && py < 1.0f)
	{
		if (0.0f < t && t < minT) minT = t;	
	}

	//intersect plane z = 1
	t = (maxPt.z - ray.origin.z) / ray.dir.z;
	px = ray.origin.x + t * ray.dir.x;
	py = ray.origin.y + t * ray.dir.y;

	if (0.0f < px && px < 1.0f && 0.0f < py && py < 1.0f)
	{
		if (0.0f < t && t < minT) minT = t;	
	}

	if (minT < FLT_MAX)
	{
		*tOut = t;
		return true;
	}

	return false;
}

BoundingBox BoundingBox::intersect(const BoundingBox & other) const
{
	BoundingBox ret;

	if (other.maxPt.x < minPt.x || maxPt.x < other.minPt.x) return ret;
	if (other.maxPt.y < minPt.y || maxPt.y < other.minPt.y) return ret;
	if (other.maxPt.z < minPt.z || maxPt.z < other.minPt.z) return ret;

	if (minPt.x < other.minPt.x)
	{
		ret.minPt.x = other.minPt.x;
		ret.maxPt.x = maxPt.x;
	}
	else
	{
		ret.minPt.x = minPt.x;
		ret.maxPt.x = other.maxPt.x;
	}

	if (minPt.y < other.minPt.y)
	{
		ret.minPt.y = other.minPt.y;
		ret.maxPt.y = maxPt.y;
	}
	else
	{
		ret.minPt.y = minPt.y;
		ret.maxPt.y = other.maxPt.y;
	}

	if (minPt.z < other.minPt.z)
	{
		ret.minPt.z = other.minPt.z;
		ret.maxPt.z = maxPt.z;
	}
	else
	{
		ret.minPt.z = minPt.z;
		ret.maxPt.z = other.maxPt.z;
	}

	return ret;
}

BoundingBox BoundingBox::scale(const Transform & transform) const
{
	Transform applyTransform;
	Vector3 p = transform.pivot();
	applyTransform.setPivot(p.x, p.y, p.z);
	Vector3 s = transform.scaling();
	applyTransform.setScaling(s.x, s.y, s.z);

	Matrix m = applyTransform.computeMatrix();
	Vector3 newMinPt = m * minPt;
	Vector3 newMaxPt = m * maxPt;

	if (newMinPt.x > newMaxPt.x) std::swap(newMinPt.x, newMaxPt.x);
	if (newMinPt.y > newMaxPt.y) std::swap(newMinPt.y, newMaxPt.y);
	if (newMinPt.z > newMaxPt.z) std::swap(newMinPt.z, newMaxPt.z);
	
	return BoundingBox(newMinPt, newMaxPt);
}

float BoundingBox::lengthX() const
{
	return maxPt.x - minPt.x;
}

float BoundingBox::lengthY() const
{
	return maxPt.y - minPt.y;
}

float BoundingBox::lengthZ() const
{
	return maxPt.z - minPt.z;
}

bool BoundingBox::isEmpty() const
{
	return minPt == maxPt;	
}

OBoundingBox BoundingBox::toObb(const Matrix & transform) const
{
	OBoundingBox obb;

	Vector3 origin = transform * minPt;
	
	Vector3 xAxis(maxPt.x - minPt.x, 0.0f, 0.0f);
	Vector3 yAxis(0.0f, maxPt.y - minPt.y, 0.0f);
	Vector3 zAxis(0.0f, 0.0f, maxPt.z - minPt.z);	
	
	xAxis = transform * (xAxis + minPt) - origin;
	yAxis = transform * (yAxis + minPt) - origin;
	zAxis = transform * (zAxis + minPt) - origin;

	obb.xAxis = xAxis;
	obb.yAxis = yAxis;
	obb.zAxis = zAxis;
	obb.origin = origin;
		
	return obb;
}

///////////////////////////////////////////////////////////////////////////////

OBoundingBox OBoundingBox::fromScriptObject(ScriptProcessor * s, JSObject * jsObb)
{
	OBoundingBox ret;
	JSContext * cx = s->jsContext();
	jsval val;

	JS_GetProperty(cx, jsObb, "xAxis", &val);
	ret.xAxis = Vector3::fromScriptObject(s, JSVAL_TO_OBJECT(val));

	JS_GetProperty(cx, jsObb, "yAxis", &val);
	ret.yAxis = Vector3::fromScriptObject(s, JSVAL_TO_OBJECT(val));

	JS_GetProperty(cx, jsObb, "zAxis", &val);
	ret.zAxis = Vector3::fromScriptObject(s, JSVAL_TO_OBJECT(val));

	JS_GetProperty(cx, jsObb, "origin", &val);
	ret.origin = Vector3::fromScriptObject(s, JSVAL_TO_OBJECT(val));

	return ret;
}

Vector3 OBoundingBox::center() const
{
	return origin + 0.5f * (xAxis + yAxis + zAxis);

	return Vector3(
		origin.x + 0.5f * (xAxis.x + yAxis.x + zAxis.x),
		origin.y + 0.5f * (xAxis.y + yAxis.y + zAxis.y),
		origin.z + 0.5f * (xAxis.z + yAxis.z + zAxis.z)
		);

}

bool OBoundingBox::intersect(const OBoundingBox & rhs) const
{	
	Vector3 T = center() - rhs.center();	
	float ra, rb;

	{//1
		const Vector3 & L = xAxis;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}

	{//2
		const Vector3 & L = yAxis;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}

	{//3
		const Vector3 & L = zAxis;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}

	{//4
		const Vector3 & L = rhs.xAxis;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}
	{//5
		const Vector3 & L = rhs.yAxis;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}

	{//6
		const Vector3 & L = rhs.zAxis;
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}
	{//7
		const Vector3 & L = xAxis ^ rhs.xAxis;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}

	{//8
		const Vector3 & L = xAxis ^ rhs.yAxis;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}

	{//9
		const Vector3 & L = xAxis ^ rhs.zAxis;
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}

	{//10
		const Vector3 & L = yAxis ^ rhs.xAxis;
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}

	{//11
		const Vector3 & L = yAxis ^ rhs.yAxis;
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}

	{//12
		const Vector3 & L = yAxis ^ rhs.zAxis;
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}

	{//13
		const Vector3 & L = zAxis ^ rhs.xAxis;
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}

	{//14
		const Vector3 & L =zAxis ^ rhs.yAxis;
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}

	{//15
		const Vector3 & L = zAxis ^ rhs.zAxis;
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		rb = fabs(rhs.xAxis * L) + fabs(rhs.yAxis * L) + fabs(rhs.zAxis * L);
		if (2.0f * fabs(T * L) > ra + rb) return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

inline void minMax(float & fmin, float & fmax, const float & a, const float & b, const float & c)
{
	fmin = FLT_MAX;
	fmax = -FLT_MAX;

	if (a < fmin) fmin = a; if (a > fmax) fmax = a;
	if (b < fmin) fmin = b; if (b > fmax) fmax = b;
	if (c < fmin) fmin = c; if (c > fmax) fmax = c;
}


bool OBoundingBox::intersect(const Vector3 & p0, const Vector3 & p1, const Vector3 & p2) const
{	
	//triangle edges
	
	Vector3 v0 = p1 - p0;
	Vector3 v1 = p2 - p1;
	Vector3 v2 = p2 - p0;

	Vector3 boxCenter = center();

	//triangle normal
	Vector3 N = (p1 - p0) ^ (p2 - p1);	
	
	float ra, rb;
	float fmin, fmax;

	{//1
		const Vector3 & L = N;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		minMax(fmin, fmax, p0 * L,  p1 * L, p2 * L);
		rb = fmax - fmin;		
		if (2.0f * fabs(boxCenter * L - 0.5f * (fmax + fmin)) > ra + rb) return false;
	}

	{//2
		const Vector3 & L = xAxis;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		minMax(fmin, fmax, p0 * L,  p1 * L, p2 * L);
		rb = fmax - fmin;		
		if (2.0f * fabs(boxCenter * L - 0.5f * (fmax + fmin)) > ra + rb) return false;
	}

	{//3
		const Vector3 & L = yAxis;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		minMax(fmin, fmax, p0 * L,  p1 * L, p2 * L);
		rb = fmax - fmin;		
		if (2.0f * fabs(boxCenter * L - 0.5f * (fmax + fmin)) > ra + rb) return false;
	}

	{//4
		const Vector3 & L = zAxis;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		minMax(fmin, fmax, p0 * L,  p1 * L, p2 * L);
		rb = fmax - fmin;		
		if (2.0f * fabs(boxCenter * L - 0.5f * (fmax + fmin)) > ra + rb) return false;
	}
	{//5
		const Vector3 & L = xAxis ^ v0;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		minMax(fmin, fmax, p0 * L,  p1 * L, p2 * L);
		rb = fmax - fmin;		
		if (2.0f * fabs(boxCenter * L - 0.5f * (fmax + fmin)) > ra + rb) return false;
	}

	{//6
		const Vector3 & L = xAxis ^ v1;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		minMax(fmin, fmax, p0 * L,  p1 * L, p2 * L);
		rb = fmax - fmin;		
		if (2.0f * fabs(boxCenter * L - 0.5f * (fmax + fmin)) > ra + rb) return false;
	}
	{//7
		const Vector3 & L = xAxis ^ v2;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		minMax(fmin, fmax, p0 * L,  p1 * L, p2 * L);
		rb = fmax - fmin;		
		if (2.0f * fabs(boxCenter * L - 0.5f * (fmax + fmin)) > ra + rb) return false;
	}

	{//8
		const Vector3 & L = yAxis ^ v0;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		minMax(fmin, fmax, p0 * L,  p1 * L, p2 * L);
		rb = fmax - fmin;		
		if (2.0f * fabs(boxCenter * L - 0.5f * (fmax + fmin)) > ra + rb) return false;
	}

	{//9
		const Vector3 & L = yAxis ^ v1;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		minMax(fmin, fmax, p0 * L,  p1 * L, p2 * L);
		rb = fmax - fmin;		
		if (2.0f * fabs(boxCenter * L - 0.5f * (fmax + fmin)) > ra + rb) return false;
	}

	{//10
		const Vector3 & L = yAxis ^ v2;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		minMax(fmin, fmax, p0 * L,  p1 * L, p2 * L);
		rb = fmax - fmin;		
		if (2.0f * fabs(boxCenter * L - 0.5f * (fmax + fmin)) > ra + rb) return false;
	}

	{//11
		const Vector3 & L = zAxis ^ v0;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		minMax(fmin, fmax, p0 * L,  p1 * L, p2 * L);
		rb = fmax - fmin;		
		if (2.0f * fabs(boxCenter * L - 0.5f * (fmax + fmin)) > ra + rb) return false;
	}

	{//12
		const Vector3 & L = zAxis ^ v1;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		minMax(fmin, fmax, p0 * L,  p1 * L, p2 * L);
		rb = fmax - fmin;		
		if (2.0f * fabs(boxCenter * L - 0.5f * (fmax + fmin)) > ra + rb) return false;
	}

	{//13
		const Vector3 & L = zAxis ^ v2;	
		ra = fabs(xAxis * L) + fabs(yAxis * L) + fabs(zAxis * L);
		minMax(fmin, fmax, p0 * L,  p1 * L, p2 * L);
		rb = fmax - fmin;		
		if (2.0f * fabs(boxCenter * L - 0.5f * (fmax + fmin)) > ra + rb) return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

bool OBoundingBox::intersect(const BoundingBox & aabb) const
{
	const Vector3 & maxPt = aabb.maxPt;
	const Vector3 & minPt = aabb.minPt;

	//TODO optimize for aabb intersection
	OBoundingBox obb;
	obb.xAxis = Vector3(maxPt.x - minPt.x, 0.0f, 0.0f);
	obb.yAxis = Vector3(0.0f, maxPt.y - minPt.y, 0.0f);
	obb.zAxis = Vector3(0.0f, 0.0f, maxPt.z - minPt.z);
	//obb.origin = Vector3(0.0f, 0.0f, 0.0f);
	obb.origin = minPt;

	return intersect(obb);
}
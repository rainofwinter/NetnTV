#pragma once

struct JSClass;
struct JSObject;
class ScriptProcessor;

#ifndef M_E
#define M_E        2.71828182845904523536f
#endif

#ifndef M_LOG2E
#define M_LOG2E    1.44269504088896340736f
#endif

#ifndef M_LOG10E
#define M_LOG10E   0.434294481903251827651f
#endif

#ifndef M_LN2
#define M_LN2      0.693147180559945309417f
#endif

#ifndef M_LN10
#define M_LN10     2.30258509299404568402f
#endif

#ifndef M_PI
#define M_PI       3.14159265358979323846f
#endif

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923f
#endif

#ifndef M_PI_4
#define M_PI_4     0.785398163397448309616f
#endif

#ifndef M_1_PI
#define M_1_PI     0.318309886183790671538f
#endif

#ifndef M_2_PI
#define M_2_PI     0.636619772367581343076f
#endif

#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257390f
#endif

#ifndef M_SQRT2
#define M_SQRT2    1.41421356237309504880f
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2  0.707106781186547524401f
#endif


class Plane;

class Vector3
{
public:
	static JSObject * scriptObjectProto(ScriptProcessor * s, JSObject * global);
	static Vector3 fromScriptObject(ScriptProcessor * s, JSObject * jsTransform);
	JSObject * createScriptObject(ScriptProcessor * s) const;

	float x, y, z;

	Vector3() {}
	Vector3(float x, float y, float z)
	{
		this->x = x; this->y = y; this->z = z;
	}

	float operator * (const Vector3 & rhs) const
	{
		return x*rhs.x + y*rhs.y + z*rhs.z;
	}

	Vector3 & operator *= (float rhs)
	{
		x *= rhs; y *= rhs; z *= rhs;
		return *this;
	}

	Vector3 operator ^ (const Vector3 & rhs) const
	{
		Vector3 ret;
		ret.x = y * rhs.z - z * rhs.y;
		ret.y = z * rhs.x - x * rhs.z;
		ret.z = x * rhs.y - y * rhs.x;
		return ret;
	}

	float magnitude() const
	{
		return sqrt(x*x + y*y + z*z);
	}
	void makeNormal()
	{
		float mag = magnitude();
		x /= mag;
		y /= mag;
		z /= mag;
	};

	float distanceFrom(const Vector3 & rhs) const 
	{
		float a = x - rhs.x, b = y - rhs.y, c = z - rhs.z;
		return sqrt(a*a + b*b + c*c);
	}

	Vector3 normalize() const
	{
		float mag = magnitude();
		if (mag < 0.00001f) return Vector3(0, 0, 0);
		return Vector3(x/mag, y/mag, z/mag);
	}

	Vector3 operator - (const Vector3 & rhs) const
	{
		Vector3 ret;
		ret.x = x - rhs.x;
		ret.y = y - rhs.y;
		ret.z = z - rhs.z;
		return ret;
	}

	Vector3 operator + (const Vector3 & rhs) const
	{
		Vector3 ret;
		ret.x = x + rhs.x;
		ret.y = y + rhs.y;
		ret.z = z + rhs.z;
		return ret;
	}

	Vector3 & operator += (const Vector3 & rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Vector3 & operator -= (const Vector3 & rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	Vector3 & operator /= (float rhs)
	{
		x /= rhs; y /= rhs; z /= rhs;
		return *this;
	}

	/**
	project this vector onto plane defined by vec1 and vec2

	assume the vector and the plane have the same origin	

	Note: a, b, c must be normal
	*/
	void projectOnToPlane(const Vector3 & vec1, const Vector3 & vec2);

	/**
	project vector onto plane.
	Note: (this->a, this->b, this->c) must be normal
	*/
	void projectOnToPlane(const Plane & plane);

	///modify this vector so it is orthonormal to vec2
	void makeOrthonormalTo(const Vector3 & vec2);
		
	bool operator == (const Vector3 & rhs) const 
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}
};

inline Vector3 operator * (float t, const Vector3 & v)
{
	return Vector3(t*v.x, t*v.y, t*v.z);
}

///////////////////////////////////////////////////////////////////////////////

class Vector2
{
public:
	static JSObject * scriptObjectProto(ScriptProcessor * s, JSObject * global);
	static Vector2 fromScriptObject(ScriptProcessor * s, JSObject * jsTransform);
	JSObject * createScriptObject(ScriptProcessor * s) const;


	float x, y;
	Vector2() {}
	Vector2(float u, float v)
	{
		this->x = u; this->y = v;
	}
	Vector2(const Vector3 & v)
	{
		this->x = v.x; this->y = v.y;
	}

	float operator * (const Vector2 & rhs) const
	{
		return x*rhs.x + y*rhs.y;
	}


	bool operator == (const Vector2 & rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}

	bool operator != (const Vector2 & rhs) const
	{
		return x != rhs.x || y != rhs.y;
	}

	Vector2 normalize() const
	{
		float len = magnitude();
		if (len == 0) return Vector2(0, 0);
		return Vector2(x/len, y/len);
	}

	Vector2 operator + (const Vector2 & rhs) const
	{
		return Vector2(x + rhs.x, y + rhs.y);
	}

	Vector2 operator - (const Vector2 & rhs) const
	{
		return Vector2(x - rhs.x, y - rhs.y);
	}

	Vector2 & operator -= (const Vector2 & rhs)
	{
		x -= rhs.x; y -= rhs.y;
		return *this;
	}

	Vector2 & operator += (const Vector2 & rhs)
	{
		x += rhs.x; y += rhs.y;
		return *this;
	}

	Vector2 & operator /= (float rhs)
	{
		x /= rhs; y /= rhs;
		return *this;
	}

	Vector2 & operator *= (float rhs)
	{
		x *= rhs; y *= rhs;
		return *this;
	}

	float magnitude() const {return sqrt(x*x + y*y);}
	
};

inline Vector2 operator * (float t, const Vector2 & v)
{
	return Vector2(t*v.x, t*v.y);
}

///////////////////////////////////////////////////////////////////////////////

class Matrix
{
public:
	static JSObject * scriptObjectProto(ScriptProcessor * s, JSObject * global);
	JSObject * createScriptObject(ScriptProcessor * s) const;
public:

	float _11, _21, _31, _41;
	float _12, _22, _32, _42;
	float _13, _23, _33, _43;
	float _14, _24, _34, _44;
	

	float * vals() {return &_11;}
	const float * vals() const {return &_11;}

	Matrix() {}
	Matrix(float * vals)
	{
		memcpy(&_11, vals, sizeof(float)*16);
	}
	Matrix(const Matrix & rhs);

	bool operator == (const Matrix & rhs) const
	{
		return 
			_11 == rhs._11 && _21 == rhs._21 && _31 == rhs._31 && _41 == rhs._41 &&
			_12 == rhs._12 && _22 == rhs._22 && _32 == rhs._32 && _42 == rhs._42 &&
			_13 == rhs._13 && _23 == rhs._23 && _33 == rhs._33 && _43 == rhs._43 &&
			_14 == rhs._14 && _24 == rhs._24 && _34 == rhs._34 && _44 == rhs._44;
	}

	Vector3 operator * (const Vector3 & rhs) const
	{
		Vector3 ret;
		ret.x = _11 * rhs.x + _12 * rhs.y + _13 * rhs.z + _14;
		ret.y = _21 * rhs.x + _22 * rhs.y + _23 * rhs.z + _24;
		ret.z = _31 * rhs.x + _32 * rhs.y + _33 * rhs.z + _34;
		float w = _41 * rhs.x + _42 * rhs.y + _43 * rhs.z + _44;

		ret.x /= w;
		ret.y /= w;
		ret.z /= w;

		return ret;
	}

	Vector3 multiply(const Vector3 & rhs) const
	{
		Vector3 ret;
		ret.x = _11 * rhs.x + _12 * rhs.y + _13 * rhs.z + _14;
		ret.y = _21 * rhs.x + _22 * rhs.y + _23 * rhs.z + _24;
		ret.z = _31 * rhs.x + _32 * rhs.y + _33 * rhs.z + _34;
		return ret;
	}

	Vector3 multiply3x3(const Vector3 & rhs) const
	{
		Vector3 ret;
		ret.x = _11 * rhs.x + _12 * rhs.y + _13 * rhs.z;
		ret.y = _21 * rhs.x + _22 * rhs.y + _23 * rhs.z;
		ret.z = _31 * rhs.x + _32 * rhs.y + _33 * rhs.z;

		return ret;
	}

	Matrix operator * (const Matrix & rhs) const
	{
		Matrix ret;
			
		ret._11 = _11*rhs._11 + _12*rhs._21 + _13*rhs._31 + _14*rhs._41;
		ret._21 = _21*rhs._11 + _22*rhs._21 + _23*rhs._31 + _24*rhs._41;
		ret._31 = _31*rhs._11 + _32*rhs._21 + _33*rhs._31 + _34*rhs._41;
		ret._41 = _41*rhs._11 + _42*rhs._21 + _43*rhs._31 + _44*rhs._41;

		ret._12 = _11*rhs._12 + _12*rhs._22 + _13*rhs._32 + _14*rhs._42;
		ret._22 = _21*rhs._12 + _22*rhs._22 + _23*rhs._32 + _24*rhs._42;
		ret._32 = _31*rhs._12 + _32*rhs._22 + _33*rhs._32 + _34*rhs._42;
		ret._42 = _41*rhs._12 + _42*rhs._22 + _43*rhs._32 + _44*rhs._42;

		ret._13 = _11*rhs._13 + _12*rhs._23 + _13*rhs._33 + _14*rhs._43;
		ret._23 = _21*rhs._13 + _22*rhs._23 + _23*rhs._33 + _24*rhs._43;
		ret._33 = _31*rhs._13 + _32*rhs._23 + _33*rhs._33 + _34*rhs._43;
		ret._43 = _41*rhs._13 + _42*rhs._23 + _43*rhs._33 + _44*rhs._43;

		ret._14 = _11*rhs._14 + _12*rhs._24 + _13*rhs._34 + _14*rhs._44;
		ret._24 = _21*rhs._14 + _22*rhs._24 + _23*rhs._34 + _24*rhs._44;
		ret._34 = _31*rhs._14 + _32*rhs._24 + _33*rhs._34 + _34*rhs._44;
		ret._44 = _41*rhs._14 + _42*rhs._24 + _43*rhs._34 + _44*rhs._44;

		return ret;
	}

	Matrix Transpose() const
	{
		Matrix ret;
		ret._11 = _11;
		ret._21 = _12;
		ret._31 = _13;
		ret._41 = _14;

		ret._12 = _21;
		ret._22 = _22;
		ret._32 = _23;
		ret._42 = _24;

		ret._13 = _31;
		ret._23 = _32;
		ret._33 = _33;
		ret._43 = _34;

		ret._14 = _41;
		ret._24 = _42;
		ret._34 = _43;
		ret._44 = _44;

		return ret;
	}

	Matrix & operator *= (const Matrix & rhs)
	{
		Matrix res = *this * rhs;
		*this = res;
		return *this;
	}

	Matrix & operator += (const Matrix & rhs)
	{
		Matrix res = *this + rhs;
		*this = res;
		return *this;
	}

	Matrix transpose() const
	{
		Matrix ret = *this;
		
		std::swap(ret._12, ret._21);
		std::swap(ret._13, ret._31);
		std::swap(ret._14, ret._41);
		
		std::swap(ret._23, ret._32);
		std::swap(ret._24, ret._42);
		
		std::swap(ret._34, ret._43);

		return ret;
	}

	Matrix operator + (const Matrix & rhs) const
	{
		Matrix ret;
		const float * lhsVals = vals();
		const float * rhsVals = rhs.vals();
		float * retVals = ret.vals();
		for (int i = 0; i < 16; ++i)
		{
			*(retVals++) = *(lhsVals++) + *(rhsVals++);
		}
		return ret;
	}

	Matrix normalTransformMatrix() const;

	Matrix inverse() const
	{
		//http://www.geometrictools.com/LibMathematics/Algebra/Wm5Matrix4.inl

		const float * mEntry = &_11;
		float a0 = mEntry[ 0]*mEntry[ 5] - mEntry[ 1]*mEntry[ 4];
		float a1 = mEntry[ 0]*mEntry[ 6] - mEntry[ 2]*mEntry[ 4];
		float a2 = mEntry[ 0]*mEntry[ 7] - mEntry[ 3]*mEntry[ 4];
		float a3 = mEntry[ 1]*mEntry[ 6] - mEntry[ 2]*mEntry[ 5];
		float a4 = mEntry[ 1]*mEntry[ 7] - mEntry[ 3]*mEntry[ 5];
		float a5 = mEntry[ 2]*mEntry[ 7] - mEntry[ 3]*mEntry[ 6];
		float b0 = mEntry[ 8]*mEntry[13] - mEntry[ 9]*mEntry[12];
		float b1 = mEntry[ 8]*mEntry[14] - mEntry[10]*mEntry[12];
		float b2 = mEntry[ 8]*mEntry[15] - mEntry[11]*mEntry[12];
		float b3 = mEntry[ 9]*mEntry[14] - mEntry[10]*mEntry[13];
		float b4 = mEntry[ 9]*mEntry[15] - mEntry[11]*mEntry[13];
		float b5 = mEntry[10]*mEntry[15] - mEntry[11]*mEntry[14];

		float det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
		if (fabs(det) > 0.00001f)
		{
			Matrix inverse;
			float * inverse_mEntry = &inverse._11;
			inverse_mEntry[ 0] = + mEntry[ 5]*b5 - mEntry[ 6]*b4 + mEntry[ 7]*b3;
			inverse_mEntry[ 4] = - mEntry[ 4]*b5 + mEntry[ 6]*b2 - mEntry[ 7]*b1;
			inverse_mEntry[ 8] = + mEntry[ 4]*b4 - mEntry[ 5]*b2 + mEntry[ 7]*b0;
			inverse_mEntry[12] = - mEntry[ 4]*b3 + mEntry[ 5]*b1 - mEntry[ 6]*b0;
			inverse_mEntry[ 1] = - mEntry[ 1]*b5 + mEntry[ 2]*b4 - mEntry[ 3]*b3;
			inverse_mEntry[ 5] = + mEntry[ 0]*b5 - mEntry[ 2]*b2 + mEntry[ 3]*b1;
			inverse_mEntry[ 9] = - mEntry[ 0]*b4 + mEntry[ 1]*b2 - mEntry[ 3]*b0;
			inverse_mEntry[13] = + mEntry[ 0]*b3 - mEntry[ 1]*b1 + mEntry[ 2]*b0;
			inverse_mEntry[ 2] = + mEntry[13]*a5 - mEntry[14]*a4 + mEntry[15]*a3;
			inverse_mEntry[ 6] = - mEntry[12]*a5 + mEntry[14]*a2 - mEntry[15]*a1;
			inverse_mEntry[10] = + mEntry[12]*a4 - mEntry[13]*a2 + mEntry[15]*a0;
			inverse_mEntry[14] = - mEntry[12]*a3 + mEntry[13]*a1 - mEntry[14]*a0;
			inverse_mEntry[ 3] = - mEntry[ 9]*a5 + mEntry[10]*a4 - mEntry[11]*a3;
			inverse_mEntry[ 7] = + mEntry[ 8]*a5 - mEntry[10]*a2 + mEntry[11]*a1;
			inverse_mEntry[11] = - mEntry[ 8]*a4 + mEntry[ 9]*a2 - mEntry[11]*a0;
			inverse_mEntry[15] = + mEntry[ 8]*a3 - mEntry[ 9]*a1 + mEntry[10]*a0;

			float invDet = ((float)1)/det;
			inverse_mEntry[ 0] *= invDet;
			inverse_mEntry[ 1] *= invDet;
			inverse_mEntry[ 2] *= invDet;
			inverse_mEntry[ 3] *= invDet;
			inverse_mEntry[ 4] *= invDet;
			inverse_mEntry[ 5] *= invDet;
			inverse_mEntry[ 6] *= invDet;
			inverse_mEntry[ 7] *= invDet;
			inverse_mEntry[ 8] *= invDet;
			inverse_mEntry[ 9] *= invDet;
			inverse_mEntry[10] *= invDet;
			inverse_mEntry[11] *= invDet;
			inverse_mEntry[12] *= invDet;
			inverse_mEntry[13] *= invDet;
			inverse_mEntry[14] *= invDet;
			inverse_mEntry[15] *= invDet;

			return inverse;
		}

		//doesn't have an inverse
		return Identity();

	}

	float Determinant() const;


	static Matrix Identity();
	static Matrix Perspective(float fovY, float aspect, float zNear, float zFar);
	static Matrix Scale(float sx, float sy, float sz);
	static Matrix Scale(const Vector3 & pivot, float sx, float sy, float sz);
	static Matrix Translate(float tx, float ty, float tz);
	static Matrix Rotate(float angle, float x, float y, float z);
	///calculates rotation matrix that will rotate v1 to v2
	static Matrix Rotate(const Vector3 & v1, const Vector3 & v2);
	static Matrix LookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);
	///like gluPickMatrix
	static Matrix pickMatrix(
		float x, float y, float width, float height, const int viewport[4]);
};


inline Matrix operator * (float m, const Matrix & rhs)
{
	Matrix ret;
	float * lhsVal = ret.vals();
	const float * rhsVal = rhs.vals();

	for (int i = 0; i < 16; ++i)
	{
		*(lhsVal++) = m * *(rhsVal++);
	}
	
	return ret;
}


////////////////////////////////////////////////////////////////////////////////

class Ray
{
public:
	Ray() {}
	Ray(const Vector3 & origin, const Vector3 & dir)
	{
		this->origin = origin;
		this->dir = dir;
	}
	Vector3 origin;
	Vector3 dir;

	Ray transform(const Matrix & trans) const
	{
		Ray newRay;
		newRay.origin = trans * origin;
		Vector3 b = trans * (dir + origin);
		newRay.dir = b - newRay.origin;
		return newRay;
	}
};

////////////////////////////////////////////////////////////////////////////////
/**
ax + by + cz + d = 0. 
*/
class Plane
{
public:
	float a, b, c, d;
public:
	static Plane fromPointNormal(const Vector3 & pt, const Vector3 & normal);
	static Plane fromPoints(const Vector3 & pt1, const Vector3 & pt2, const Vector3 &pt3);

	/**
	like D3DXPlaneDotCoord
	*/
	float dotCoord(const Vector3 & pt) const
	{
		return a*pt.x + b*pt.y + c*pt.z + d;
	}

	bool intersectLine(Vector3 * out, const Vector3 & a, const Vector3 & b) const;

	bool intersectLine(Vector3 * out, float * t,
		const Vector3 & pointA, const Vector3 & pointB) const;

	bool intersectSegment(Vector3 * out, float * t,
		const Vector3 & pointA, const Vector3 & pointB) const;

	bool intersect(
		Vector3 * out, const Ray & ray) const;

	bool intersect(
		Vector3 * out, float * t, const Ray & ray) const;

	Vector3 normal() const
	{
		return Vector3(a, b, c).normalize();
	}
};

///////////////////////////////////////////////////////////////////////////////
class Sphere
{
public:
	Sphere(const Vector3 & center, float radius) : center_(center), radius_(radius)
	{}

public:
	/**
	intersects line with sphere and returns result in intPt1 and intPt2.
	intPt1 and intPt2 may be very similar or same for case with one intersection 
	pt. Returns false if no intersection.

	pOut1 is closer to a than pOut2

	Use the infinite line that contains a and b

	*/
	bool intersectLine(Vector3 * intPt1, Vector3 * intPt2, 
		const Vector3 & a, const Vector3 & b) const;

	Vector3 center_;
	float radius_;
};

///////////////////////////////////////////////////////////////////////////////
class Spline
{
public:
//	bool BezierSpline(std::vector<Vector3> & P, Vector3 & Bezier);

	Vector3 BezierSpline(std::vector<Vector3>& points, GLfloat u);
	
//	bool BezierSpline(Vector3 & p1, Vector3 & p2, Vector3 & p3, Vector3 & p4, Vector3 & Bezier);
	
	

	
	//bool intersectLine(Vector3 * intPt1, Vector3 * intPt2, const Vector3 & a, const Vector3 & b) const;
	
};


////////////////////////////////////////////////////////////
inline float RadToDeg(float rad)
{
	return (float)(rad * 180 / M_PI);
}

inline float DegToRad(float deg)
{
	return  (float)(deg * M_PI / 180);
}

/**
return distance of point p from segment a-b
closest point on line is returned in pOut
*/
float distancePointSegment(
	Vector2 * pOut, const Vector2 & p, const Vector2 & a, const Vector2 & b);

float distancePointSegment(
	Vector3 * pOut, const Vector3 & p, const Vector3 & a, const Vector3 & b);

///makes v1 normal to v2 and returns the resulting vector
///resulting vector will be unit length
Vector3 makeNormalTo(const Vector3 & v1, const Vector3 & v2);


bool triangleIntersectRay(Vector3 * pOut, 
	const Vector3 & v0, const Vector3 & v1, const Vector3 & v2, 
	const Ray & ray);


bool triangleIntersectRay(Vector3 * pOut, float * tOut, 
	const Vector3 & v0, const Vector3 & v1, const Vector3 & v2, 
	const Ray & ray);


bool triangleIntersectSegment(Vector3 * pOut, float * tOut,
	const Vector3 & v0, const Vector3 & v1, const Vector3 & v2,
	const Vector3 & p0, const Vector3 & p1);

bool triangleIntersectLine(Vector3 * pOut, float * tOut,
	const Vector3 & v0, const Vector3 & v1, const Vector3 & v2,
	const Vector3 & p0, const Vector3 & p1);

float roundFloat(float val);


float randf();
float triangleArea(const Vector3 & p0, const Vector3 & p1, const Vector3 & p2);
Vector3 triangleRandomPoint(const Vector3 & p0, const Vector3 & p1, const Vector3 & p2);


#include "stdafx.h"
#include "MeshPart.h"
#include "Material.h"
#include "MathStuff.h"
#include "Mesh.h"
#include "GfxRenderer.h"
#include "Reader.h"
#include "Writer.h"
#include "Camera.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

void MeshPart::create()
{
	material_ = 0;
	indexBuffer_ = 0;
	doAlphaBlend_ = false;
}

MeshPart::MeshPart()
{	
	create();
}

MeshPart::~MeshPart()
{
	uninit();

}

MeshPart::MeshPart(const MeshPart & rhs)
{
	create();
	material_ = rhs.material_;
	doAlphaBlend_ = rhs.doAlphaBlend_;
}

MeshPart & MeshPart::setIndices(const std::vector<unsigned short> & indices)
{
	indices_ = indices;
	return *this;
}

MeshPart & MeshPart::setMaterial(Material * material)
{
	material_ = material;
	return *this;
}

void MeshPart::init(GfxRenderer * gl)
{	
	uninit();
	if (!indexBuffer_)
	{
		glGenBuffers(1, &indexBuffer_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);

		if (doAlphaBlend_)
		{
			glBufferData(
				GL_ELEMENT_ARRAY_BUFFER, 
				sizeof(unsigned short) * indices_.size(), 
				&indices_[0], GL_DYNAMIC_DRAW);
		}
		else
		{
			glBufferData(
				GL_ELEMENT_ARRAY_BUFFER, 
				sizeof(unsigned short) * indices_.size(), 
				&indices_[0], GL_STATIC_DRAW);
		}
		
	}
	
}

void MeshPart::uninit()
{
	if (indexBuffer_) 
	{
		glDeleteBuffers(1, &indexBuffer_);
		indexBuffer_ = 0;
	}
}

void MeshPart::draw(GfxRenderer * gl, const  Mesh * mesh, bool useMeshShader)
{
	gl->bindElementArrayBuffer(indexBuffer_);	

	if (doAlphaBlend_)
	{
		glDepthMask(false);
		mvpMatrix_ = gl->computeMvpMatrix();

		/*
		sort(mesh);		
		*/

		sort(mesh);		
		sortRefine1(mesh, 16);	

		/*
		sort(mesh);		
		sortRefine1(mesh, 16);		
		sortRefine(mesh, 8);		
		*/
		

		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
			sizeof(unsigned short) * indices_.size(), 
			&indices_[0]);
		
	}

	if (useMeshShader) material_->Use(gl);
	gl->applyCurrentShaderMatrix();
	glDrawElements( GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, 0);

	if (doAlphaBlend_) glDepthMask(true);
}

bool MeshPart::intersect(float * tOut, const Mesh * mesh, const Ray & ray) const
{
	float t;
	float minT = FLT_MAX;


	int index = 0;
	Vector3 intPt;
	Vector3 v1, v2, v3;
	for (unsigned int i = 0; i < indices_.size() / 3; ++i)
	{
		
		v1 = mesh->deformedVertices_[indices_[index++]].position;
		v2 = mesh->deformedVertices_[indices_[index++]].position;
		v3 = mesh->deformedVertices_[indices_[index++]].position;
		bool intersect = triangleIntersectRay(&intPt, &t, 
			v1, v2, v3, ray);

		/*
		very strange... the following code doesn't work in release build
		However there is no problem in debug build...

		bool intersect = triangleIntersectRay(&intPt, &t, 
			trans*mesh->vertices_[indices_[index++]], 
			trans*mesh->vertices_[indices_[index++]], 
			trans*mesh->vertices_[indices_[index++]],
			ray);			
		*/

		if (intersect) 
		{
			if (t < minT) minT = t;	
		}
	}

	if (minT < FLT_MAX)
	{
		*tOut = minT;
		return true;
	}

	return false;
}


bool MeshPart::intersectLine(float * tOut, const Mesh * mesh, const Vector3 & a, const Vector3 & b) const
{
	float t;
	float minT = FLT_MAX;


	int index = 0;
	Vector3 intPt;
	Vector3 v1, v2, v3;
	for (unsigned int i = 0; i < indices_.size() / 3; ++i)
	{
		
		v1 = mesh->deformedVertices_[indices_[index++]].position;
		v2 = mesh->deformedVertices_[indices_[index++]].position;
		v3 = mesh->deformedVertices_[indices_[index++]].position;
		bool intersect = triangleIntersectLine(&intPt, &t, 
			v1, v2, v3, a, b);	

		if (intersect) 
		{
			if (t < minT) minT = t;	
		}
	}

	if (minT < FLT_MAX)
	{
		*tOut = minT;
		return true;
	}

	return false;
}

int MeshPart::numTriangles() const
{
	return indices_.size()/3;
}

void MeshPart::write(Writer & writer) const
{
	writer.write(indices_);
	writer.write(material_);	
	writer.write(doAlphaBlend_);
}
void MeshPart::read(Reader & reader, unsigned char)
{
	reader.read(indices_);
	reader.read(material_);
	reader.read(doAlphaBlend_);
}

template <typename VectorType>
bool pointInTriangle2D(const VectorType & pt, 
	const VectorType & A, const VectorType & B, const VectorType & C)
{
	//static const float BIAS = 0.00001f;
	/*
	We want point in triangle to fail if a point is right on one of the 
	triangle corners. As is the case of triangles adjacent to each other.
	*/
	static const float BIAS = -0.00001f;

	VectorType v0 = C - A;
	VectorType v1 = B - A;
	VectorType v2 = pt - A;

	float dot00 = v0 * v0;
	float dot01 = v0 * v1;
	float dot02 = v0 * v2;
	float dot11 = v1 * v1;
	float dot12 = v1 * v2;

	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	return (u > -BIAS) && (v > -BIAS) && (u + v < 1.0f + BIAS);
}

/**
http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
*/
template<typename VectorType>
bool segmentIntersectSegment2D(
	VectorType * pOut,
	const VectorType & u1, const VectorType & u2, const VectorType & v1, const VectorType & v2)
{
	//static const float BIAS = 0.00001f; 
	/*
	We want test to fail if a point is right on one of the 
	triangle corners. As is the case of triangles adjacent to each other.
	*/
	static const float BIAS = -0.00001f;

	float D = (v2.y - v1.y) * (u2.x - u1.x) - (v2.x - v1.x) * (u2.y - u1.y);
	
	if (fabs(D) < BIAS) return false;

	float s = ((v2.x - v1.x) * (u1.y - v1.y) - (v2.y - v1.y) * (u1.x - v1.x)) / D;
	float t = ((u2.x - u1.x) * (u1.y - v1.y) - (u2.y - u1.y) * (u1.x - v1.x)) / D;

	
	if (s > 1 + BIAS || s < -BIAS) return false;
	if (t > 1 + BIAS || t < -BIAS) return false;
	

	/*
	if (s >= 1.0f || s <= 0.0f) return false;
	if (t >= 1.0f || t <= 0.0f) return false;
*/


	pOut->x = u1.x + s * (u2.x - u1.x);
	pOut->y = u1.y + s * (u2.y - u1.y);

	return true;
}

/**
triangle interpolation using Barycentric coords

this is 2d interpolation.

The value being interpolated is the z coords of the three triangle verts.
http://en.wikipedia.org/wiki/Barycentric_coordinate_system_%28mathematics%29#Barycentric_coordinates_on_triangles
*/
template<typename VectorType>
float triangleInterpolation2D(
	const VectorType & pt,
	const Vector3 & r1, const Vector3 & r2, const Vector3 & r3)
{
	float denom = (r2.y - r3.y) * (r1.x - r3.x) + (r3.x - r2.x) * (r1.y - r3.y);

	//get the barycentric coords of pt
	float l1 = ((r2.y - r3.y) * (pt.x - r3.x) + (r3.x - r2.x) * (pt.y - r3.y))/denom;
	float l2 = ((r3.y - r1.y) * (pt.x - r3.x) + (r1.x - r3.x) * (pt.y - r3.y))/denom;
	float l3 = 1.0f - l1 - l2;

	return l1 * r1.z + l2 * r2.z + l3* r3.z;
}


//-----------------------------------------------------------------------------
int MeshPart::geoCompare(
	const Vector3 & lhs0, const Vector3 & lhs1, const Vector3 & lhs2,
	const Vector3 & rhs0, const Vector3 & rhs1, const Vector3 & rhs2)
{
	//do quick bb intersection test first
	Vector3 lhsBbMin = lhs0, lhsBbMax = lhs0;
	Vector3 rhsBbMin = rhs0, rhsBbMax = rhs0;

	if (lhs1.x < lhsBbMin.x) lhsBbMin.x = lhs1.x;
	if (lhs2.x < lhsBbMin.x) lhsBbMin.x = lhs2.x;
	if (lhs1.y < lhsBbMin.y) lhsBbMin.y = lhs1.y;
	if (lhs2.y < lhsBbMin.y) lhsBbMin.y = lhs2.y;
	if (lhs1.z < lhsBbMin.z) lhsBbMin.z = lhs1.z;
	if (lhs2.z < lhsBbMin.z) lhsBbMin.z = lhs2.z;

	if (lhs1.x > lhsBbMax.x) lhsBbMax.x = lhs1.x;
	if (lhs2.x > lhsBbMax.x) lhsBbMax.x = lhs2.x;
	if (lhs1.y > lhsBbMax.y) lhsBbMax.y = lhs1.y;
	if (lhs2.y > lhsBbMax.y) lhsBbMax.y = lhs2.y;
	if (lhs1.z > lhsBbMax.z) lhsBbMax.z = lhs1.z;
	if (lhs2.z > lhsBbMax.z) lhsBbMax.z = lhs2.z;

	if (rhs1.x < rhsBbMin.x) rhsBbMin.x = rhs1.x;
	if (rhs2.x < rhsBbMin.x) rhsBbMin.x = rhs2.x;
	if (rhs1.y < rhsBbMin.y) rhsBbMin.y = rhs1.y;
	if (rhs2.y < rhsBbMin.y) rhsBbMin.y = rhs2.y;
	if (rhs1.z < rhsBbMin.z) rhsBbMin.z = rhs1.z;
	if (rhs2.z < rhsBbMin.z) rhsBbMin.z = rhs2.z;

	if (rhs1.x > rhsBbMax.x) rhsBbMax.x = rhs1.x;
	if (rhs2.x > rhsBbMax.x) rhsBbMax.x = rhs2.x;
	if (rhs1.y > rhsBbMax.y) rhsBbMax.y = rhs1.y;
	if (rhs2.y > rhsBbMax.y) rhsBbMax.y = rhs2.y;
	if (rhs1.z > rhsBbMax.z) rhsBbMax.z = rhs1.z;
	if (rhs2.z > rhsBbMax.z) rhsBbMax.z = rhs2.z;
	
	BoundingBox lhsBb(lhsBbMin, lhsBbMax);
	BoundingBox rhsBb(rhsBbMin, rhsBbMax);
	
	if (!lhsBb.doesIntersect(rhsBb)) return 0;

	static vector<Vector3> intRegionPts;
	intRegionPts.clear();

	//line - line intersections
	Vector3 intPt;
	if (segmentIntersectSegment2D(&intPt, lhs0, lhs1, rhs0, rhs1)) intRegionPts.push_back(intPt);
	if (segmentIntersectSegment2D(&intPt, lhs0, lhs1, rhs1, rhs2)) intRegionPts.push_back(intPt);
	if (segmentIntersectSegment2D(&intPt, lhs0, lhs1, rhs0, rhs2)) intRegionPts.push_back(intPt);
	if (segmentIntersectSegment2D(&intPt, lhs1, lhs2, rhs0, rhs1)) intRegionPts.push_back(intPt);
	if (segmentIntersectSegment2D(&intPt, lhs1, lhs2, rhs1, rhs2)) intRegionPts.push_back(intPt);
	if (segmentIntersectSegment2D(&intPt, lhs1, lhs2, rhs0, rhs2)) intRegionPts.push_back(intPt);	
	if (segmentIntersectSegment2D(&intPt, lhs0, lhs2, rhs0, rhs1)) intRegionPts.push_back(intPt);
	if (segmentIntersectSegment2D(&intPt, lhs0, lhs2, rhs1, rhs2)) intRegionPts.push_back(intPt);
	if (segmentIntersectSegment2D(&intPt, lhs0, lhs2, rhs0, rhs2)) intRegionPts.push_back(intPt);

	//pt in triangle tests
	if (pointInTriangle2D(lhs0, rhs0, rhs1, rhs2)) intRegionPts.push_back(lhs0);
	if (pointInTriangle2D(lhs1, rhs0, rhs1, rhs2)) intRegionPts.push_back(lhs1);
	if (pointInTriangle2D(lhs2, rhs0, rhs1, rhs2)) intRegionPts.push_back(lhs2);
	if (pointInTriangle2D(rhs0, lhs0, lhs1, lhs2)) intRegionPts.push_back(rhs0);
	if (pointInTriangle2D(rhs1, lhs0, lhs1, lhs2)) intRegionPts.push_back(rhs1);
	if (pointInTriangle2D(rhs2, lhs0, lhs1, lhs2)) intRegionPts.push_back(rhs2);
	
	if (intRegionPts.empty())
	{
		return 0;
	}

	Vector2 midPt(0.0f, 0.0f);
	for (int i = 0; i < (int)intRegionPts.size(); ++i)
	{
		midPt.x += intRegionPts[i].x;
		midPt.y += intRegionPts[i].y;
	}
	midPt.x /= (int)intRegionPts.size();
	midPt.y /= (int)intRegionPts.size();

	float lhsZ = triangleInterpolation2D(midPt, lhs0, lhs1, lhs2);
	float rhsZ = triangleInterpolation2D(midPt, rhs0, rhs1, rhs2);

	if (lhsZ < rhsZ) return -1;
	else return 1;
}



/**
Do bb test first, then geoCompareSub.
Quicker than geoCompare but does not handle as well cases where tris intersect 
somewhat.
*/
int MeshPart::geoCompare1(
	const Vector3 & lhs0, const Vector3 & lhs1, const Vector3 & lhs2,
	const Vector3 & rhs0, const Vector3 & rhs1, const Vector3 & rhs2)
{
	//do quick bb intersection test first
	Vector3 lhsBbMin = lhs0, lhsBbMax = lhs0;
	Vector3 rhsBbMin = rhs0, rhsBbMax = rhs0;

	if (lhs1.x < lhsBbMin.x) lhsBbMin.x = lhs1.x;
	if (lhs2.x < lhsBbMin.x) lhsBbMin.x = lhs2.x;
	if (lhs1.y < lhsBbMin.y) lhsBbMin.y = lhs1.y;
	if (lhs2.y < lhsBbMin.y) lhsBbMin.y = lhs2.y;
	if (lhs1.z < lhsBbMin.z) lhsBbMin.z = lhs1.z;
	if (lhs2.z < lhsBbMin.z) lhsBbMin.z = lhs2.z;

	if (lhs1.x > lhsBbMax.x) lhsBbMax.x = lhs1.x;
	if (lhs2.x > lhsBbMax.x) lhsBbMax.x = lhs2.x;
	if (lhs1.y > lhsBbMax.y) lhsBbMax.y = lhs1.y;
	if (lhs2.y > lhsBbMax.y) lhsBbMax.y = lhs2.y;
	if (lhs1.z > lhsBbMax.z) lhsBbMax.z = lhs1.z;
	if (lhs2.z > lhsBbMax.z) lhsBbMax.z = lhs2.z;

	if (rhs1.x < rhsBbMin.x) rhsBbMin.x = rhs1.x;
	if (rhs2.x < rhsBbMin.x) rhsBbMin.x = rhs2.x;
	if (rhs1.y < rhsBbMin.y) rhsBbMin.y = rhs1.y;
	if (rhs2.y < rhsBbMin.y) rhsBbMin.y = rhs2.y;
	if (rhs1.z < rhsBbMin.z) rhsBbMin.z = rhs1.z;
	if (rhs2.z < rhsBbMin.z) rhsBbMin.z = rhs2.z;

	if (rhs1.x > rhsBbMax.x) rhsBbMax.x = rhs1.x;
	if (rhs2.x > rhsBbMax.x) rhsBbMax.x = rhs2.x;
	if (rhs1.y > rhsBbMax.y) rhsBbMax.y = rhs1.y;
	if (rhs2.y > rhsBbMax.y) rhsBbMax.y = rhs2.y;
	if (rhs1.z > rhsBbMax.z) rhsBbMax.z = rhs1.z;
	if (rhs2.z > rhsBbMax.z) rhsBbMax.z = rhs2.z;
	
	BoundingBox lhsBb(lhsBbMin, lhsBbMax);
	BoundingBox rhsBb(rhsBbMin, rhsBbMax);
	
	if (!lhsBb.doesIntersect(rhsBb)) return 0;

	//line - line intersections
	Vector3 intPt;

	if (segmentIntersectSegment2D(&intPt, lhs0, lhs1, rhs0, rhs1)) goto segInt;
	if (segmentIntersectSegment2D(&intPt, lhs0, lhs1, rhs1, rhs2)) goto segInt;
	if (segmentIntersectSegment2D(&intPt, lhs0, lhs1, rhs0, rhs2)) goto segInt;
	if (segmentIntersectSegment2D(&intPt, lhs1, lhs2, rhs0, rhs1)) goto segInt;
	if (segmentIntersectSegment2D(&intPt, lhs1, lhs2, rhs1, rhs2)) goto segInt;
	if (segmentIntersectSegment2D(&intPt, lhs1, lhs2, rhs0, rhs2)) goto segInt;
	if (segmentIntersectSegment2D(&intPt, lhs0, lhs2, rhs0, rhs1)) goto segInt;
	if (segmentIntersectSegment2D(&intPt, lhs0, lhs2, rhs1, rhs2)) goto segInt;
	if (segmentIntersectSegment2D(&intPt, lhs0, lhs2, rhs0, rhs2)) goto segInt;

	//pt in triangle tests
	if (pointInTriangle2D(lhs0, rhs0, rhs1, rhs2)) 
	{
		intPt = lhs0;
		goto segInt;
	}
	if (pointInTriangle2D(rhs0, lhs0, lhs1, lhs2))
	{
		intPt = rhs0;
		goto segInt;
	}
	
	return 0;

segInt:

	float lhsZ = triangleInterpolation2D(intPt, lhs0, lhs1, lhs2);
	float rhsZ = triangleInterpolation2D(intPt, rhs0, rhs1, rhs2);


	if (lhsZ < rhsZ) return -1;
	else return 1;
}

/**
Approximate but faster version of geoCompare
*/
int MeshPart::geoCompareApproxBb(
	const Vector3 & lhs0, const Vector3 & lhs1, const Vector3 & lhs2,
	const Vector3 & rhs0, const Vector3 & rhs1, const Vector3 & rhs2)
{
	Vector3 lhsBbMin = lhs0, lhsBbMax = lhs0;
	Vector3 rhsBbMin = rhs0, rhsBbMax = rhs0;

	if (lhs1.x < lhsBbMin.x) lhsBbMin.x = lhs1.x;
	if (lhs2.x < lhsBbMin.x) lhsBbMin.x = lhs2.x;
	if (lhs1.y < lhsBbMin.y) lhsBbMin.y = lhs1.y;
	if (lhs2.y < lhsBbMin.y) lhsBbMin.y = lhs2.y;
	if (lhs1.z < lhsBbMin.z) lhsBbMin.z = lhs1.z;
	if (lhs2.z < lhsBbMin.z) lhsBbMin.z = lhs2.z;

	if (lhs1.x > lhsBbMax.x) lhsBbMax.x = lhs1.x;
	if (lhs2.x > lhsBbMax.x) lhsBbMax.x = lhs2.x;
	if (lhs1.y > lhsBbMax.y) lhsBbMax.y = lhs1.y;
	if (lhs2.y > lhsBbMax.y) lhsBbMax.y = lhs2.y;
	if (lhs1.z > lhsBbMax.z) lhsBbMax.z = lhs1.z;
	if (lhs2.z > lhsBbMax.z) lhsBbMax.z = lhs2.z;

	if (rhs1.x < rhsBbMin.x) rhsBbMin.x = rhs1.x;
	if (rhs2.x < rhsBbMin.x) rhsBbMin.x = rhs2.x;
	if (rhs1.y < rhsBbMin.y) rhsBbMin.y = rhs1.y;
	if (rhs2.y < rhsBbMin.y) rhsBbMin.y = rhs2.y;
	if (rhs1.z < rhsBbMin.z) rhsBbMin.z = rhs1.z;
	if (rhs2.z < rhsBbMin.z) rhsBbMin.z = rhs2.z;

	if (rhs1.x > rhsBbMax.x) rhsBbMax.x = rhs1.x;
	if (rhs2.x > rhsBbMax.x) rhsBbMax.x = rhs2.x;
	if (rhs1.y > rhsBbMax.y) rhsBbMax.y = rhs1.y;
	if (rhs2.y > rhsBbMax.y) rhsBbMax.y = rhs2.y;
	if (rhs1.z > rhsBbMax.z) rhsBbMax.z = rhs1.z;
	if (rhs2.z > rhsBbMax.z) rhsBbMax.z = rhs2.z;
	
	BoundingBox lhsBb(lhsBbMin, lhsBbMax);
	BoundingBox rhsBb(rhsBbMin, rhsBbMax);
	
	if (!lhsBb.doesIntersect(rhsBb)) return 0;

	Vector2 intMin, intMax;
	if (lhsBbMin.x < rhsBbMin.x)
	{
		intMin.x = rhsBbMin.x;
		intMax.x = lhsBbMax.x;
	}
	else
	{
		intMin.x = lhsBbMin.x;
		intMax.x = rhsBbMax.x;
	}

	if (lhsBbMin.y < rhsBbMin.y)
	{
		intMin.y = rhsBbMin.y;
		intMax.y = lhsBbMax.y;
	}
	else
	{
		intMin.y = lhsBbMin.y;
		intMax.y = rhsBbMax.y;
	}

	Vector2 midPt(
		0.5f * (intMin.x + intMax.x), 
		0.5f * (intMin.y + intMax.y));

	float lhsZ = triangleInterpolation2D(midPt, lhs0, lhs1, lhs2);
	float rhsZ = triangleInterpolation2D(midPt, rhs0, rhs1, rhs2);

	if (lhsZ < rhsZ) return -1;
	else return 1;

}



int MeshPart::compare(const Mesh * mesh, const unsigned short * lhs, const unsigned short * rhs) const
{
	const std::vector<VertexData> & vertices = mesh->vertices();

	Vector3 lhs0 = mvpMatrix_ * vertices[lhs[0]].position;
	Vector3 lhs1 = mvpMatrix_ * vertices[lhs[1]].position;
	Vector3 lhs2 = mvpMatrix_ * vertices[lhs[2]].position;
	Vector3 rhs0 = mvpMatrix_ * vertices[rhs[0]].position;
	Vector3 rhs1 = mvpMatrix_ * vertices[rhs[1]].position;
	Vector3 rhs2 = mvpMatrix_ * vertices[rhs[2]].position;

	Vector3 lhsTriMid = (1.0f/3.0f)*(lhs0 + lhs1 + lhs2);
	Vector3 rhsTriMid = (1.0f/3.0f)*(rhs0 + rhs1 + rhs2);

	return (lhsTriMid.z > rhsTriMid.z)?1:-1;
}

inline void triSwap(unsigned short * lhs, unsigned short * rhs)
{
	std::swap(lhs[0], rhs[0]);
	std::swap(lhs[1], rhs[1]);
	std::swap(lhs[2], rhs[2]);
}

inline void triAssign(unsigned short * lhs, unsigned short * rhs)
{
	lhs[0] = rhs[0];
	lhs[1] = rhs[1];
	lhs[2] = rhs[2];
}

void MeshPart::sort(const Mesh * mesh)
{	
	int numTris = (int)indices_.size()/ 3;

	//quick sort
	sort(mesh, 0, (int)(indices_.size() / 3 - 1));
}


void MeshPart::sort(const Mesh * mesh, int left, int right)
{
	if (right > left)
	{
		int pivotIndex = (right + left) / 2;
		int pivotNewIndex = partition(mesh, left, right, pivotIndex);
		sort(mesh, left, pivotNewIndex - 1);
		sort(mesh, pivotNewIndex + 1, right);
	}
}



int MeshPart::partition(const Mesh * mesh, int left, int right, int pivotIndex)
{
	unsigned short pivotValue[3];
	triAssign(pivotValue, &indices_[3*pivotIndex]);
	
	triSwap(&indices_[3*pivotIndex], &indices_[3*right]);

	int storeIndex = left;
	bool doLeftThisTime = true;
	for (int i = left; i < right; ++i)
	{
		bool shouldBeLeft = false;
		if (compare(mesh, &indices_[3*i], pivotValue) == 1)
		{
			shouldBeLeft = true;		
		}

		if (shouldBeLeft)
		{
			triSwap(&indices_[3*i], &indices_[3*storeIndex]);
			storeIndex = storeIndex + 1;
		}
	}


	triSwap(&indices_[3*storeIndex], &indices_[3*right]);
	return storeIndex;
}


void MeshPart::sortRefine(const Mesh * mesh, int spanSize)
{
	const std::vector<VertexData> & vertices = mesh->vertices();

	int numTris = (int)indices_.size()/ 3;

	Vector3 rhs0, rhs1, rhs2;

	for (int i = numTris - 1; i >= 0; --i)
	{
		unsigned short * rhs = &indices_[3*i];
		rhs0 = mvpMatrix_ * vertices[rhs[0]].position;
		rhs1 = mvpMatrix_ * vertices[rhs[1]].position;
		rhs2 = mvpMatrix_ * vertices[rhs[2]].position;

		for (int j = i - 1; j >= 0 && j >= i - spanSize + 1; --j)
		{
			unsigned short * lhs = &indices_[3*j];
			
			Vector3 lhs0 = mvpMatrix_ * vertices[lhs[0]].position;
			Vector3 lhs1 = mvpMatrix_ * vertices[lhs[1]].position;
			Vector3 lhs2 = mvpMatrix_ * vertices[lhs[2]].position;

			if (geoCompare(lhs0, lhs1, lhs2, rhs0, rhs1, rhs2) < 0) 
			{
				triSwap(rhs, lhs);

				rhs0 = lhs0;
				rhs1 = lhs1;
				rhs2 = lhs2;
			}
		}		
	}
}

void MeshPart::sortRefine1(const Mesh * mesh, int spanSize)
{
	const std::vector<VertexData> & vertices = mesh->vertices();

	int numTris = (int)indices_.size()/ 3;

	Vector3 rhs0, rhs1, rhs2;

	for (int i = numTris - 1; i >= 0; --i)
	{
		unsigned short * rhs = &indices_[3*i];
		rhs0 = mvpMatrix_ * vertices[rhs[0]].position;
		rhs1 = mvpMatrix_ * vertices[rhs[1]].position;
		rhs2 = mvpMatrix_ * vertices[rhs[2]].position;

		for (int j = i - 1; j >= 0 && j >= i - spanSize + 1; --j)
		{
			unsigned short * lhs = &indices_[3*j];
			
			Vector3 lhs0 = mvpMatrix_ * vertices[lhs[0]].position;
			Vector3 lhs1 = mvpMatrix_ * vertices[lhs[1]].position;
			Vector3 lhs2 = mvpMatrix_ * vertices[lhs[2]].position;

			if (geoCompare1(lhs0, lhs1, lhs2, rhs0, rhs1, rhs2) < 0) 
			{
				triSwap(rhs, lhs);

				rhs0 = lhs0;
				rhs1 = lhs1;
				rhs2 = lhs2;
			}
		}		
	}
}

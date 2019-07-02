#pragma once
#include "MathStuff.h"

///////////////////////////////////////////////////////////////////////////////
class Material;
class GfxRenderer;
class Vector3;
class Mesh;
class Ray;
class Reader;
class Writer;

class MeshPart
{
	
public:
	MeshPart();
	MeshPart(const MeshPart & rhs);
	~MeshPart();

	MeshPart & setIndices(const std::vector<unsigned short> & indices);
	const std::vector<unsigned short> & indices() const {return indices_;}
	MeshPart & setMaterial(Material * material);

	void init(GfxRenderer * gl);
	void uninit();

	void draw(GfxRenderer * gl, const Mesh * mesh, bool useMeshShader = true);

	bool doAlphaBlend() const {return doAlphaBlend_;}
	void setDoAlphaBlend(bool val) {doAlphaBlend_ = val;}

	Material * material() const {return material_;}

	bool intersect(float * t, const Mesh * mesh, const Ray & ray) const;
	bool intersectLine(float * t, const Mesh * mesh, const Vector3 & a, const Vector3 & b) const;

	int numTriangles() const;

	unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
private:
	
	void create();

	class Triangle
	{
	public:
		unsigned short index0;
		unsigned short index1;
		unsigned short index2;
	};

	/**
	@param invCameraPoint position of camera in the same coordinate space as the
	mesh vertices
	*/
	void sort(const Mesh * mesh);
	void sort(const Mesh * mesh, int left, int right);
	int partition(const Mesh * mesh, int left, int right, int pivotIndex);

	void sortRefine(const Mesh * mesh, int spanSize);

	/**
	Quicker than sortRefine but does not handle as well cases where tris 
	intersect somewhat.
	*/
	void sortRefine1(const Mesh * mesh, int spanSize);

	/**
	@return 1 if lhs should come before rhs, 0 if uncertain (no overlap when renderered), 
	-1 if rhs should come before lhs

	*/
	int compare(const Mesh * mesh, 
		const unsigned short * lhs, const unsigned short * rhs) const;

	/**
	@return 0: inconclusive, -1: lhs should be in front, 1: rhs should be in front
	*/
	int geoCompare(
		const Vector3 & lhs0, const Vector3 & lhs1, const Vector3 & lhs2,
		const Vector3 & rhs0, const Vector3 & rhs1, const Vector3 & rhs2
		);

	int geoCompare1(
		const Vector3 & lhs0, const Vector3 & lhs1, const Vector3 & lhs2,
		const Vector3 & rhs0, const Vector3 & rhs1, const Vector3 & rhs2
		);

	int geoCompareApproxBb(
		const Vector3 & lhs0, const Vector3 & lhs1, const Vector3 & lhs2,
		const Vector3 & rhs0, const Vector3 & rhs1, const Vector3 & rhs2
		);

	Matrix mvpMatrix_;

	MeshPart & operator = (const MeshPart & rhs);


	std::vector<unsigned short> indices_;
	Material * material_;	
	GLuint indexBuffer_;

	bool doAlphaBlend_;

};
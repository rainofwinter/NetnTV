#pragma once
#include "MathStuff.h"
#include "BoundingBox.h"
#include "Model.h"

class Model;
class ModelFile;
class SceneObject;
class BoundingBoxObject;

class KDTreeTriList
{
public:
	KDTreeTriList();
	void uninit();

	unsigned short meshIndex;
	unsigned short meshPartIndex;
	unsigned short numTris;
	unsigned short * triIndices;
};

///////////////////////////////////////////////////////////////////////////////

class KDTreeNode
{
public:
	KDTreeNode();
	void uninit();

	static const unsigned char NONE = 0;
	static const unsigned char X = 1;
	static const unsigned char Y = 2;
	static const unsigned char Z = 3;

	unsigned char direction;
	float pos;

	///0 for no child node
	unsigned int leftNode;
	unsigned int rightNode;

	unsigned short numTriLists;
	KDTreeTriList * triLists;
};

///////////////////////////////////////////////////////////////////////////////

struct IntTriData
{
	unsigned short meshIndex;
	unsigned short meshPartIndex;
	unsigned short triIndex;
	Vector3 normal;

	bool operator < (const IntTriData & rhs) const 
	{
		if (meshIndex != rhs.meshIndex) return meshIndex < rhs.meshIndex;
		if (meshPartIndex != rhs.meshPartIndex) return meshPartIndex < rhs.meshPartIndex;
		return triIndex < rhs.triIndex;
	}
};

struct IntTriData0
{
	unsigned short meshIndex;
	unsigned short meshPartIndex;
	unsigned short triIndex;

	bool operator < (const IntTriData0 & rhs) const 
	{
		if (meshIndex != rhs.meshIndex) return meshIndex < rhs.meshIndex;
		if (meshPartIndex != rhs.meshPartIndex) return meshPartIndex < rhs.meshPartIndex;
		return triIndex < rhs.triIndex;
	}
};


class KDTree
{
public:
	static JSObject * createScriptObjectProto(ScriptProcessor * s, JSObject * global);
	JSObject * getScriptObject(ScriptProcessor * s);

	KDTree(int numLeafTris);
	~KDTree();

	const std::vector<KDTreeNode> & nodes() const {return nodes_;}

	/**
	All meshes need to have identity transform and not be animated
	*/
	void genFromModel(SceneObject * wrapperObj, Model * model);

	bool intersect(std::set<IntTriData> * intTriData, const BoundingBoxObject * bbObject) const;
	bool intersect(std::set<IntTriData> * intTriData, const OBoundingBox & obb) const;
	bool intersect(Vector3 * intPt, Vector3 * normal, const Vector3 & pt0, const Vector3 & pt1) const;

	bool childNodeAabb(const BoundingBox & parentAabb, int nodeIndex, BoundingBox * leftAabb, BoundingBox * rightAabb) const;

	/**
	Get the mesh triangles that are within aabb
	*/
	void getIntersectingTriangles(std::set<IntTriData0> * intTriData, const OBoundingBox & obb) const;
	
	/**
	Same as getIntersectingTriangles, but obb is in local coords, so no mucking
	about with coordinate transforms necessary
	*/
	void getIntersectingTrianglesLocal(std::set<IntTriData0> * intTriData, const OBoundingBox & obb) const;
private:
	
	void splitNode(unsigned int nodeIndex, const Vector3 & avgPt, unsigned int depth);

	bool intersectOBB(std::set<IntTriData> * intTriData, unsigned int nodeIndex, const OBoundingBox & obb, const BoundingBox & aabb) const;
	bool intersectSegment(Vector3 * intPt, Vector3 * normal, unsigned int nodeIndex, const Vector3 & pt0, const Vector3 & pt1) const;

	void getIntersectingTriangles(std::set<IntTriData0> * intTriData, int nodeIndex, const OBoundingBox & obb, const BoundingBox & aabb) const;

	void uninit();
private:
	unsigned int maxDepth_;
	unsigned int numLeafTris_;
	std::vector<KDTreeNode> nodes_;
	
	Model * model_;
	SceneObject * obj_;
	
	JSObject * scriptObject_;
};

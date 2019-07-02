#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "MathStuff.h"
#include "Model.h"
#include "SpatialPartitioning.h"
///////////////////////////////////////////////////////////////////////////////
class Map;
class Model;
class ModelAnimation;
struct IntTriData;
class BoundingBoxObject;
class PathMesh;
class Path;

///////////////////////////////////////////////////////////////////////////////
class KDTree;
//#define CULLING 1


class Map : public SceneObject
{

public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "Map";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("2BCD9DFA-2D32-22A9-1A12-2615D3C3845F");
		return uuid;
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new Map(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	bool update(float sceneTime);

	virtual const Matrix & inverseTotalTransform() const;

	virtual bool asyncLoadUpdate();
	virtual bool isLoaded() const;

	Map();
	Map(const Map & rhs, ElementMapping * elementMapping);

	~Map();

	/**
	Default behavior is to set width and height to the size of the image file
	*/
	void setFileName(const std::string & fileName);
	const std::string & fileName() const {return fileName_;}

	void setPathMeshFileName(const std::string & fileName);
	const std::string & pathMeshFileName() const {return pathMeshFileName_;}

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "Map";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	bool pathFindPts(Path * path, const Vector3 & pStartPt, const Vector3 & pEndPt, float margin) const;
	bool randomPt(Vector3 * pt, float margin) const;

	int pathPtAlongSegment(Vector3 * pOut, 
		const Vector3 & startPt, const Vector3 & endPt) const;

	bool intersect(std::set<IntTriData> * intTriData, const BoundingBoxObject * bbObject) const;
	bool intersect(Vector3 * intPt, Vector3 * normal, const Vector3 & pt0, const Vector3 & pt1) const;
private:
	Map(const Map & rhs);
	Map & operator = (const Map & rhs);
	void create();


	
#ifdef CULLING
	void initRenderGeometry();
	void uninitRenderGeometry();
	void doNode(GfxRenderer * gl, int nodeIndex, const BoundingBox & aabb) const;
	bool isAabbOutsideFrustrum(const BoundingBox & aabb) const;
#endif
private:

	std::string fileName_;
	ModelVisualAttrib visualAttrib_;
	bool loadRequested_;
	
	bool pathMeshLoadReq_;	

	Model * model_;
	PathMesh * pathMesh_;
	
	KDTree * collisionTree_;
			
#ifdef CULLING
	BoundingBox aabb_;
	KDTree * renderTree_;
	GLuint * vertexBuffers_;

	struct MeshPartData
	{
		std::vector<unsigned short> indices;	
	};
	struct MeshData
	{
		std::vector<MeshPartData> meshPartData;
	};
	mutable std::vector<MeshData> drawMeshData_;

	struct TriListData
	{
		std::vector<unsigned short> indices;
	};
	struct NodeData
	{
		std::vector<TriListData> triListData; 
	};
	
	std::vector<NodeData> nodeData_;

	GLuint indexBuffer_;
	int indexBufferSize_;
	int maxTrisAtATime_;


	mutable Vector3 drawCamEye_;
	mutable Plane drawFrustrumPlanes_[6];
#endif
	

	std::string pathMeshFileName_;

};

///////////////////////////////////////////////////////////////////////////////

class Path
{
	friend class PathMesh;
public:
	static JSObject * scriptObjectProto(ScriptProcessor * s, JSObject * global);
	const std::vector<Vector3> & pathPts() const {return pathPts_;}
	void clear();
private:
	PathMesh * pathMesh_;
	std::vector<int> pathIndices_;
	std::vector<Vector3> pathPts_;

};
///////////////////////////////////////////////////////////////////////////////
struct PathMeshNode
{
	unsigned short p[3];
	std::vector<int> adjNodes;

	bool isAdjacentTo(const PathMeshNode & rhs) const;	

	float cumArea;
	mutable float fScore, gScore;
};

class PathMesh
{
public:
	PathMesh();
	/**
	Takes ownership of pathModel
	*/
	void build(Map * map, Model * pathModel);

	/**
	return a random point on the path mesh
	*/
	bool randomPt(int * nodeIndex, Vector3 * randPt, float margin) const;

	bool pathFind(std::vector<int> * pathNodeIndices, int startNodeIndex, int endNodeIndex) const;

	bool pathFindPts(Path * path, const Vector3 & pStartPt, const Vector3 & pEndPt, float margin) const;

	/**	
	gets point along startPt - endPt segment that is closest to endPt and still 
	in line of sight of startPt.
	(startPt, endPt projected onto path mesh).
	*/
	int pathPtAlongSegment(Vector3 * pOut, 
		const Vector3 & startPt, const Vector3 & endPt) const;

	~PathMesh();
private:
	void reconstructPath(std::vector<int> * pathNodeIndices, const std::map<int, int> & cameFrom, int curIndex) const;
	float dist(int startIndex, int endIndex) const;
	float calcH(int startNodeIndex, int endNodeIndex) const;

	Vector3 nodeCenterPt(int nodeIndex) const;
	int ptToNode(Vector3 * pt) const;

	/**
	Convert a path of node indices to a path of points
	*/
	void indexToPtPath(Path * path, const std::vector<int> & pathNodeIndices,
		const Vector3 & startPt, const Vector3 & endPt, float margin) const;

	/**
	Straighten (make less blocky and more direct) path computed by indexToPtPath
	*/
	void straightenPath(std::vector<Vector3> * pathPts, const std::vector<int> & pathNodeIndices, float margin) const;
	void doPathMargins(std::vector<Vector3> * pathPts, 
		const std::vector<int> & pathNodeIndices, float margin) const;

	void getAdjEdge(int * e0, int * e1, int node1, int node2) const;
	bool isSameEdge(int a0, int a1, int b0, int b1) const;


	bool checkMargin(const Vector3 & pt, int triIndex, int fromEdge0, int fromEdge1, float margin) const;

private:
	std::vector<Vector3> verts_;
	std::vector<PathMeshNode> nodes_;
	float totalArea_;

	Map * map_;
	Model * pathModel_;
	KDTree * kdTree_;
	typedef std::map<IntTriData0, int> TriMap;
	TriMap triMap_;
};
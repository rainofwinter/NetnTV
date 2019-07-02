#pragma once
#include "MathStuff.h"
#include "SceneObject.h"
#include "ModelKeyFrameChannel.h"

class Material;
class GfxRenderer;
class MeshPart;
class Model;
class BlendShape;

///////////////////////////////////////////////////////////////////////////////

class VertexDataDelta
{
public:
	Vector3 positionDelta;
	Vector3 normalDelta;
	Vector3 tangentDelta;

	VertexDataDelta operator + (const VertexDataDelta & vd) const
	{
		VertexDataDelta ret;
		ret.positionDelta = positionDelta + vd.positionDelta;
		ret.normalDelta = normalDelta + vd.normalDelta;
		ret.tangentDelta = tangentDelta + vd.tangentDelta;
		return ret;
	}

	VertexDataDelta operator - (const VertexDataDelta & vd) const
	{
		VertexDataDelta ret;
		ret.positionDelta = positionDelta - vd.positionDelta;
		ret.normalDelta = normalDelta - vd.normalDelta;
		ret.tangentDelta = tangentDelta - vd.tangentDelta;
		return ret;
	}
};

inline VertexDataDelta operator * (float t, const VertexDataDelta & vd)
{
	VertexDataDelta ret;
	ret.positionDelta = t*vd.positionDelta;
	ret.normalDelta = t*vd.normalDelta;
	ret.tangentDelta = t*vd.tangentDelta;
	return ret;
}

///////////////////////////////////////////////////////////////////////////////

class VertexData
{
public:
	Vector3 position;
	Vector2 texCoords;
	Vector3 normal;
	Vector3 tangent;
	float binormalFactor;

	VertexData & operator += (const VertexDataDelta & vd)
	{
		position += vd.positionDelta;
		normal += vd.normalDelta;
		tangent += vd.tangentDelta;
		return *this;
	}	
};

///////////////////////////////////////////////////////////////////////////////

class SkinningData
{
public:
	unsigned int jointIndices[4];
	float jointWeights[4];
};


///////////////////////////////////////////////////////////////////////////////

class Mesh
{
	friend class Model;
	friend class MeshPart;

public:
	struct EdgeIndices
	{
		unsigned short v0, v1;
	};

public:
	
	Mesh();
	Mesh(const Mesh & rhs);	
	~Mesh();

	Mesh & setVertices(const std::vector<VertexData> & vertexData);	
	const std::vector<VertexData> & vertices() const {return deformedVertices_;}
	
	Mesh & addMeshPart(
		const std::vector<unsigned short> & indices, Material * material);

	void addBlendShape(BlendShape * blendShape);

	void setSkinningData(const std::vector<SkinningData> & skinningData)
	{skinningData_ = skinningData;}

	void setSkinningDataIndices(const std::vector<unsigned int> & indices)
	{skinningDataIndices_ = indices;}

	void setJointIndices(const std::vector<unsigned int> & jointIndices) 
	{jointIndices_ = jointIndices;}

	void setGlobalInvBindPoseMats(const std::vector<Matrix> & matrices)
	{globalInvBindPoseMats_ = matrices;}
	
	void setParentModel(Model * model) {model_ = model;}

	void setNeedsVertexDeformation(bool val) {needsVertexDeformation_ = val;}
	void init(GfxRenderer * gl);
	void uninit();

	/**
	@return whether successful
	*/
	bool setTransformMatrix(const Matrix & transform);

	void drawBare(GfxRenderer * gl) const;
	void drawBare(GfxRenderer * gl, unsigned int meshPartIndex) const;
	
	virtual void draw(GfxRenderer * gl) const;
	void draw(GfxRenderer * gl, bool alphaBlended) const;
	void drawWireframe(GfxRenderer * gl) const;

	virtual BoundingBox extents() const;

	const Matrix & transform() const {return transform_;}
	bool intersect(float * t, const Ray & ray);
	bool intersect(Vector3 * intPt, const Ray & ray);
	bool intersectLine(float * t, const Vector3 & a, const Vector3 & b);
	bool intersectLine(Vector3 * intPt, const Vector3 & a, const Vector3 & b);

	unsigned char version() const {return 0;}
	void write(Writer & writer) const;
	void read(Reader & reader, unsigned char);

	
	const std::vector<MeshPart *> & meshParts() const {return meshParts_;}

	void setTransform(const Matrix & transform)
	{
		setTransformMatrix(transform);
	}

	void setTime(float time);

	/**
	Deform vertices appropriately based on blendshapes, joints, etc.
	Caution: joints list of model must have correct globalTransforms set.
	*/
	bool doVertexDeformations() const;

	void setId(const std::string & id) {id_ = id;}
	const std::string & id () const {return id_;}

	void setEdgeIndices(const std::vector<EdgeIndices> & edgeIndices)
	{
		edgeIndices_ = edgeIndices;
	}

	int numTriangles() const;

private:

	void create();
	Mesh & operator =(const Mesh &);

	BoundingBox computeExtents() const;


	void setAnimationChannel(ModelKeyFrameChannel<Matrix> * channel)
	{
		animationChannel_ = channel;
	}

	ModelKeyFrameChannel<Matrix> * animationChannel() const 
	{
		return animationChannel_;
	}

	//-------------------------------------------------------------------------
	mutable std::vector<VertexData> deformedVertices_;	
	mutable BoundingBox extents_;
	mutable bool needsVertexDeformation_;
	mutable std::vector<Matrix> jointTransforms_;

	/**
	skinningDataIndices_[i] is the index into skinningData_ for vertices_[i]
	*/
	std::vector<unsigned int> skinningDataIndices_;
	std::vector<SkinningData> skinningData_;
	
	///backup of the original vertices (may need to be restored during 
	///animation / deformation)
	std::vector<VertexData> origVertices_;
	//-------------------------------------------------------------------------
	std::vector<MeshPart *> meshParts_;

	std::vector<BlendShape *> blendShapes_;

	Matrix transform_;

	/**
	The total list of joints is stored in the Model class that contains these
	Mesh objects. the jointIndices in skinningData_ index into 
	jointIndices_ which in turn indexes into the joint list in the parent Model
	object.

	globalInvBindMats_[i] is the inverse bind pose transform matrix for 
	this mesh and the joint corresponding to jointIndices_[i]
	*/
	std::vector<unsigned int> jointIndices_;
	std::vector<Matrix> globalInvBindPoseMats_;

	GLuint vertexBuffer_, indexBuffer_;

	///The parent model
	Model * model_;

	GfxRenderer * gl_;

	std::string id_;

	ModelKeyFrameChannel<Matrix> * animationChannel_;

	std::vector<EdgeIndices> edgeIndices_;
	
};


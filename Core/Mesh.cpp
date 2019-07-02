#include "stdafx.h"
#include "Mesh.h"
#include "MeshPart.h"
#include "GfxRenderer.h"
#include "Exception.h"
#include "Joint.h"
#include "Reader.h"
#include "Writer.h"
#include "Model.h"
#include "BlendShape.h"

//TODO 
/*
It's wasteful to keep both deformedVertices_ and origVertices_ populated at all
times
*/


void Mesh::create()
{
	vertexBuffer_ = indexBuffer_ = 0;
	transform_ = Matrix::Identity();
	needsVertexDeformation_ = true;
	gl_ = 0;
}

Mesh::Mesh()
{	
	create();
}

Mesh::Mesh(const Mesh & rhs)
{
	create();
	origVertices_ = rhs.origVertices_;
	deformedVertices_ = rhs.deformedVertices_;
	extents_ = rhs.extents_;
	BOOST_FOREACH(MeshPart * rhsMeshPart, rhs.meshParts_)
	{
		meshParts_.push_back(new MeshPart(*rhsMeshPart));
	}
}

bool Mesh::setTransformMatrix(const Matrix & transform)
{
	//enforce identity transform on skinned mesh
	//(mesh transfomation animation/adjustment) has no effect
	if (!skinningData_.empty()) 
	{
		return false;
	}

	transform_ = transform;

	return true;
}

Mesh::~Mesh()
{
	uninit();
	for (int i = 0; i < (int)meshParts_.size(); ++i)
		delete meshParts_[i];

	BOOST_FOREACH(BlendShape * blendShape, blendShapes_)
		delete blendShape;
}


Mesh & Mesh::setVertices(const std::vector<VertexData> & vertexData)
{
	deformedVertices_ = vertexData;
	extents_ = computeExtents();
	origVertices_ = vertexData;
	return *this;
}

Mesh & Mesh::addMeshPart(const std::vector<unsigned short> & indices, Material * material)
{
	MeshPart * meshPart = new MeshPart();
	meshPart->setIndices(indices);
	meshPart->setMaterial(material);
	meshParts_.push_back(meshPart);
	return *this;
}

void Mesh::init(GfxRenderer * gl)
{	
	uninit();

	bool needsDeformation = false;
	if (skinningData_.size() > 0) needsDeformation = true;

	if (!vertexBuffer_)
	{
		glGenBuffers( 1, &vertexBuffer_);	
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
		if (needsDeformation)
			glBufferData(
				GL_ARRAY_BUFFER, 
				sizeof(VertexData) * deformedVertices_.size(), 
				&deformedVertices_[0], GL_DYNAMIC_DRAW);
		else
			glBufferData(
				GL_ARRAY_BUFFER, 
				sizeof(VertexData) * deformedVertices_.size(), 
				&deformedVertices_[0], GL_STATIC_DRAW);
	
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	for (int i = 0; i < (int)meshParts_.size(); ++i)
	{
		meshParts_[i]->init(gl);
	}	

	gl_ = gl;
}

void Mesh::uninit()
{
	if (!gl_) return;
	
	if (vertexBuffer_) 
	{		
		//glDisableVertexAttribArray(AttribPosition);
		glDeleteBuffers(1, &vertexBuffer_);		
		vertexBuffer_ = 0;
	}
	
	
	for (int i = 0; i < (int)meshParts_.size(); ++i)
	{
		meshParts_[i]->uninit();
	}
	
	gl_ = 0;
}

void Mesh::draw(GfxRenderer * gl) const
{
	//TODO revise this
	gl->pushMatrix();
	gl->multMatrix(transform_);	
			
	gl->enableVertexAttribArrayPosition();
	gl->enableVertexAttribArrayTexCoord();	

	gl->bindArrayBuffer(vertexBuffer_);


	gl->vertexAttribPositionPointer(sizeof(VertexData), 0);
	gl->vertexAttribTexCoordPointer(sizeof(VertexData), (char*)0 + sizeof(float)*3);	
	
		
	for (int i = 0; i < (int)meshParts_.size(); ++i)
	{
		meshParts_[i]->draw(gl, this, true);
	}

	gl->popMatrix();
}

void Mesh::drawBare(GfxRenderer * gl) const
{
	//TODO revise this
	gl->pushMatrix();
	gl->multMatrix(transform_);	

	gl->enableVertexAttribArrayPosition();
	gl->enableVertexAttribArrayTexCoord();	

	gl->bindArrayBuffer(vertexBuffer_);


	gl->vertexAttribPositionPointer(sizeof(VertexData), 0);
	gl->vertexAttribTexCoordPointer(sizeof(VertexData), (char*)0 + sizeof(float)*3);	


	for (int i = 0; i < (int)meshParts_.size(); ++i)
	{
		meshParts_[i]->draw(gl, this, false);
	}

	gl->popMatrix();
}

void Mesh::drawBare(GfxRenderer * gl, unsigned int meshPartIndex) const
{
	if (meshPartIndex >= (unsigned int)meshParts_.size()) return;

	gl->pushMatrix();
	gl->multMatrix(transform_);	

	gl->enableVertexAttribArrayPosition();
	gl->enableVertexAttribArrayTexCoord();	

	gl->bindArrayBuffer(vertexBuffer_);


	gl->vertexAttribPositionPointer(sizeof(VertexData), 0);
	gl->vertexAttribTexCoordPointer(sizeof(VertexData), (char*)0 + sizeof(float)*3);	
	
	meshParts_[meshPartIndex]->draw(gl, this, false);

	gl->popMatrix();
}

void Mesh::drawWireframe(GfxRenderer * gl) const
{
	gl->pushMatrix();
	gl->multMatrix(transform_);	
			
	gl->enableVertexAttribArrayPosition();
	gl->bindArrayBuffer(vertexBuffer_);

	if (needsVertexDeformation_)
	{
		doVertexDeformations();
		
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
		glBufferSubData(GL_ARRAY_BUFFER, 0,
			sizeof(VertexData) * deformedVertices_.size(), 
			&deformedVertices_[0]);
		needsVertexDeformation_ = false;
	}

	gl->vertexAttribPositionPointer(sizeof(VertexData), 0);
	
	gl->bindElementArrayBuffer(0);

	gl->applyCurrentShaderMatrix();

	glDrawElements(GL_LINES, edgeIndices_.size() * 2, GL_UNSIGNED_SHORT, &edgeIndices_[0]);

	gl->popMatrix();

}

void Mesh::draw(GfxRenderer * gl, bool alphaBlended) const
{
	gl->pushMatrix();
	gl->multMatrix(transform_);	
			
	gl->enableVertexAttribArrayPosition();
	gl->enableVertexAttribArrayTexCoord();	
	glEnableVertexAttribArray(AttribNormal);
	glEnableVertexAttribArray(AttribTangent);
	glEnableVertexAttribArray(AttribBinormalFactor);

	gl->bindArrayBuffer(vertexBuffer_);
	
	if (needsVertexDeformation_)
	{
		doVertexDeformations();
		
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
		glBufferSubData(GL_ARRAY_BUFFER, 0,
			sizeof(VertexData) * deformedVertices_.size(), 
			&deformedVertices_[0]);
		needsVertexDeformation_ = false;
	}
	
	gl->vertexAttribPositionPointer(sizeof(VertexData), 0);
	gl->vertexAttribTexCoordPointer(sizeof(VertexData), (char*)0 + sizeof(float)*3);	
	glVertexAttribPointer(AttribNormal, 3, GL_FLOAT, GL_FALSE, 
		sizeof(VertexData), (char*)0 + sizeof(float)*5);
	
	glVertexAttribPointer(AttribTangent, 3, GL_FLOAT, GL_FALSE, 
		sizeof(VertexData), (char*)0 + sizeof(float)*8);
	
	glVertexAttribPointer(AttribBinormalFactor, 1, GL_FLOAT, GL_FALSE, 
		sizeof(VertexData), (char*)0 + sizeof(float)*11);	
	
	for (int i = 0; i < (int)meshParts_.size(); ++i)
	{
		if (meshParts_[i]->doAlphaBlend() == alphaBlended)
			meshParts_[i]->draw(gl, this);
	}

	gl->popMatrix();

	gl->disableVertexAttribArrayPosition();
	gl->disableVertexAttribArrayTexCoord();
	glDisableVertexAttribArray(AttribNormal);
	glDisableVertexAttribArray(AttribTangent);
	glDisableVertexAttribArray(AttribBinormalFactor);
}

bool Mesh::intersect(float * tOut, const Ray & ray)
{	
	Matrix trans = transform_;
	
	float t;
	float minT = FLT_MAX;
	for (int i = 0; i < (int)meshParts_.size(); ++i)
	{
		if (meshParts_[i]->intersect(&t, this, ray))
		{
			if (t < minT)minT = t;
		}
	}
	
	if (minT < FLT_MAX)
	{		
		*tOut = t;
		return true;
	}

	return 0;
}

bool Mesh::intersectLine(float * tOut, const Vector3 & a, const Vector3 & b)
{	
	Matrix trans = transform_;

	float t;
	float minT = FLT_MAX;
	for (int i = 0; i < (int)meshParts_.size(); ++i)
	{
		if (meshParts_[i]->intersectLine(&t, this, a, b))
		{
			if (t < minT)minT = t;
		}
	}

	if (minT < FLT_MAX)
	{		
		*tOut = t;
		return true;
	}

	return 0;
}

bool Mesh::intersect(Vector3 * out, const Ray & ray)
{	
	float t;
	bool ret = intersect(&t, ray);
	
	if (ret)
	{
		*out = ray.origin + t * ray.dir;
		return true;
	}

	return 0;
}

bool Mesh::intersectLine(Vector3 * out, const Vector3 & a, const Vector3 & b)
{	
	float t;
	bool ret = intersectLine(&t, a, b);

	if (ret)
	{
		*out = a + t * (b - a);
		return true;
	}

	return 0;
}


void Mesh::setTime(float time)
{
	BOOST_FOREACH(BlendShape * blendShape, blendShapes_)
	{
		if (blendShape->setTime(time))
			needsVertexDeformation_ = true;
	}
}

int Mesh::numTriangles() const
{
	int numTris = 0;

	BOOST_FOREACH(MeshPart * meshPart, meshParts_)
	{
		numTris += meshPart->numTriangles();
	}

	return numTris;
}

bool Mesh::doVertexDeformations() const
{
	bool deformedFlag = false;

	memcpy(&deformedVertices_[0], &origVertices_[0], sizeof(VertexData) * deformedVertices_.size());

	BOOST_FOREACH(BlendShape * blendShape, blendShapes_)
	{
		blendShape->applyToMesh(&deformedVertices_);
		deformedFlag = true;
	}

	if (skinningData_.size() > 0)
	{
		std::vector<Joint *> joints = model_->joints();
		
		Matrix totalJointTransform;		

		int numElem = (int)skinningData_.size();
		for (int i = 0; i < numElem; ++i)
		{			
			const SkinningData & skinningData = skinningData_[i];

			Matrix & totalJointTransform = jointTransforms_[i];
			float * ptr = totalJointTransform.vals();
			for (int j = 0; j < 16; ++j) *(ptr++) = 0;

			int numJointWeights = 0;
			for (int j = 0; skinningData.jointWeights[j] != 0 && j < 4; ++j)
			{
				numJointWeights++;
				int localJointIndex = skinningData.jointIndices[j];
				int globalJointIndex  = jointIndices_[localJointIndex];	
				
				totalJointTransform += skinningData.jointWeights[j] * 
					(joints[globalJointIndex]->globalTransform() *
					globalInvBindPoseMats_[localJointIndex]);
			}

		}

		int numVerts = (int)deformedVertices_.size();
		for (int i = 0; i < numVerts; ++i)
		{
			const Matrix & transform = jointTransforms_[skinningDataIndices_[i]];
			deformedVertices_[i].position = transform.multiply(deformedVertices_[i].position);
			deformedVertices_[i].tangent = transform.multiply3x3(deformedVertices_[i].tangent);
			deformedVertices_[i].normal = transform.multiply3x3(deformedVertices_[i].normal);
		}

		deformedFlag = true;
		extents_ = computeExtents();
	}

	return deformedFlag;
}

BoundingBox Mesh::extents() const
{
	return extents_;
}

void Mesh::addBlendShape(BlendShape * blendShape)
{
	blendShapes_.push_back(blendShape);
}

void Mesh::write(Writer & writer) const
{
	writer.write(id_);
	writer.write(transform_);
	writer.write(origVertices_);

	writer.write(skinningDataIndices_);
	writer.write(skinningData_);

	writer.write(jointIndices_);
	writer.write(globalInvBindPoseMats_);
	writer.write(meshParts_);	

	writer.write(blendShapes_);

	writer.write(edgeIndices_);
}
void Mesh::read(Reader & reader, unsigned char version)
{
	reader.read(id_);
	reader.read(transform_);
	reader.read(origVertices_);
	deformedVertices_ = origVertices_;	
	extents_ = computeExtents();

	reader.read(skinningDataIndices_);
	reader.read(skinningData_);

	jointTransforms_.resize(skinningData_.size());
	
	reader.read(jointIndices_);
	reader.read(globalInvBindPoseMats_);	

	if (!skinningData_.empty())
	{		
		for (int i = 0; i < (int)globalInvBindPoseMats_.size(); ++i)
		{
			globalInvBindPoseMats_[i] *= transform_;
		}
		transform_ = Matrix::Identity();
	}

	reader.read(meshParts_);
	reader.read(blendShapes_);
	reader.read(edgeIndices_);
}

BoundingBox Mesh::computeExtents() const
{
	BoundingBox ret;

	ret.minPt = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	ret.maxPt = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (int i = 0; i < (int)deformedVertices_.size(); ++i)
	{
		Vector3 & vert = deformedVertices_[i].position;

		if (ret.maxPt.x < vert.x) ret.maxPt.x = vert.x;
		if (ret.maxPt.y < vert.y) ret.maxPt.y = vert.y;
		if (ret.maxPt.z < vert.z) ret.maxPt.z = vert.z;

		if (ret.minPt.x > vert.x) ret.minPt.x = vert.x;
		if (ret.minPt.y > vert.y) ret.minPt.y = vert.y;
		if (ret.minPt.z > vert.z) ret.minPt.z = vert.z;		
	}	

	return ret;

}
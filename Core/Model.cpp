#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Reader.h"
#include "FileUtils.h"
#include "Global.h"
#include "Joint.h"
#include "Scene.h"
#include "Camera.h"
#include "Reader.h"
#include "Writer.h"
#include "Exception.h"
#include "SpatialPartitioning.h"
#include "ModelAnimation.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

ModelVisualAttrib::ModelVisualAttrib(SceneObject * parentObject) 
{
	modelObj_ = parentObject;
	invSet_ = false;
}

void ModelVisualAttrib::onSetTransform()
{
	VisualAttrib::onSetTransform();
	setInv();

}

void ModelVisualAttrib::onGetTransform() const
{
	VisualAttrib::onGetTransform();
	if (!invSet_) setInv();

}

void ModelVisualAttrib::setInv() const
{
	invTotalTransform_ = (modelObj_->parentTransform() * transformMatrix_).inverse();
	invSet_ = true;
}

void ModelVisualAttrib::write(Writer & writer) const
{
	writer.writeParent<VisualAttrib>(this);
}

void ModelVisualAttrib::read(Reader & reader, unsigned char version)
{
	reader.readParent<VisualAttrib>(this);	
}

///////////////////////////////////////////////////////////////////////////////

unsigned int cMagicNumber = 3247115;

void Model::write(const char * fname, Model * model)
{
	Writer writer(fname, 0);
	writer.write(cMagicNumber);
	writer.write(model);
}

Model * Model::read(const char * directory, FILE * file)
{
	Model * model;
	Reader reader(file);
	unsigned int magicNumber;
	reader.read(magicNumber);
	if (magicNumber != cMagicNumber) throw Exception("Incorrect file format");
	reader.read(model);
	model->convertToAbsFileNames(directory);
	return model;
}

Model * Model::read(const char * directory, const char * fname)
{
	Model * model = 0;	
	FILE * file = 0;
	std::string absFileName = getAbsFileName(directory, fname);

	BOOST_SCOPE_EXIT( (&file) )
    {
		if (file) fclose(file);
    } BOOST_SCOPE_EXIT_END

	file = fopen(absFileName.c_str(), "rb");
	if (!file) throw Exception("Could not open file");
	model = read(directory, file);

	return model;
}

void Model::create()
{
	gl_ = 0;
}

Model::Model()
{
	create();
}

Model::Model(const Model & rhs)
{
	create();

	map<Texture *, Texture *> oldToNewTextures;

	textures_.reserve(rhs.textures_.size());
	BOOST_FOREACH(Texture * obj, rhs.textures_)
	{
		Texture * newObj = new Texture(*obj);
		oldToNewTextures[obj] = newObj;
		textures_.push_back(newObj);
	}

	materials_.reserve(rhs.materials_.size());
	BOOST_FOREACH(Material * obj, rhs.materials_)
	{
		Material * newObj = new Material(*obj);
		newObj->SetDiffuseMap(oldToNewTextures[newObj->diffuseMap()]);
		newObj->setNormalMap(oldToNewTextures[newObj->normalMap()]);
		newObj->setSpecularMap(oldToNewTextures[newObj->specularMap()]);
		materials_.push_back(newObj);
	}

	meshes_.reserve(rhs.meshes_.size());
	BOOST_FOREACH(Mesh * obj, rhs.meshes_)
	{
		Mesh * newObj = new Mesh(*obj);
		meshes_.push_back(newObj);
	}

	joints_.reserve(rhs.joints_.size());
	BOOST_FOREACH(Joint * obj, rhs.joints_)
	{
		Joint * newObj = new Joint(*obj);
		joints_.push_back(newObj);
	}

	fps_ = rhs.fps_;
	startTime_ = rhs.startTime_;
	endTime_ = rhs.endTime_;

	computeTempAnimData();
}

Model::~Model()
{

	BOOST_FOREACH(Texture * texture, textures_) delete texture;
	textures_.clear();

	BOOST_FOREACH(Mesh * mesh, meshes_) delete mesh;
	meshes_.clear();

	BOOST_FOREACH(Material * material, materials_) delete material;
	materials_.clear();

	BOOST_FOREACH(Joint * joint, joints_) delete joint;
	joints_.clear();

	BOOST_FOREACH(ModelKeyFrameChannel<Matrix> * elem, jointAnimationChannels_)
		delete elem;
	jointAnimationChannels_.clear();

	BOOST_FOREACH(ModelKeyFrameChannel<Matrix> * elem, transformAnimationChannels_)
		delete elem;
	transformAnimationChannels_.clear();

}

void Model::referencedFiles(std::vector<std::string> * refFiles) const
{
	for (int i = 0; i < (int)textures_.size(); ++i)
	{
		refFiles->push_back(textures_[i]->fileName());		
	}

}

int Model::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{	
	for (int i = 0; i < (int)textures_.size(); ++i)
	{
		textures_[i]->setFileName(refFiles[index++]);		
	}
	return index;
}




void Model::init(GfxRenderer * gl)
{
	uninit();

	BOOST_FOREACH(Texture * texture, textures_)
		texture->init(gl);

	BOOST_FOREACH(Material * material, materials_)
		material->init(gl);

	BOOST_FOREACH(Mesh * mesh, meshes_)
		mesh->init(gl);

	gl_ = gl;
}

void Model::uninit()
{
	if (!gl_) return;

	BOOST_FOREACH(Texture * texture, textures_)
		texture->uninit();

	BOOST_FOREACH(Material * material, materials_)
		material->uninit();

	BOOST_FOREACH(Mesh * mesh, meshes_)
		mesh->uninit();	
	
	gl_ = 0;
}


bool Model::update(bool needsDeform)
{	
	bool needRedraw = false;

	if (needsDeform)
	{
		computeGlobalJointTransforms();
		BOOST_FOREACH(Mesh * mesh, meshes_)
			mesh->setNeedsVertexDeformation(true);			
	}

	needRedraw = true;

	return needRedraw;
}

bool Model::asyncLoadUpdate()
{
	bool everythingLoaded = true;

	BOOST_FOREACH(Texture * texture, textures_)
	{
		everythingLoaded &= texture->asyncLoadUpdate();
	}
	return everythingLoaded;	
}

bool Model::isLoaded() const
{
	bool everythingLoaded = true;

	BOOST_FOREACH(Texture * texture, textures_)
	{
		everythingLoaded &= texture->isLoaded();
	}
	return everythingLoaded;
}


void Model::drawWireframe(GfxRenderer * gl) const
{	
	BOOST_FOREACH(Mesh * mesh, meshes_)	
		mesh->drawWireframe(gl);
}

void Model::drawObject(GfxRenderer * gl, Model::DrawAlphaMode drawAlpha) const
{	
	int prevDepthFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);
	if (prevDepthFunc == GL_ALWAYS) gl->clearZBuffer();
	glDepthFunc(GL_LEQUAL);

	if (drawAlpha == DrawAlpha)
	{
		gl->setNeedAlphaBlending(true);
		BOOST_FOREACH(Mesh * mesh, meshes_)	
			mesh->draw(gl, true);
	}
	else if (drawAlpha == DrawOpaque)
	{
		gl->setNeedAlphaBlending(false);
		BOOST_FOREACH(Mesh * mesh, meshes_)	
			mesh->draw(gl, false);
	}
	else
	{
		gl->setNeedAlphaBlending(false);
		BOOST_FOREACH(Mesh * mesh, meshes_)		
			mesh->draw(gl, false);

		gl->setNeedAlphaBlending(true);
		BOOST_FOREACH(Mesh * mesh, meshes_)		
			mesh->draw(gl, true);
	}
	

	glDepthFunc(prevDepthFunc);
	gl->setNeedAlphaBlending(true);
}

BoundingBox Model::extents() const
{
	
	BoundingBox ret;
	BOOST_FOREACH(Mesh * mesh, meshes_)
	{
		ret = ret.unite(mesh->extents().transform(mesh->transform()));
	}
	return ret;
}



Texture * Model::equivalentTexture(const Texture & testTexture) const
{
	BOOST_FOREACH(Texture * texture, textures_)
	{
		if (texture->isEquivalent(testTexture)) return texture;
	}

	return 0;
}

Material * Model::equivalentMaterial(const Material & testMaterial) const
{
	BOOST_FOREACH(Material * material, materials_)
	{
		if (material->isEquivalent(testMaterial)) return material;
	}

	return 0;
}


void Model::addTexture(Texture * texture)
{
	textures_.push_back(texture);
}

void Model::addMaterial(Material * material)
{
	materials_.push_back(material);
}

void Model::addMesh(Mesh * mesh)
{
	meshes_.push_back(mesh);
}

void Model::addJoint(Joint * joint)
{
	joints_.push_back(joint);
}

void Model::addJointAnimationChannel(ModelKeyFrameChannel<Matrix> *  channel)
{
	jointAnimationChannels_.push_back(channel);
}

void Model::addTransformAnimationChannel(ModelKeyFrameChannel<Matrix> * channel)
{
	transformAnimationChannels_.push_back(channel);
}

void Model::computeGlobalJointTransforms()
{
	int numJoints = (int)joints_.size();
	for (int i = 0; i < numJoints; ++i)
	{		
		Joint * joint = joints_[i];
		if (joints_[i]->hasParent())
		{
			Joint * parent = joints_[joint->parent()];
			joint->setGlobalTransform(parent->globalTransform() * joint->transform());
		}
		else
		{
			joint->setGlobalTransform(joint->transform());
		}


	}
}

void Model::computeTempAnimData()
{
	int numJoints = (int)joints_.size();
	
	for (int i = 0; i < numJoints; ++i)
	{
		joints_[i]->children().clear();		
		joints_[i]->setAnimationChannel(NULL);
	}

	for (int i = 0; i < numJoints; ++i)
	{
		if (joints_[i]->hasParent())
		{
			Joint * parent = joints_[joints_[i]->parent()];
			parent->children().push_back(i);
		}
	}

	for (int i = 0; i < (int)jointAnimationChannels_.size(); ++i)
	{
		int jointIndex = jointAnimationChannels_[i]->targetObjectIndex();
		joints_[jointIndex]->setAnimationChannel(jointAnimationChannels_[i]);
	}

	for (int i = 0; i < (int)meshes_.size(); ++i)
	{
		meshes_[i]->setAnimationChannel(NULL);
	}

	for (int i = 0; i < (int)transformAnimationChannels_.size(); ++i)
	{
		int meshIndex = transformAnimationChannels_[i]->targetObjectIndex();
		meshes_[meshIndex]->setAnimationChannel(transformAnimationChannels_[i]);
	}

	
}

void Model::animationSetTime(const ModelAnimation * animation, float animTime, bool * jointTransformed)
{
	*jointTransformed = false;
	animJoints_.clear();

	if (animation->restrictJoints)
	{
		if (animation->jointIndex < joints_.size())
			animJoints_.push_back(animation->jointIndex);

		for (int i = 0; i < (int)animJoints_.size(); ++i)
		{		
			if (animation->isExcludedJointIndex(animJoints_[i])) continue;
			vector<int> & children = joints_[animJoints_[i]]->children();
			for (int j = 0; j < (int)children.size(); ++j)
			{
				animJoints_.push_back(children[j]);
			}
		}

		BOOST_FOREACH(int jointIndex, animJoints_)
		{
			Joint * joint = joints_[jointIndex];
			ModelKeyFrameChannel<Matrix> * channel = joint->animationChannel();
			if (!channel) continue;
			*jointTransformed |= channel->setTime(animTime);
			joint->setTransform(channel->value());
		}
	}
	else
	{
		BOOST_FOREACH(Joint * joint, joints_)
		{
			ModelKeyFrameChannel<Matrix> * channel = joint->animationChannel();
			if (!channel) continue;
			*jointTransformed |= channel->setTime(animTime);
			joint->setTransform(channel->value());
		}
	}

	if (animation->restrictMeshes)
	{
		BOOST_FOREACH(int meshIndex, animation->meshIndices)
		{		
			Mesh * mesh = meshes_[meshIndex];
			ModelKeyFrameChannel<Matrix> * channel = mesh->animationChannel();
			if (!channel) continue;
			channel->setTime(animTime);
			mesh->setTransform(channel->value());
			mesh->setTime(animTime);
		}
	}
	else
	{
		BOOST_FOREACH(Mesh * mesh, meshes_)
		{				
			ModelKeyFrameChannel<Matrix> * channel = mesh->animationChannel();
			if (!channel) continue;
			channel->setTime(animTime);
			mesh->setTransform(channel->value());
			mesh->setTime(animTime);
		}
	}	
}

void Model::convertToAbsFileNames(const char * directory)
{
	BOOST_FOREACH(Texture * texture, textures_)
	{
		texture->setFileName(getAbsFileName(directory, texture->fileName()));
	}
}


bool Model::intersect(Vector3 * intPt, const Ray & ray)
{		
	float t;
	float minT = FLT_MAX;
	for (int i = 0; i < (int)meshes_.size(); ++i)
	{
		Matrix invMatrix = meshes_[i]->transform().inverse();
		Ray invRay;
		Vector3 b = invMatrix * (ray.origin + ray.dir);
		invRay.origin = invMatrix * ray.origin;
		invRay.dir = b - invRay.origin;
		if (meshes_[i]->intersect(&t, invRay))
		{			
			if (t < minT)minT = t;
		}
	}
	
	if (minT < FLT_MAX)
	{
		*intPt = ray.origin + t*ray.dir;
		return true;
	}

	return 0;
}

bool Model::intersect(Vector3 * intPt, const Ray & ray, const Matrix & transform)
{		
	float t;
	float minT = FLT_MAX;
	Matrix invModelTrans = transform.inverse();
	for (int i = 0; i < (int)meshes_.size(); ++i)
	{
		Matrix invMatrix = meshes_[i]->transform().inverse() * invModelTrans;
		Ray invRay;
		Vector3 b = invMatrix * (ray.origin + ray.dir);
		invRay.origin = invMatrix * ray.origin;
		invRay.dir = b - invRay.origin;
		if (meshes_[i]->intersect(&t, invRay))
		{			
			if (t < minT)minT = t;
		}
	}
	
	if (minT < FLT_MAX)
	{
		*intPt = ray.origin + t*ray.dir;
		return true;
	}

	return 0;
}

void Model::write(Writer & writer) const
{
	writer.write(textures_, "textures");
	writer.write(materials_, "materials");
	writer.write(meshes_, "meshes");	
	writer.write(joints_, "joints");

	writer.write(fps_);
	writer.write(startTime_);
	writer.write(endTime_);

	writer.write(jointAnimationChannels_);
	writer.write(transformAnimationChannels_);
}

void Model::read(Reader & reader, unsigned char version)
{
	reader.read(textures_);
	BOOST_FOREACH(Texture * texture, textures_)	
		texture->setForModel(true);
	
	reader.read(materials_);
	reader.read(meshes_);	
	BOOST_FOREACH(Mesh * mesh, meshes_) mesh->setParentModel(this);

	reader.read(joints_);
	
	reader.read(fps_);
	reader.read(startTime_);
	reader.read(endTime_);

	reader.read(jointAnimationChannels_);
	reader.read(transformAnimationChannels_);

	computeGlobalJointTransforms();
	computeTempAnimData();
	
}



int Model::startFrame() const
{
	return (int)(startTime_ * fps_);
}

int Model::endFrame() const
{
	return (int)(endTime_ * fps_);
}

void Model::applyAnimation(const ModelAnimation & animation, float time)
{
	bool animationJointTransformed;	
	animationSetTime(&animation, time, &animationJointTransformed);


	if (animationJointTransformed) update(animationJointTransformed);	
}
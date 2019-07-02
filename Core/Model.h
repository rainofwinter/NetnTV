#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "Texture.h"
#include "ModelKeyFrameChannel.h"

///////////////////////////////////////////////////////////////////////////////
class ModelVisualAttrib : public VisualAttrib
{
	friend class ModelFile;
public:
	
	ModelVisualAttrib(SceneObject * parentObject);

	virtual void onGetTransform() const;
	virtual void onSetTransform();

	unsigned char version() const {return 0;}
	void write(Writer & writer) const;
	void read(Reader & reader, unsigned char);



	/**
	Like setTransform but without calling onSetTransform
	*/
	void setTransformDirect(const Transform & transform)
	{
		transform_ = transform;
		transformMatrix_ = transform.computeMatrix();
	}

	const Matrix & inverseTotalTransform() const {onGetTransform(); return invTotalTransform_;}
	void setModelObject(SceneObject * obj) {modelObj_ = obj;}

private:

	void setInv() const;
	SceneObject * modelObj_;
	mutable bool invSet_;
	mutable Matrix invTotalTransform_;
};
///////////////////////////////////////////////////////////////////////////////
class Mesh;
class Material;
class Joint;
class ModelAnimation;

class Model
{
	friend class ModelFile;
public:
	
	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	bool update(bool needsDeform);


	bool asyncLoadUpdate();
	bool isLoaded() const;

	static void write(const char * fname, Model * model);
	static Model * read(const char * directory, const char * fname);
	static Model * read(const char * directory, FILE * file);

	Model();
	Model(const Model & rhs);

	~Model();

	void drawWireframe(GfxRenderer * gl) const;

	enum DrawAlphaMode
	{
		DrawAlpha,
		DrawOpaque,
		DrawBoth
	};
	void drawObject(GfxRenderer * gl, DrawAlphaMode drawAlpha = DrawOpaque) const;
		
	void init(GfxRenderer * gl);
	void uninit();

	BoundingBox extents() const;

	Texture * equivalentTexture(const Texture & texture) const;
	Material * equivalentMaterial(const Material & material) const;
	void addTexture(Texture * texture);
	void addMaterial(Material * material);
	void addMesh(Mesh * mesh);
	void addJoint(Joint * joint);

	void addJointAnimationChannel(ModelKeyFrameChannel<Matrix> *  channel);
	void addTransformAnimationChannel(ModelKeyFrameChannel<Matrix> * channel);

	void setStartTime(float time) {startTime_ = time;}
	void setEndTime(float time) {endTime_ = time;}
	void setFps(float fps) {fps_ = fps;}

	bool intersect(Vector3 * intPt, const Ray & ray);
	/**
	Transform model using the specified transform matrix before performing the
	intersection test
	*/
	bool intersect(Vector3 * intPt, const Ray & ray, const Matrix & transform);

	unsigned char version() const {return 0;}
	void write(Writer & writer) const;
	void read(Reader & reader, unsigned char);

	const std::vector<Texture *> & textures() const {return textures_;}
	const std::vector<Material *> & materials() const {return materials_;}
	const std::vector<Mesh *> & meshes() const {return meshes_;}
	const std::vector<Joint *> & joints() const {return joints_;}

	void computeGlobalJointTransforms();
	void computeTempAnimData();


	int startFrame() const;
	int endFrame() const;

	float endTime() const {return endTime_;}
	float startTime() const {return startTime_;}

	float fps() const {return fps_;}

	void applyAnimation(const ModelAnimation & animation, float time);

private:
	Model & operator = (const Model & rhs);
	void create();

	void animationSetTime(const ModelAnimation * animation, float animTime,
		bool * jointTransformed);
	
	void convertToAbsFileNames(const char * directory);

private:
	
	std::vector<Texture *> textures_;
	std::vector<Material *> materials_;
	std::vector<Mesh *> meshes_;	
	std::vector<Joint *> joints_;

	/**
	index into joints_
	*/
	std::vector< ModelKeyFrameChannel<Matrix> * > jointAnimationChannels_;

	/**
	index into meshes_
	*/
	std::vector< ModelKeyFrameChannel<Matrix> * > transformAnimationChannels_;

	float fps_;
	float startTime_;
	float endTime_;

	GfxRenderer * gl_;


	///temp fields

	/**
	list of indices of joints to apply an animation on 
	*/
	std::vector<int> animJoints_;
};


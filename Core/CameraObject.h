#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "Texture.h"
///////////////////////////////////////////////////////////////////////////////
class Camera;

class CameraObject;


class CameraVisualAttrib : public VisualAttrib
{
	friend class CameraObject;
public:
	
	CameraVisualAttrib(CameraObject * parentObject);

	virtual void onGetTransform() const;
	virtual void onSetTransform();

	unsigned char version() const {return 0;}
	void write(Writer & writer) const;
	void read(Reader & reader, unsigned char);

	/**
	Like setTransformMatrix but without calling onSetTransform
	*/
	void setTransformMatrixDirect(const Matrix & matrix)
	{
		transform_.setMatrix(matrix);
		transformMatrix_ = matrix;
	}

	/**
	Like setTransform but without calling onSetTransform
	*/
	void setTransformDirect(const Transform & transform)
	{
		transform_ = transform;
		transformMatrix_ = transform.computeMatrix();
	}

private:
	CameraObject * cameraObject_;
};


class CameraObject : public SceneObject
{
friend class CameraVisualAttrib;

public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "Camera";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("24DE0F12-0D32-C199-1A12-26A53B33845F");
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new CameraObject(*this, elementMapping);}

	bool update(float sceneTime);

	Ray unproject(const Vector2 & projCoords) const;

	virtual bool asyncLoadUpdate();
	virtual bool isLoaded() const;

	CameraObject();
	CameraObject(const CameraObject & rhs, ElementMapping * elementMapping);

	~CameraObject();

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 3;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "CameraObject";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	Camera * camera() const {return camera_;}

	void setSyncTransformNeeded(bool val) {syncTransformNeeded_ = val;}

	/**
	Compute the transformation matrix needed to match the camera object to 
	the internal camera_
	@return the computed transformation matrix
	*/
	Transform syncCameraTransform(const Transform & curTransform) const;


	/**
	Set the transform without automatically updating the state of camera_
	*/
	void setTransformDirect(const Transform & transform);
	
	void setTo2dArea(float centerX, float centerY,
		float width, float height, float fovY, float aspect);

	void setCamera(const Camera & camera);
	void setAspectRatio(const float & aspectRatio);

	void setFovY(const float & fovY);
	const float & fovY() const;
	
	void setNearPlane(const float & nearPlane);
	const float & nearPlane() const;

	void setFarPlane(const float & farPlane);
	const float & farPlane() const;

private:
	CameraObject(const CameraObject & rhs);
	CameraObject & operator = (const CameraObject & rhs);
	void create();	

private:

	CameraVisualAttrib visualAttrib_;
	Camera * camera_;
	mutable bool syncTransformNeeded_;
};
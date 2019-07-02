#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "Texture.h"
///////////////////////////////////////////////////////////////////////////////
class Light;

class LightObject;


class LightVisualAttrib : public VisualAttrib
{
	friend class LightObject;
public:
	
	LightVisualAttrib(LightObject * parentObject);

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
	LightObject * lightObject_;
};


class LightObject : public SceneObject
{
friend class LightVisualAttrib;

public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "Light";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("2DDEAF12-AD32-C199-1A12-26B53BDC817A");
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new LightObject(*this, elementMapping);}

	bool update(float sceneTime);

	Ray unproject(const Vector2 & projCoords) const;

	virtual bool asyncLoadUpdate();
	virtual bool isLoaded() const;

	LightObject();
	LightObject(const LightObject & rhs, ElementMapping * elementMapping);

	~LightObject();

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 4;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "LightObject";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	const Vector3 & lightDirection() const {return lightDirection_;}
	void setLightDirection(const Vector3 & lightDirection);
	void setSyncTransformNeeded(bool val) {syncTransformNeeded_ = val;}

	void setIntensity(const float & intensity) {intensity_ = intensity;}
	const float & intensity() const {return intensity_;}

	void setSpecularIntensity(const float & intensity) {specularIntensity_ = intensity;}
	const float & specularIntensity() const {return specularIntensity_;}

	/**
	Compute the transformation matrix needed to match the light object to 
	the internal light
	@return the computed transformation matrix
	*/
	Transform syncLightTransform(const Transform & curTransform) const;


	/**
	Set the transform without automatically updating the state of light
	*/
	void setTransformDirect(const Transform & transform);
	

private:
	LightObject(const LightObject & rhs);
	LightObject & operator = (const LightObject & rhs);
	void create();	

private:
	
	LightVisualAttrib visualAttrib_;

	/**
	Always unit
	*/
	Vector3 lightDirection_;
	mutable bool syncTransformNeeded_;
	
	float intensity_;
	float specularIntensity_;
};
#pragma once
#include "jsapi.h"
#include "MathStuff.h"

class Writer;
class XmlReader;
class XmlWriter;
class Reader;
/**
Represents a transform as separate rotation, scale, shear, translation, etc.
components.

Can convert to and from a matrix
*/
class Transform
{
	friend JSBool Transform_setProperty(
		JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp);
public:
	static JSObject * scriptObjectProto(ScriptProcessor * s);
	JSObject * createScriptObject(ScriptProcessor * s) const;
	static Transform fromJsonScriptObject(ScriptProcessor * s, JSObject * jsTransform);

	Transform();
	Transform(const Matrix & matrix);

	unsigned char version() const {return 0;}

	bool isIdentity() const;

	void setTranslation(float tx, float ty, float tz);
	void setTranslation(const Vector3 & t) {setTranslation(t.x, t.y, t.z);}

	void setRotation(float rx, float ry, float rz);
    void setRotation(const Vector3 & axis, float angle);
	void setRotation(const Matrix & rotationMatrix);
	
	void setScaling(float sx, float sy, float sz);
	void setScaling(const Vector3 & s) {setScaling(s.x, s.y, s.z);}
	/**
	Sets the pivot. But also adjusts translation so that the object remains in place
	after setting the new pivot.

	Parameters are in local coords
	*/
	void setPivot(float px, float py, float pz);
	void setPivot(const Vector3 & p) {setPivot(p.x, p.y, p.z);}
	
	void setShear(float xy, float xz, float yz);
    
	void setMatrix(const Matrix & matrix, bool preserveScaling = false);

//	void ConcatRotation(const Vector3 & axis, float angle);
	void concatRotation(const Matrix & rotMatrix);
	
	///Concatenate the specified rotation so that is applied BEFORE the 
	///existing rotation
	void concatRotationPre(const Matrix & rotMatrix);

	void concatenate(const Transform & transform);
	void concatenate(const Matrix & matrix, bool preserveScaling = false);

	
	const Vector3 & translation() const {return trans_;}
	const Vector3 & scaling() const {return scale_;}
	const Vector3 & rotation() const {return rot_;}
	/**
	Gets pivot point in local coordinates
	*/
	const Vector3 & pivot() const {return pivot_;}
	/**
	Gets the pivot point but in global coordinates    
	*/
	Vector3 globalPivot() const;
	float shearXY() const {return shxy_;}
	float shearXZ() const {return shxz_;}
	float shearYZ() const {return shyz_;}

	Matrix computeMatrix() const;
	Matrix rotationMatrix() const;

	/**
	Get the coordinate axes corresponding to this transform.
	They are always unit vectors.
	*/
	void getCoordAxes(
		Vector3 & xAxis, 
		Vector3 & yAxis, 
		Vector3 & zAxis) const;

	Transform lerp(const Transform & rhs, float t) const;

	bool operator == (const Transform & rhs) const;
	bool operator != (const Transform & rhs) const;

	void write(Writer & writer) const;
	void writeXml(XmlWriter & w) const;
	void read(Reader & reader, unsigned char);
	void readXml(XmlReader & r, xmlNode * parent);
	
private:
	///rotation angles for each axis
	Vector3 rot_;
	
	///translation deltas for each axis
	Vector3 trans_;
	
	///scale factors for each axis
	Vector3 scale_;
		
	///pivot of scaling and rotation
    //in local coordinates
	Vector3 pivot_;

	///shear xy
	float shxy_;
	
	///shear xz
	float shxz_;
	
	///shear yz
	float shyz_; 

};


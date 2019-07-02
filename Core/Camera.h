#pragma once
#include "MathStuff.h"
class Writer;
class Reader;
class XmlReader;
class XmlWriter;

class Camera
{
	friend class CameraVisualAttrib;
public:
	Camera();

	void SetAspectRatio(float aspectRatio) {aspectRatio_ = aspectRatio;}

	void SetTo2DArea(
		float centerX, float centerY,
		float width, float height, float fovY, float aspect);	

	void SetTo2DArea(
		float centerX, float centerY,
		float focusX, float focusY,
		float width, float height, float fovY, float aspect);	

	const Matrix & GetMatrix() const {return matrix_;}
	
	void unproject(const Vector2 & projCoords, Vector3 * a, Vector3 * b) const;
	static void unproject(const Matrix & inverseCameraMatrix, const Vector2 & projCoords, Vector3 * a, Vector3 * b);

	Ray unproject(const Vector2 & projCoords) const;
	Vector3 project(const Vector3 & worldCoord) const;

	const Vector3 & lookAt() const {return lookAt_;}
	void setLookAt(const Vector3 & lookAt);
	void setFovY(const float & fov);
	const float & fovY() const {return fovY_;}

	/**
	Moves both lookAt_ and eye_ so that lookAt_ ends up at the specified point
	*/
	void lookAtPoint(const Vector3 & pt);

	void setUp(const Vector3 & up);


	const Vector3 & eye() const {return eye_;}
	void setEye(const Vector3 & eye);

	/**
	* orbit camera (rotate about lookAtPos)
	* \param deltaX how much to rotate about local camera x axis
	* \param deltaY how much to rotate about world y axis
	*/
	void orbit(float deltaX, float deltaY);
	

	/**
	* moves eyePos toward or away from lookAtPos. 
	* \param distancePercentage factor to adjust distance from lookAtPos to 
	* eyePos by
	*/
	void dolly(float distancePercentage);

	void pan(const Vector3 & panDelta);

	Plane lookAtPlane() const;

	float lookDistance() const;
	Vector3 direction() const {return (lookAt_ - eye_).normalize();}

	void setAspectRatio(float ratio);
	const float & aspectRatio() const;

	void setCenter(float x, float y);

	unsigned char version() const {return 1;}
	void write(Writer & writer) const;
	void read(Reader & reader, unsigned char);
	const char * xmlTag() const {return "TextTable";}
	void writeXml(XmlWriter & w) const;
	void readXml(XmlReader & r, xmlNode * node);

	void readOld(const unsigned char * bytes);

	const Vector3 & up() const {return up_;}

	const float & nearPlane() const {return zNear_;}
	const float & farPlane() const {return zFar_;}

	void setNearPlane(const float & nearPlane);
	void setFarPlane(const float & farPlane);

	void dollyex(float distancePercentage);
	void initDefaultMag();
private:
	void updateMatrix();
	/*
	Like updateMatrix, but constrain eye and lookAt to integers
	*/
	//void updateMatrixInt();
private:

	Matrix matrix_;

	float fovY_;
	Vector3 eye_;
	Vector3 lookAt_;
	Vector3 up_;
	float aspectRatio_;
	float zNear_, zFar_;

	float postPerspX_, postPerspY_;

	float defaultMag_;
};
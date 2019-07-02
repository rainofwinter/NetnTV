#include "stdafx.h"
#include "Camera.h"
#include "Writer.h"
#include "Reader.h"
#include "Xml.h"
#include "Global.h"
#include "Document.h"

Camera::Camera()
{
	fovY_ = 45;
	aspectRatio_ = 1;
	up_ = Vector3(0, -1, 0);
	zFar_ = 100000;
	zNear_ = 1;
	postPerspX_ = postPerspY_ = 0.0f;

	eye_ = Vector3(0.0f, 0.0f, 0.0f);
	lookAt_ = Vector3(0.0f, 0.0f, 1.0f);
	defaultMag_ = 0;
}

float Camera::lookDistance() const
{
	return (lookAt_ - eye_).magnitude();
}

void Camera::SetTo2DArea(
	float centerX, float centerY, float width, float height, 
	float fovY, float aspect)
{
	SetTo2DArea(centerX, centerY, centerX, centerY, width, height, fovY, aspect);
}


void Camera::SetTo2DArea(
	float centerX, float centerY, float focusX, float focusY, 
	float width, float height, float fovY, float aspect)
{
	double d;
	double fH = height;
	if (width / height > aspect)
	{
		fH = width / aspect;
	}

	d = fH / 2 / tan(fovY / 2 * M_PI/180);

	eye_ = Vector3(focusX, focusY, (float)-d);
	lookAt_ = Vector3(focusX, focusY, 0);

	up_ = Vector3(0, -1, 0);

	zFar_ = (float)d*100;
	zNear_ = zFar_ / 1000;

	fovY_ = fovY;
	aspectRatio_ = aspect;

	postPerspX_ = 2.0f * (focusX - centerX) / width;
	postPerspY_ = 2.0f * (centerY - focusY) / height;

	//updateMatrixInt();
	updateMatrix();
}

void Camera::setEye(const Vector3 & eye)
{
	eye_ = eye;
	updateMatrix();
}

void Camera::setLookAt(const Vector3 & lookAt)
{
	lookAt_ = lookAt;
	updateMatrix();
}

void Camera::lookAtPoint(const Vector3 & pt)
{
	Vector3 moveDelta = pt - lookAt_;
	lookAt_ += moveDelta;
	eye_ += moveDelta;
	updateMatrix();
}

void Camera::setUp(const Vector3 & up)
{
	up_ = up;
	updateMatrix();
}

void Camera::setFovY(const float & fov)
{
	fovY_ = fov;
	updateMatrix();
}

void Camera::setNearPlane(const float & nearPlane)
{
	zNear_ = nearPlane;
	updateMatrix();
}

void Camera::setFarPlane(const float & farPlane)
{
	zFar_ = farPlane;
	updateMatrix();
}

void Camera::unproject(const Vector2 & projCoords, Vector3 * a, Vector3 * b) const
{
	Matrix invMat = matrix_.inverse();
	Vector3 vec(projCoords.x, projCoords.y, -1.0f);
	*a = invMat * vec;
	vec.z = 1;
	*b = invMat * vec;
}

void Camera::unproject(const Matrix & invCameraMatrix, const Vector2 & projCoords, Vector3 * a, Vector3 * b)
{
	Vector3 vec(projCoords.x, projCoords.y, -1.0f);
	*a = invCameraMatrix * vec;
	vec.z = 1;
	*b = invCameraMatrix * vec;
}

Ray Camera::unproject(const Vector2 & projCoords) const
{	
	Vector3 a, b;
	unproject(projCoords, &a, &b);
	return Ray(a, b - a);

}

Vector3 Camera::project(const Vector3 & worldCoord) const
{
	return matrix_ * worldCoord;	
}

void Camera::orbit(float deltaX, float deltaY)
{
	Vector3 toEye = eye_ - lookAt_;
	Vector3 camRight = up_ ^ toEye;

	Matrix rot = Matrix::Rotate(deltaX, camRight.x, camRight.y, camRight.z);
	toEye = rot * toEye;
	up_ = rot * up_;


	Vector3 yAxis(0, -1, 0);
	rot = Matrix::Rotate(deltaY, yAxis.x, yAxis.y, yAxis.z);
	toEye = rot * toEye;
	up_ = rot * up_;

	//must update up vector
	//make it so that it is in same planes as world yAxis and toEye
	//also make it so that it is orthogonal to toEye
	up_.projectOnToPlane(yAxis, toEye);
	up_.makeOrthonormalTo(toEye);

	eye_ = lookAt_ + toEye;

	updateMatrix();
}

void Camera::dolly(float distancePercentage)
{
	if (distancePercentage < 0) distancePercentage = 0;
	Vector3 toLookAtPos = lookAt_ - eye_;	

	toLookAtPos = distancePercentage * toLookAtPos;

	if (toLookAtPos.magnitude() < 1.1*zNear_)
	{
		toLookAtPos = lookAt_ - eye_;
		toLookAtPos.makeNormal();
		toLookAtPos = 1.1f*zNear_* toLookAtPos;
	}

	eye_ = lookAt_ - toLookAtPos;

	updateMatrix();
}

void Camera::pan(const Vector3 & panDelta)
{
	lookAt_ = lookAt_ + panDelta;
	eye_ = eye_ + panDelta;

	

	//updateMatrixInt();
	updateMatrix();
}


Plane Camera::lookAtPlane() const
{	
	Vector3 toEye = eye_ - lookAt_;
	toEye.makeNormal();
	return Plane::fromPointNormal(lookAt_, toEye);
}

void Camera::setAspectRatio(float ratio)
{
	aspectRatio_ = ratio;
	updateMatrix();
}

const float & Camera::aspectRatio() const
{
	return aspectRatio_;
}

void Camera::setCenter(float x, float y)
{
}

void Camera::updateMatrix()
{
	matrix_ = 
		Matrix::Translate(postPerspX_, postPerspY_, 0.0f) *
		Matrix::Perspective(fovY_, aspectRatio_, zNear_, zFar_) *
		Matrix::LookAt(
			eye_.x, -eye_.y, -eye_.z, 
			lookAt_.x, -lookAt_.y, -lookAt_.z, 
			up_.x, -up_.y, -up_.z) *
		Matrix::Rotate(M_PI, 1, 0, 0);
}

/*
void Camera::updateMatrixInt()
{
	Vector3 intEye(round(eye_.x), round(eye_.y), round(eye_.z));
	Vector3 intLookAt(round(lookAt_.x), round(lookAt_.y), round(lookAt_.z));;

	matrix_ = 
		Matrix::Translate(postPerspX_, postPerspY_, 0.0f) *
		Matrix::Perspective(fovY_, aspectRatio_, zNear_, zFar_) *
		Matrix::LookAt(
			intEye.x, -intEye.y, -intEye.z, 
			intLookAt.x, -intLookAt.y, -intLookAt.z, 
			up_.x, -up_.y, -up_.z) *
		Matrix::Rotate(M_PI, 1, 0, 0);
}
*/


void Camera::read(Reader & reader, unsigned char version)
{
	reader.read(matrix_);
	reader.read(fovY_);
	reader.read(eye_);
	reader.read(lookAt_);
	reader.read(up_);
	reader.read(aspectRatio_);
	reader.read(zNear_);
	reader.read(zFar_);
	reader.read(postPerspX_);
	reader.read(postPerspY_);
}

void Camera::readOld(const unsigned char * bytes)
{
	const unsigned char * ptr = bytes;
	matrix_ = *(Matrix *)ptr; ptr += sizeof(Matrix);
	fovY_ = *(float *)ptr; ptr += sizeof(float);
	eye_ = *(Vector3 *)ptr; ptr += sizeof(Vector3);
	lookAt_ = *(Vector3 *)ptr; ptr += sizeof(Vector3);
	up_ = *(Vector3 *)ptr; ptr += sizeof(Vector3);
	aspectRatio_ = *(float *)ptr; ptr += sizeof(float);
	zNear_ = *(float *)ptr; ptr += sizeof(float);
	zFar_ = *(float *)ptr; ptr += sizeof(float);
}

void Camera::write(Writer & writer) const
{
	writer.write(matrix_);
	writer.write(fovY_);
	writer.write(eye_);
	writer.write(lookAt_);
	writer.write(up_);
	writer.write(aspectRatio_);
	writer.write(zNear_);
	writer.write(zFar_);
	writer.write(postPerspX_);
	writer.write(postPerspY_);
}

void Camera::writeXml(XmlWriter & w) const
{
	std::ostringstream os1, os2, os3, os4;
	w.startTag("Matrix");
	os1 << matrix_._11 << ' ' << matrix_._12 << ' ' << matrix_._13 << ' ' << matrix_._14;
	w.writeTag("Row1", os1.str());
	os2 << matrix_._21 << ' ' << matrix_._22 << ' ' << matrix_._23 << ' ' << matrix_._24;
	w.writeTag("Row2", os2.str());
	os3 << matrix_._31 << ' ' << matrix_._32 << ' ' << matrix_._33 << ' ' << matrix_._34;
	w.writeTag("Row3", os3.str());
	os4 << matrix_._41 << ' ' << matrix_._42 << ' ' << matrix_._43 << ' ' << matrix_._44;
	w.writeTag("Row4", os4.str());
	w.endTag();

	w.writeTag("FovY", fovY_);
	w.writeTag("Eye", eye_);
	w.writeTag("LookAt", lookAt_);
	w.writeTag("Up", up_);
	w.writeTag("AspectRatio", aspectRatio_);
	w.writeTag("ZNear", zNear_);
	w.writeTag("ZFar", zFar_);
	w.writeTag("PostPerspX", postPerspX_);
	w.writeTag("PostPerspY", postPerspY_);
}

void Camera::readXml(XmlReader & r, xmlNode * parent)
{
	std::string str;

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{
		if (curNode->type != XML_ELEMENT_NODE) continue;		
		if (r.isNodeName(curNode, "Matrix"))
		{
			xmlNode * child = curNode->children;
			for( ; child ; child = child->next)
			{
				if(r.getNodeContentIfName(str, child, "Row1"))
					sscanf(str.c_str(), "%f %f %f %f", &matrix_._11, &matrix_._12, &matrix_._13, &matrix_._14);
				else if(r.getNodeContentIfName(str, child, "Row2"))
					sscanf(str.c_str(), "%f %f %f %f", &matrix_._21, &matrix_._22, &matrix_._23, &matrix_._24);
				else if(r.getNodeContentIfName(str, child, "Row3"))
					sscanf(str.c_str(), "%f %f %f %f", &matrix_._31, &matrix_._32, &matrix_._33, &matrix_._34);
				else if(r.getNodeContentIfName(str, child, "Row4"))
					sscanf(str.c_str(), "%f %f %f %f", &matrix_._41, &matrix_._42, &matrix_._43, &matrix_._44);
			}
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(fovY_, curNode, "FovY"));
		else if (r.getNodeContentIfName(eye_, curNode, "Eye"));
		else if (r.getNodeContentIfName(lookAt_, curNode, "LookAt"));
		else if (r.getNodeContentIfName(up_, curNode, "Up"));
		else if (r.getNodeContentIfName(aspectRatio_, curNode, "AspectRatio"));
		else if (r.getNodeContentIfName(zNear_, curNode, "ZNear"));
		else if (r.getNodeContentIfName(zFar_, curNode, "ZFar"));
		else if (r.getNodeContentIfName(postPerspX_, curNode, "PostPerspX"));
		else if (r.getNodeContentIfName(postPerspY_, curNode, "PostPerspY"));
	}
}

void Camera::dollyex(float distancePercentage)
{
	if (distancePercentage < 0) distancePercentage = 0;
	Vector3 toLookAtPos = lookAt_ - eye_;	

	toLookAtPos = distancePercentage * toLookAtPos;

	if (toLookAtPos.magnitude() < 1.1*zNear_)
	{
		toLookAtPos = lookAt_ - eye_;
		toLookAtPos.makeNormal();
		toLookAtPos = 1.1f*zNear_* toLookAtPos;
	}

	eye_ = lookAt_ - toLookAtPos;

	if (defaultMag_ > 0)
	{
		float mag = defaultMag_ / toLookAtPos.magnitude();
		Global::instance().setCurSceneMagnification(mag);
	}

	updateMatrix();
}

void Camera::initDefaultMag()
{
	Vector3 toLookAtPos = lookAt_ - eye_;

	if (toLookAtPos.magnitude() < 1.1*zNear_)
	{
		toLookAtPos = lookAt_ - eye_;
		toLookAtPos.makeNormal();
		toLookAtPos = 1.1f*zNear_* toLookAtPos;
	}

	defaultMag_ = toLookAtPos.magnitude();
	Global::instance().setCurSceneMagnification(1);
}
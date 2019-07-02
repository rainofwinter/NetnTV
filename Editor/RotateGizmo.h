#pragma once
#include "TransformGizmo.h"
#include "MathStuff.h"
#include "Transform.h"

class VisualAttrib;
class Mesh;
class Material;
class TransformTool;

class RotateGizmo : public TransformGizmo
{
public:
	enum Mode
	{
		X, Y, Z, NONE, CAMERA, FREE
	};
public:
	RotateGizmo(GLWidget * gl, TransformTool * transformTool);
	~RotateGizmo();

	virtual void init();
	virtual void uninit();

	virtual void setTransformAttrib(Transform * transform);

	virtual void mousePressEvent(QMouseEvent * event, bool transformableObjClicked);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void mouseDoublePressEvent(QMouseEvent * event);

	virtual void draw();

	virtual bool intersect(const QPoint & mousePos) const;

	Mode mode() const {return mode_;}

	virtual bool isActive() const {return mode_ != NONE;}

private:
	/**
	xCircleTransform_, ..., camCircleTransform_ must already be computed
	*/
	void computeInitRotatePlanes();
	/**
	mode_, xPlane_, ..., yPlane_ must already be computed
	*/
	void computeCurRotatePlane();


	Mode doHitTest(const Vector2 & mousePos) const;

	/**
	calculate parameters transforms coordinates etc for gizmo display
	*/
	void update();
	

private:
	Mode mode_;

	Transform initTransform_;
	float gizmoRadiusPixels_;
	float gizmoRadiusWorld_;
	float grabLen_;
	float centerSquareSize_;

	Vector3 origin_, xAxisTip_, yAxisTip_, zAxisTip_;
	float coneRad_, coneLen_;

	std::vector<Vector3> circleVerts_;
	
	///Original click location in coords
	Vector2 origMousePt_;

	Matrix xCircleTransform_, yCircleTransform_, zCircleTransform_, camCircleTransform_;
	Plane xPlane_, yPlane_, zPlane_, camPlane_;

	
	Plane rotatePlane_;
};